---
title: "Miniconda + Anaconda Navigator 配置Python开发环境"
date: 2025-11-16
permalink: /posts/2025/11/Anaconda-environment/
tags:
  - Anaconda
  - Miniconda
  - Anaconda Navigator
  - Python开发环境配置
categories:
  - Technical Documentation
excerpt: "在本地快速搭建 **Miniconda** 环境管理 + **VSCode** 代码编辑 + **Anaconda Navigator** 可视化管理 的 Python 开发工作流。"
collection: posts
---


## 1. 安装 Miniconda（轻量级 Conda）

### 1.1 下载
- 官网：[https://docs.conda.io/en/latest/miniconda.html](https://docs.conda.io/en/latest/miniconda.html)



### 1.2 安装步骤
1. **运行安装包**
   - Windows：双击 `.exe`，勾选 **“Add Miniconda to PATH”**（推荐）
   - macOS/Linux：终端执行
     ```bash
     bash Miniconda3-latest-*.sh
     ```
2. **初始化 Conda**
   ```bash
   conda init --all
   ```
   重启终端后输入 `conda -V` 验证版本。

---

## 2. 安装 Anaconda Navigator

> 在应用里找到miniconda文件夹，运行miniconda文件夹下的“Anaconda powershell prompt”。

```bash
conda install anaconda-navigator -c conda-forge
```

启动：
```bash
anaconda-navigator
```

---
## 3. 在 VSCode 中使用 Conda 环境

### 自动识别
1. 打开项目文件夹
2. `Ctrl+Shift+P` → 输入 **“Python: Select Interpreter”**
3. 选择：
   ```
   ~/miniconda3/envs/myenv/bin/python   (Linux/macOS)
   C:\Users\xxx\miniconda3\envs\myenv\python.exe (Windows)
   ```
### 切换默认终端
vscode默认终端是powershell，在激活Conda环境时可能有警告信息，简易切换默认终端为cmd
1. 按下 Ctrl+Shift+P 快捷键打开命令面板。
2. 输入：Terminal: Select Default Profile。
3. 在弹出的列表中选择Command。
## 4. 创建与使用 Conda 环境（如果不使用Navigator）
- 推荐创建环境用Navigator，安装包使用vscode命令行；环境将会在运行程序时自动激活
### 4.1 创建环境（推荐 Python 3.11）
```bash
conda create -n myenv python=3.11 -y
```

### 4.2 激活环境
```bash
conda activate myenv
```

### 4.3 安装常用包
```bash
conda install numpy pandas matplotlib jupyter scikit-learn -y
# 或用 pip
pip install torch torchvision seaborn
```

## 5. 常用命令速查

| 命令 | 说明 |
|------|------|
| `conda env list` | 查看所有环境 |
| `conda activate myenv` | 激活环境 |
| `conda deactivate` | 退出环境 |
| `conda env export > environment.yml` | 导出环境 |
| `conda env create -f environment.yml` | 从文件创建环境 |
| `conda clean --all` | 清理缓存 |

---

