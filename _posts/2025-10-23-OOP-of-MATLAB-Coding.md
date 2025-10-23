---
title: "MATLAB类基础：面向对象编程"
date: 2025-10-23
permalink: /posts/2025/10/OOP-of-MATLAB-Coding/
tags:
  - MATLAB
  - 面向对象
  - 类
  - Teilscale
  - 软路由
categories:
  - Technical Documentation
excerpt: "使用MATLAB实现类，从而进行OOP编程，方便整个系统和工程的调试和维护"
collection: posts
---

欢迎！这个文档是为 MATLAB 新手准备的，专注于**类（Class）**的创建和使用。MATLAB 的类让你像写“蓝图”一样定义对象（比如一个“发射机”），包含数据（属性）和操作（方法）。我们用简单语言、步骤和例子解释，不涉及高级话题。

> **为什么学这个？** 类让代码更整洁、可复用。比如模拟无线信号时，一个类就能管理所有相关数据和计算，避免乱七八糟的变量。

## 1. 类的基本结构
MATLAB 类定义在单独的 `.m` 文件中，文件名 = 类名。核心是 `classdef` 块。

### 格式模板
```matlab
classdef 类名 < handle  % < handle 表示“句柄类”，对象像指针（修改影响所有引用）
    properties  % 类的变量定义处
        属性名1 = 默认值  % 可以设默认值
        属性名2
    end
    
    methods  % 函数定义处
        function obj = 类名(输入参数)  % 构造函数：创建对象时自动跑
            % 初始化代码
        end
        
        function 输出 = 方法名(obj, 输入参数)  % 实例方法：操作对象
            % 方法代码
        end
    end
end
```

- **classdef**：类的开头。推荐后面加`< handle` 。
- **properties**：存数据，像对象的“背包”。
- **methods**：定义行为，像对象的“技能”。

### 快速创建步骤
1. 新建 `.m` 文件，命名为类名（e.g., `MyClass.m`）。
2. 复制模板，填内容。
3. 保存，命令行运行 `obj = MyClass();` 测试。

## 2. Properties：用于定义类内的公共变量
- **作用**：声明类的变量，每个对象有自己的拷贝（存比特数、信号等）。
- **格式**：
  ```matlab
  properties
      NumBit = 100;  % 公共属性，默认 100（所有对象初始值）
      Signal          % 无默认，创建时为空
      (Access = private) PrivateVar  % 私有：外部不能直接改
  end
  ```
- **关键**：必须先声明，才能在方法中用 `obj.属性名`。类型自动推断（e.g., 数字→double）。

> **小贴士**：属性名常用大写开头（NumBit），参数用小写（num_bit），避免混淆。

## 3. Methods：用于定义类内的函数
- **作用**：让对象“做事”。分两种：
  - **构造函数**：创建对象时跑，初始化属性。
  - **实例方法**：操作特定对象。

### 构造函数（必学！）
- **签名**：`function obj = 类名(输入)`
- **为什么返回 obj？** obj 是“新对象”的引用，返回它给调用者。
- **示例**：
  ```matlab
  function obj = Transmitter(num_bit)
      obj.NumBit = num_bit;  % 从输入设属性
      obj.Signal = [];       % 设为空数组
  end
  ```
- **调用**：`tx = Transmitter(100);` → obj.NumBit = 100。

### 其他函数
- **签名**：`function 输出 = 方法名(obj, 输入)`
- **为什么有 obj？** obj 是“当前对象”，类比Python中的“self”
- **示例**（QPSK 调制）：
  ```matlab
  function signal = transmit(obj)
      obj.Signal = rand(1, obj.NumBit);  % 用属性生成数据
      signal = obj.Signal * 2;           % 返回处理后
  end
  ```
- **调用**：`sig = tx.transmit();` → 修改 tx.Signal 并返回。

> **obj vs 无 obj**：有 obj = 实例方法（需对象调用）；无 obj = 静态方法（直接 `ClassName.method()`，少用）。

## 4. Properties 默认值 vs 构造函数初始化：有区别！
两者都能“填数据”，但别混用。看表格对比：

| 方式                | 何时跑？                 | 灵活吗？              | 示例场景                       |
| ------------------- | ------------------------ | --------------------- | ------------------------------ |
| **Properties 默认** | 创建对象前自动           | 固定值                | 简单常量（如默认比特数=100）   |
| **构造函数 初始化** | 创建对象时（可覆盖默认） | 动态（基于输入/条件） | 用户输入验证（如 num_bit > 0） |

- **结合用**：properties 设简单默认，构造函数加逻辑。
- **示例**：
  ```matlab
  properties
      NumBit = 50;  % 默认 50
  end
  
  methods
      function obj = MyClass(num)
          if nargin > 0  % 有输入？
              obj.NumBit = num;  % 覆盖默认
          end
      end
  end
  ```
  - `obj = MyClass();` → 50
  - `obj = MyClass(200);` → 200

