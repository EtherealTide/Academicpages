//
// Created by EtherealTide on 2025/7/10.
//
#include "stm32f1xx_hal.h" // change this according to your MCU series, this is for STM32F103C8T6
#include "key.h"

uint8_t key_flag[KEY_NUM]; // key flag, used to record the status of each key

uint8_t get_key_state(uint8_t key_index)
{
  /*
    this function detect the status of specified key
    Args:
      key_code: Designated detection key number
    Return:
      1 if key is down else 0
   */
  if (key_index == 1)
  {
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET)
    {
      return 1;
    }
  }
  else if (key_index == 2)
  {
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == GPIO_PIN_RESET)
    {
      return 1;
    }
  }
  else if (key_index == 3)
  {
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_RESET)
    {
      return 1;
    }
  }
  else if (key_index == 4)
  {
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET)
    {
      return 1;
    }
  }
  else if (key_index == 5)
  {
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET)
    {
      return 1;
    }
  }
  else if (key_index == 6)
  {
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_RESET)
    {
      return 1;
    }
  }
  return 0;
}

uint8_t check_key_state(uint8_t flag, uint8_t key_index)
{
  /*
    this function checks if the specified key state flag is set and clears it
    Args:
      flag: The key state flag to check (KEY_DOWN, KEY_UP, KEY_SINGLE, KEY_DOUBLE, KEY_LONG)
      key_index: The key index to check (1-6)
    Return:
      1 if the flag was set, 0 otherwise
 */
  if (flag & key_flag[key_index - 1])
  {
    // reset the flag
    key_flag[key_index - 1] &= ~flag;
    return 1; // the key state is checked
  }
  return 0; // the key state is not checked
}

void key_process(void)
{
  /*
    This function is the interface to the main function, which is called
    in the timer interrupt to set each flag bit of each case, and can be triggered
    by the timer interrupt every 1ms for instance.
 */
  static uint8_t S[KEY_NUM];              // status of each key
  static uint16_t time[KEY_NUM];          // time counter for each key
  static uint8_t curr_state[KEY_NUM];     // current state of each key
  static uint8_t prev_state[KEY_NUM];     // previous state of each key
  static uint8_t stable_state[KEY_NUM];   // stable state after debouncing
  static uint8_t debounce_count[KEY_NUM]; // debounce counter

  // Decrement time counters
  for (int i = 0; i < KEY_NUM; i++)
  {
    if (time[i] > 0)
    {
      time[i]--;
    }
  }

  // Key debouncing process
  for (int i = 0; i < KEY_NUM; i++)
  {
    prev_state[i] = stable_state[i];      // Save previous stable state
    curr_state[i] = get_key_state(i + 1); // Get current raw state

    // Debouncing logic
    if (curr_state[i] == stable_state[i])
    {
      // State is consistent, reset debounce counter
      debounce_count[i] = 0;
    }
    else
    {
      // State is inconsistent, start debounce counting
      debounce_count[i]++;
      if (debounce_count[i] >= DEBOUNCE_TIME)
      {
        // Debounce time reached, update stable state
        stable_state[i] = curr_state[i];
        debounce_count[i] = 0;
      }
    }

    // Update key flags (based on stable state)
    if (stable_state[i] == 1)
    {
      key_flag[i] |= KEY_HOLD; // key is being pressed
    }
    else
    {
      key_flag[i] &= ~KEY_HOLD; // key is not being pressed
    }

    // Detect key press and release events
    if (stable_state[i] == 1 && prev_state[i] == 0)
    {
      key_flag[i] |= KEY_DOWN; // key is pressed down
    }
    else if (stable_state[i] == 0 && prev_state[i] == 1)
    {
      key_flag[i] |= KEY_UP; // key is released
    }
  }

  // State machine processing
  for (int i = 0; i < KEY_NUM; i++)
  {
    key_process_state_machine(&S[i], &time[i], &stable_state[i], i);
  }
}

// state machine
void key_process_state_machine(uint8_t *S, uint16_t *time, uint8_t *stable_state, uint8_t key_index)
{
  /*
    this function implements the state machine for key processing to detect single, double, and long press
    Args:
      S: Pointer to the current state of the key
      time: Pointer to the time counter for the key
      stable_state: Pointer to the current stable state of the key (pressed or not)
      key_index: The index of the key being processed (0-5)
    Return:
      None
 */
  switch (*S)
  {
  case 0:                   // Idle state
    if (*stable_state == 1) // Use stable_state
    {
      *S = 1;            // Key pressed
      *time = LONG_TIME; // Set long press detection time
    }
    break;

  case 1:                   // Key pressed state
    if (*stable_state == 0) // Use stable_state
    {                       // Key released, not a long press
      *time = DOUBLE_TIME;  // Set double click detection time
      *S = 2;               // Enter single click waiting state
    }
    else if (*time == 0)
    {                                  // Long press time reached
      key_flag[key_index] |= KEY_LONG; // Set long press flag
      *S = 4;                          // Enter long press state
    }
    break;

  case 2: // Single click waiting state (waiting for possible double click)
    if (*time == 0)
    {                                    // Double click time reached, confirm as single click
      key_flag[key_index] |= KEY_SINGLE; // Set single click flag
      *S = 0;                            // Return to idle state
    }
    else if (*stable_state == 1)         // Use stable_state
    {                                    // Pressed again within waiting time, double click
      key_flag[key_index] |= KEY_DOUBLE; // Set double click flag
      *S = 3;                            // Enter double click state
    }
    break;

  case 3:                   // Double click state
    if (*stable_state == 0) // Use stable_state
    {
      *S = 0; // Return to idle state
    }
    break;

  case 4:                   // Long press state
    if (*stable_state == 0) // Use stable_state
    {
      *S = 0; // Return to idle state
    }
    break;

  default:
    *S = 0; // Abnormal state, reset to idle
    break;
  }
}