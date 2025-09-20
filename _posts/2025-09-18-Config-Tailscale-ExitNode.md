---
title: "配置 Tailscale 路由器作为出口节点（Exit Node）"
date: 2025-09-18
permalink: /posts/2025/09/Config-Tailscale-ExitNode/
tags:
  - OpenWRT
  - glinet
  - be3600
  - Teilscale
  - 软路由
categories:
  - Technical Documentation
excerpt: "路由器放学校，实现随时随地访问校园网"
collection: posts
---

## 目标
通过 Tailscale 将路由器配置为出口节点（exit node），以便远程通过路由器上网（如路由器放在学校，可以实现在家连接校园网）。前提条件：电脑、手机、路由器都安装有Tailscale。

## 路由器配置：打开Tailscale网页管理界面

勾选 use as exit node

<img src="C:\Users\qly24\AppData\Roaming\Typora\typora-user-images\image-20250918121752108.png" alt="image-20250918121752108" style="zoom:33%;" />

如果是灰色，则使用ssh远程连接路由器

运行以下命令时：

```bash
sudo tailscale up --advertise-exit-node
```
如果遇到报错如下：
```
Error: changing settings via 'tailscale up' requires mentioning all
non-default flags. To proceed, either re-run your command with --reset or
use the command below to explicitly mention the current value of
all non-default settings:

        tailscale up --advertise-exit-node --accept-dns=false --accept-routes --advertise-routes=10.193.0.0/16,192.168.8.0/24
```

这是因为路由器上 Tailscale 已经配置了一些非默认选项（比如 --accept-dns=false、 --accept-routes 和 --advertise-routes=10.193.0.0/16,192.168.8.0/24）。这是 Tailscale 的安全机制，防止意外覆盖现有配置。

运行以下命令以保留现有配置并启用出口节点：

```bash
sudo tailscale up --advertise-exit-node --accept-dns=false --accept-routes --advertise-routes=10.193.0.0/16,192.168.8.0/24
```

验证 Exit Node 状态：
- 检查 Tailscale 状态：
  ```bash
  tailscale status
  ```
  确认路由器在线并显示 Tailscale IP（100.x.x.x）。
- 登录 Tailscale 管理控制台（https://login.tailscale.com/admin/machines），确认路由器标记为“Exit Node”。

### 电脑端配置
1. **设置路由器为出口节点**：
   - 在电脑上运行：
     ```bash
     tailscale up --exit-node=<路由器的 Tailscale IP>
     ```
     （替换 `<路由器的 Tailscale IP>` 为 100.x.x.x，可通过 `tailscale status` 获取）。
   - 或在 Tailscale 客户端界面选择路由器作为出口节点。

2. **测试连接**：
   - 访问 https://whatismyipaddress.com，确认电脑的公网 IP 与路由器的公网 IP 一致。