> **常见坑**：忘返回 obj → 对象为空。属性未声明 → “Undefined property” 错。

## 5. 完整示例：简单发射机类
文件 `Transmitter.m`：
```matlab
classdef Transmitter < handle
    properties
        NumBit = 100;  % 默认比特数
        TxBit
        TxSignal
    end
    
    methods
        function obj = Transmitter(num_bit)
            if nargin > 0
                obj.NumBit = num_bit;
            end
            obj.TxBit = [];
            obj.TxSignal = [];
        end
        
        function transmit(obj)
            obj.TxBit = randi([0 1], 1, obj.NumBit);  % 随机比特
            obj.TxSignal = obj.TxBit * 2 - 1;         % 简单调制
        end
    end
end
```

**测试**（命令行）：
```matlab
tx = Transmitter(10);     % 创建
tx.transmit();            % 生成信号
disp(tx.TxSignal);        % 查看：随机 ±1 序列
```

## 6.MATLAB 警告解释：值类方法（没写handle）必须返回修改后的对象

它是 MATLAB 在 OOP（面向对象编程）中对**值类（value class）**的“安全提醒”。简单说：如果你用方法修改了对象，但没返回修改后的对象，MATLAB 担心你会丢失变化（因为值类的工作方式）。

#### 1. **警告为什么出现？**
- **触发条件**：
  - 你的类是**值类**（classdef 中**没有 `< handle`**）。
  - 方法中修改了 `obj` 的属性（e.g., `obj.tx_symbols = ...`）。
  - 但方法**没有返回**修改后的 `obj`（签名如 `function transmit(obj)`，无 `= obj`）。
  
- **本质原因**：
  - **值类 vs 句柄类**：
    | 类型            | 继承方式      | 修改行为                                                     | 适合场景                             |
    | --------------- | ------------- | ------------------------------------------------------------ | ------------------------------------ |
    | **值类** (默认) | 无 `< handle` | 方法内 `obj` 是**拷贝**（本地副本）。修改只影响副本，原对象不变。除非返回新对象。 | 不可变数据（如数学计算），像结构体。 |
    | **句柄类**      | `< handle`    | 方法内 `obj` 是**引用**（指针）。修改直接影响原对象，无需返回。 | 可变状态（如你的发射机），像对象。   |
  - 在值类中，调用 `obj = transmit(obj);` 时，MATLAB 传的是拷贝。如果不返回，变化“丢了”——原 `tx` 没变！

- **你的代码示例**（假设值类）：
  ```matlab
  function transmit(obj)  % 无返回 → 警告！
      obj.tx_symbols = ...;  % 只改了拷贝
  end
  ```
  - 调用：`tx.transmit();` → `tx.tx_symbols` 还是旧值（空）。

#### 2. **修复方法**
##### **方案1: 改为句柄类（推荐，永久解决）**
- 在 `classdef` 加 `< handle`：
  ```matlab
  classdef Transmitter < handle  % 加这！
      properties
          bits_per_packet
          tx_symbols
          num_symbols
      end
      
      methods
          function obj = Transmitter(num_bits)  % 构造函数返回 obj
              obj.bits_per_packet = num_bits;
          end
          
          function transmit(obj)  % 无返回也 OK！
              tx_bits = ceil(rand(1, obj.bits_per_packet) - 0.5);
              obj.tx_symbols = obj.qpsk_modulate(tx_bits);
              obj.num_symbols = obj.bits_per_packet / 2;
          end
      end
  end
  ```
- **效果**：调用 `tx.transmit();` 后，`tx.tx_symbols` 直接更新。无警告！
- **注意**：句柄类对象像“遥控器”，所有引用共享变化（e.g., `tx2 = tx; tx.transmit();` → tx2 也变）。

##### **方案2: 值类中使用返回值（保持值类）**
```matlab
classdef Transmitter  % 无 < handle，值类
    % ... properties ...
    
    methods
        function [tx_symbols, num_symbols] = transmit(obj)  % 返回 obj！
            tx_bits = ceil(rand(1, obj.bits_per_packet) - 0.5);
            obj.tx_symbols = obj.qpsk_modulate(tx_bits);
            obj.num_symbols = obj.bits_per_packet / 2;
            tx_symbols = obj.tx_symbols;
            num_symbols = obj.num_symbols;
        end
    end
end
```

- **调用**：[tx_symbols, num_symbols]=tx.transmit();

## 7. 常见问题 & 调试

- **报错“Undefined property”**：检查拼写/大小写，确认在 properties 声明。
- **对象为空**：构造函数忘 `return obj`。
- **值类 vs 句柄类**：`< handle` 让修改共享；无 = 拷贝新对象。
- **调试**：用 `disp(obj)` 看属性；加 `fprintf` 打印。
