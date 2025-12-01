---
title: "使用 Intel ARC 显卡加速 YOLO 推理检测的配置指南（Windows + OpenVINO + Anaconda)"
date: 2025-11-29
permalink: /posts/2025/11/GPU-YOLO-Accelerate/
tags:
  - GPU加速
  - 深度学习
  - YOLO
  - ARC核显
  - OpenVINO
categories:
  - Technical Documentation
excerpt: "本指南介绍如何在搭载 Intel ARC 集成显卡（如 Ultra 7 255H / Ultra 5 125H）的 Windows 电脑上，使用 OpenVINO 工具链和 Arc GPU 实现 YOLO 模型的加速推理，适用于 YOLOv5 / YOLOv8 模型。"
collection: posts
---


## ✅ 1. 安装前提和环境准备

### 1.1 必备软件

| **名称**                              | **说明**                                                                                                                                                                                                                                                       | **安装方式**                                                                                                            |
| ------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------- |
| **Intel oneAPI Base Toolkit部分驱动** | **选择安装：Intel® oneAPI Deep Neural Network Library (oneDNN)** <br />**Intel® oneAPI Threading Building Blocks (oneTBB)** <br />**Intel® oneAPI Math Kernel Library (oneMKL)** <br />**Intel® oneAPI Data Analytics Library (oneDAL)** | [Get the Intel® oneAPI Base Toolkit](https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit-download.html) |
| **Anaconda / Miniconda**              | **推荐用 Conda 管理虚拟环境，安装Python 3.9–3.12**                                                                                                                                                                                                            | [https://www.anaconda.com/](https://www.anaconda.com/)                                                                           |
| **Intel ARC GPU 驱动**                | **启用 GPU 加速（必装）**                                                                                                                                                                                                                                      | [英特尔® 驱动程序和支持助理](https://www.intel.cn/content/www/cn/zh/support/intel-driver-support-assistant.html)                |
| **OpenVINO Runtime**                  | **用于运行模型**                                                                                                                                                                                                                                               | **通过 conda 安装**                                                                                                     |
| **Ultralytics YOLOv8**                | **导出 YOLO 模型（.pt -> OpenVINO）**                                                                                                                                                                                                                          | `pip install ultralytics`                                                                                                   |

> **⚠ 注意：无需安装 Intel oneAPI Base Toolkit 的完整版本，仅需驱动和 Python 依赖。**

---

## ✅ 2. 使用Anaconda Navigator或命令行创建 Conda 环境并安装依赖

```
conda create -n yolo_arc python=3.10 -y
conda activate yolo_arc
conda install -c conda-forge openvino=2025.3.0
pip install ultralytics
```

---

## ✅ 3. 导出 best.pt 为 OpenVINO 模型

```
from ultralytics import YOLO

model = YOLO("best.pt") # 假设yolo训练好的模型为best.pt
model.export(format="openvino")  # 生成 best_openvino_model/ 目录
```

**还需保存类别名（可选），因为导出的openvino模型没有torch模型中类别名：**

```
from ultralytics import YOLO

model = YOLO("best.pt")
class_names = model.names
with open("class_names.txt", "w", encoding="utf-8") as f:
    for name in class_names.values():
        f.write(f"{name}\n")
```

---

## ✅ 4. 验证 GPU 是否启用

**添加以下调试代码查看设备：**

```
print("可用设备:", core.available_devices)
```

**输出应包含：**`['CPU', 'GPU']`，说明 Arc GPU 被正确识别。

---

## ✅ 5. 示例程序

```
import cv2
import numpy as np
import time
from openvino import Core
from pathlib import Path

# ========== 配置 ==========
model_path = Path("best_openvino_model/best.xml")
image_path = Path("test.jpg")
class_file = Path("class_names.txt")
conf_thresh = 0.25
iou_thresh = 0.45
# 尝试加载best.xml模型文件，否则转换best.pt
try:
    model_path.exists()
except:
    from ultralytics import YOLO

    model = YOLO("best.pt")  # 假设yolo训练好的模型为best.pt
    model.export(format="openvino")  # 生成 best_openvino_model/ 目录
# 尝试加载类别文件，否则从pt模型中提取类别
try:
    class_file.exists()
except:
    from ultralytics import YOLO

    model = YOLO("best.pt")
    class_names = model.names
    with open("class_names.txt", "w", encoding="utf-8") as f:
        for name in class_names.values():
            f.write(f"{name}\n")


# ========== 加载类别 ==========
with open(class_file, "r", encoding="utf-8") as f:
    class_names = [line.strip() for line in f.readlines()]

print(f"类别: {class_names}")

# ========== 加载 OpenVINO 模型 ==========
print("\n加载 OpenVINO 模型...")
core = Core()
model = core.read_model(model_path)

# 选择设备
available_devices = core.available_devices
print(f"可用设备: {available_devices}")

# 优先使用 GPU，否则使用 CPU
device = "GPU" if "GPU" in available_devices else "CPU"
print(f"使用设备: {device}")

compiled_model = core.compile_model(model, device_name=device)
input_layer = compiled_model.input(0)
output_layer = compiled_model.output(0)

input_shape = input_layer.shape  # [1, 3, H, W]
print(f"输入形状: {input_shape}")
print(f"输出形状: {output_layer.shape}\n")


# ========== 图像预处理 ==========
def preprocess_image(img_path, input_shape):
    """预处理图像"""
    img = cv2.imread(str(img_path))
    img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    img_resized = cv2.resize(img_rgb, (input_shape[3], input_shape[2]))
    img_normalized = img_resized.astype(np.float32) / 255.0
    img_transposed = np.transpose(img_normalized, (2, 0, 1))
    img_batch = np.expand_dims(img_transposed, axis=0)
    return img_batch, img


# ========== 后处理 ==========
def postprocess(output, conf_thresh, iou_thresh, orig_shape, num_classes):
    """OpenVINO 输出后处理"""
    # output shape: [1, 8, 8400] -> 转置为 [8400, 8]
    pred = output[0].T  # [8400, 8]

    # 提取坐标和类别分数
    boxes = pred[:, :4]  # [x, y, w, h]
    class_scores = pred[:, 4 : 4 + num_classes]  # 类别分数

    # 获取最大分数和类别
    max_scores = np.max(class_scores, axis=1)
    class_ids = np.argmax(class_scores, axis=1)

    # 置信度过滤
    mask = max_scores > conf_thresh
    boxes = boxes[mask]
    max_scores = max_scores[mask]
    class_ids = class_ids[mask]

    if len(boxes) == 0:
        return []

    # 中心坐标转左上右下
    x_center, y_center, w, h = boxes[:, 0], boxes[:, 1], boxes[:, 2], boxes[:, 3]
    x1 = x_center - w / 2
    y1 = y_center - h / 2
    x2 = x_center + w / 2
    y2 = y_center + h / 2

    # 缩放到原图尺寸
    scale_x = orig_shape[1] / 640
    scale_y = orig_shape[0] / 640
    x1 = np.clip(x1 * scale_x, 0, orig_shape[1]).astype(int)
    y1 = np.clip(y1 * scale_y, 0, orig_shape[0]).astype(int)
    x2 = np.clip(x2 * scale_x, 0, orig_shape[1]).astype(int)
    y2 = np.clip(y2 * scale_y, 0, orig_shape[0]).astype(int)

    # NMS
    boxes_xywh = np.stack([x1, y1, x2 - x1, y2 - y1], axis=1)
    indices = cv2.dnn.NMSBoxes(
        boxes_xywh.tolist(), max_scores.tolist(), conf_thresh, iou_thresh
    )

    results = []
    if len(indices) > 0:
        for idx in indices.flatten():
            results.append(
                {
                    "box": [x1[idx], y1[idx], x2[idx], y2[idx]],
                    "score": float(max_scores[idx]),
                    "class_id": int(class_ids[idx]),
                }
            )

    return results


# ========== 预热 ==========
print("预热推理...")
img_tensor, orig_image = preprocess_image(image_path, input_shape)

warmup_start = time.time()
_ = compiled_model({input_layer.any_name: img_tensor})
warmup_time = (time.time() - warmup_start) * 1000
print(f"预热时间: {warmup_time:.2f} ms\n")
# 计算检测一次的时间
time_all = time.time()
# ========== 推理 ==========
img_tensor, _ = preprocess_image(image_path, input_shape)
output = compiled_model({input_layer.any_name: img_tensor})[output_layer]
# ========== 后处理 ==========
orig_h, orig_w = orig_image.shape[:2]
detections = postprocess(
    output, conf_thresh, iou_thresh, (orig_h, orig_w), len(class_names)
)
print(f"检测到 {len(detections)} 个目标:\n")

# ========== 绘制结果 ==========
colors = [(0, 255, 0), (255, 0, 0), (0, 0, 255), (255, 255, 0)]

for det in detections:
    x1, y1, x2, y2 = det["box"]
    conf = det["score"]
    cls_id = det["class_id"]
    class_name = class_names[cls_id]

    print(f"  类别: {class_name}, 置信度: {conf:.2f}, 坐标: ({x1},{y1})-({x2},{y2})")

    color = colors[cls_id % len(colors)]
    cv2.rectangle(orig_image, (x1, y1), (x2, y2), color, 2)

    label = f"{class_name}{conf:.2f}"
    (label_w, label_h), _ = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.6, 2)
    cv2.rectangle(orig_image, (x1, y1 - label_h - 10), (x1 + label_w, y1), color, -1)
    cv2.putText(
        orig_image,
        label,
        (x1, y1 - 5),
        cv2.FONT_HERSHEY_SIMPLEX,
        0.6,
        (255, 255, 255),
        2,
    )
time_post = (time.time() - time_all) * 1000
print(f"\n总处理时间: {time_post:.2f} ms")
cv2.imwrite("result_openvino.jpg", orig_image)
print(f"\n结果已保存到 result_openvino.jpg")
print(f"理论 FPS: {1000/time_post:.1f}")

cv2.imshow("OpenVINO Detection", orig_image)
print("\n按任意键关闭...")
cv2.waitKey(0)
cv2.destroyAllWindows()

```
