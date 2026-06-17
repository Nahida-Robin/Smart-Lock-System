# 从零上手：Smart Lock & Environmental Monitor System

> **关联文档**：
> - **[README.md](../README.md)** —— 项目整体介绍、架构详解
> - **[HOLE.md](HOLE.md)** —— 调试踩坑记录（共16个问题）
> - 本文档是 **快速上手指南**，侧重操作步骤

## 🍓前期准备

### 1. 硬件清单（必需）
| 模块 | 型号 | 说明 |
|------|------|------|
| **主控板** | STM32F407VET6 | 核心板（LQFP100封装） |
| **OLED** | 0.96寸 SSD1306 | I2C接口，4针（VCC/GND/SCL/SDA） |
| **矩阵键盘** | 4×4矩阵键盘 | 行线接PD0‑3，列线接PD4‑7 |
| **温湿度** | DHT11 | 单总线，接PB6 |
| **烟雾** | MQ‑2 | 模拟输出接PA1（ADC1） |
| **光照** | BH1750 | I2C接口，与OLED共用SCL/SDA |
| **红外接收** | HS0038B | 38kHz接收头，接PC6（TIM8_CH1） |
| **红外发射** | 红外LED + 三极管 | 接PA0（TIM5_CH1） |
| **Flash存储** | W25Q16 | SPI接口，接SPI1 |
| **步进电机** | 28BYJ‑48 | ULN2003驱动，接PB8‑11 |
| **舵机** | SG90 | PWM控制，接PA8（TIM1_CH1） |
| **蓝牙** | JDY‑31B | 串口透传，接USART2（PA2/PA3） |
| **蜂鸣器** | 有源 | 接PC13 |
| **LED** | 普通LED | 接PA5（PWM调光） |

### 2. 软件环境
- **IDE**：Keil MDK‑ARM 5.36
- **串口工具**：PuTTY、SecureCRT或任意串口助手（波特率9600）
- **手机APP**：蓝牙串口 (如微信小程序江协蓝牙串口或软件HC调试助手)

### 3. 下载工程
```bash
git clone https://github.com/Nahida-Robin/Smart-Lock-System
```

## 💮软件配置与编译

### 1. 打开工程
1. 进入 `MDK‑ARM/` 文件夹
2. 双击 `test.uvprojx`（Keil工程文件）

### 2. 检查配置
- **注意勾选Use MicroLib 和 C99Mode**
- **Device**：STM32F407VET6
- **Target Options → C/C++**：
  - Define: `USE_HAL_DRIVER, STM32F407xx`
  - Include Paths: 确认包含 `Core/Inc`、`Drivers/STM32F4xx_HAL_Driver/Inc`、`Drivers/Hardware`
- **Target Options → Debug**：选择你的调试器（ST‑Link/DAP‑Link）

### 3. 编译与下载
1. **编译**：点击 `Build`（F7）按钮，确认 **0 Error(s), 0 Warning(s)**
2. **下载**：点击 `Download`（F8）或 `Flash → Download`
3. **复位**：按开发板复位键，程序开始运行

## 🍥运行验证

### 1. OLED显示确认
上电后，OLED应显示：
```
   2026‑06‑08
   20:30:45
```
表示RTC工作正常，系统进入 **IDLE状态**。

### 2. 键盘操作
按下矩阵键盘：
- **`11`** → 进入密码验证界面（显示 `Input Password:`）
- **`12`** → 进入主菜单（显示温度、湿度、烟雾、光照、开锁次数、报警次数）
- **`16`**（在菜单界面）→ 返回IDLE
- **后续操作OLED均有提示，不再赘述**
- **任意界面下按`16`返回上一级界面**

### 3. 串口通信
1. 连接USB转TTL到 **USART1（PA9/PA10）**，波特率 **9600**
2. 打开串口工具，发送命令（不带换行）：
   - `STATUS` → 返回系统状态
   - `HELP` → 显示命令列表

### 4. 蓝牙通信
1. 手机打开蓝牙串口APP
2. 搜索并连接 **JDY‑31B**
3. 发送相同命令（`STATUS`、`HELP`等），观察响应

### 5. 红外遥控
1. 使用 **NEC协议遥控器**
2. 对准红外接收头按下任意键
3. 观察串口输出（应解析出地址码和数据码）

### 6. 传感器测试
- **温度/湿度**：手捏DHT11，观察OLED菜单中数值变化
- **烟雾**：打火机（不点火）靠近MQ‑2，数值上升
- **光照**：用手遮住光敏电阻(或BH1750)，光照值下降

## ☘️常见问题排查

**可查阅[HOLE.md](HOLE.md)**

**文档参考**：
- 硬件连接问题 → 查看 `Datasheet/` 对应模块手册
- 软件bug → 查看 `HOLE.md` 中类似问题的解决方案
- 架构理解 → 阅读 `README.md` 系统设计部分

## 💥总结

本系统是一个 **完整的嵌入式综合项目**，涵盖了：
- **硬件设计**：多模块协同工作
- **软件架构**：状态机+模块化驱动
- **通信协议**：串口、蓝牙、红外、I2C、SPI、单总线
- **存储管理**：Flash掉电保存
- **人机交互**：OLED显示+矩阵键盘
