# Smart Lock & Environmental Monitor System — FreeRTOS 版

基于STM32F407VET6的智能门锁与环控系统，**FreeRTOS 多任务版**。将裸机版的状态机拆分为5个独立 RTOS 任务，通过消息队列通信、互斥量保护共享资源，实现更好的实时性与模块隔离。

> - **RTOS 版**：基于 FreeRTOS的多任务架构
> - **开发记录**：详细开发步骤、踩坑复盘、调试经验，请查阅 [Doc/STAGE.md](Doc/STAGE.md)（开发步骤）和 [Doc/HOLE.md](Doc/HOLE.md)（踩坑记录）。
> - **操作指南**：[Doc/STEP.md](Doc/STEP.md)：从零上手指南（硬件连接、软件配置、功能验证）

🍃 *"想要穷尽世间繁多的真理和秘密，必须奉献自己的全部。"* — 纳西妲

## 😶‍🌫️项目概述

本项目是基于STM32平台的智能门锁系统，同时集成了温湿度、烟雾、光照等环境参数的监测，具有完整的用户交互界面和多通信接口。

### 核心功能

- **智能门锁**：矩阵键盘输入，认证记录存储和错误次数报警
- **环境检测**：DHT11 监测温湿度、MQ2 监测烟雾、光敏电阻(或可选 BH1750 )监测光照
- **数据存储**：外接 W25Q16 存储，保存正确密码、用户密码、操作记录、系统参数
- **人机交互**：丰富人机交互功能，OLED 实时显示、蜂鸣器、红外传感器、LED 指示、多接口通信(串口、蓝牙)
- **自动控制**：温控风扇、红外感应 LED 灯

## 🥇项目亮点

### 1. 裸机 → FreeRTOS 成功移植

> 从裸机系统成功迁移到 **FreeRTOS 多任务架构**，是此版本的核心亮点。

| 项目 | 裸机版 | RTOS 版 |
|------|--------|---------|
| 架构 | 状态机 `TaskDriver_Run()` 循环调度 | 5 个独立 RTOS 任务，内核调度 |
| 通信 | 全局变量+函数调用 | `osMessageQueue` 消息队列驱动 |
| 互斥 | 裸机顺序执行 | `osMutex` 保护 OLED 共享资源 |
| 实时性 | 状态机轮询 | 任务级独立，时间片流转 |

### 2. FreeRTOS 资源

```c
// 消息队列
KeyQueueHandle        = osMessageQueueNew(10, sizeof(uint8_t), NULL);
HwCtrlQueueHandle     = osMessageQueueNew(10, sizeof(uint8_t), NULL);

// 互斥量
OLED_MutexHandle = osMutexNew(NULL);

// 任务通知
osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);

// 总堆大小
configTOTAL_HEAP_SIZE  = 15360
```

## 🏠工程结构

```
Smart Lock System RTOS/
├── Core/                           # STM32 HAL库 + FreeRTOS 核心文件
│   ├── Inc/                        # 头文件目录
│   │   ├── FreeRTOSConfig.h        # FreeRTOS 配置
│   │   ├── TaskDriver.h            # 任务声明/队列句柄
│   │   ├── main.h
│   │   └── ...
│   └── Src/                        # 源文件目录
│       ├── main.c                  # 主程序入口(初始化+启动内核)
│       ├── freertos.c              # FreeRTOS 对象创建(队列/互斥量/任务)
│       ├── TaskDriver.c            # 5个RTOS任务实现
│       └── ...
├── Drivers/                        # 驱动层
│   └── Hardware/                   # 硬件驱动层
│       ├── SensorRead.c/.h         # 传感器数据采集
│       └── ...
├── Middlewares/                     # FreeRTOS源码
├── Doc/                            # 项目文档
│   ├── Datasheet                   # 芯片和模块数据手册
│   ├── HOLE.md                     # 调试踩坑记录
│   └── STEP.md                     # 开发步骤记录
├── MDK-ARM/                        # Keil MDK工程文件
├── SmartLock_RTOS.ioc              # STM32CubeMX配置文件
└── README.md                       # 本文档
```

### 与裸机版的工程差异

| 项目 | 裸机版 | RTOS版 |
|------|--------|--------|
| 任务驱动 | `TaskDriver.c`(状态机) | `TaskDriver.c`(任务) + `freertos.c`(RTOS调度) |
| FreeRTOS 配置 | 无 | `FreeRTOSConfig.h` |
| 时基 | SysTick | TIM14 (SysTick 被 FreeRTOS 占领) |
| 队列/互斥量 | 无 | 4个消息队列 + 1个互斥量 |

## 🍃相关文档

| 文档 | 内容 | 推荐阅读 |
|------|------|----------|
| **[HOLE.md](Doc/HOLE.md)** | 调试过程中遇到的典型问题与解决方案 | 硬件调试、软件bug、时序问题 |
| **[STAGE.md](Doc/STAGE.md)** | 项目开发阶段的步骤复盘 | 架构设计、模块编写、集成测试 |
| **[STEP.md](Doc/STEP.md)** | 工程快速上手指南 | 硬件连接、操作方式、功能验证 |
| **Datasheet/** | 所有芯片、模块的官方数据手册 | 引脚定义、通信协议、电气参数 |

**建议**：
1. 了解项目 → 阅读本README
2. 重现或调试 → 查阅HOLE.md避免踩坑
3. 开发流程 → 阅读STAGE.md了解各阶段思路
4. 快速上手 → 阅读STEP.md了解操作方式
5. 硬件连接 → 查看Datasheet/对应模块文档

## 💨快速开始

### 1. 环境准备
- **IDE**：Keil MDK-ARM 5.36+
- **固件库**：STM32CubeF4 HAL库
- **硬件**：STM32F407VET6核心板 + 外设模块（见Doc/Datasheet）

### 2. 编译与下载
1. 打开 `MDK-ARM/SmartLock_RTOS.uvprojx`
2. 根据硬件修改引脚配置
3. 编译（F7）→ 下载（F8）→ 复位运行

> **注意**：本工程freeRTOS中删除了自动生成 的默认任务，cubemx再次生成会重新创建该任务，多一个空任务调度

## 💮将来扩展功能

### 硬件扩展
1. **WiFi模块** - 联网实现远程控制
2. **RFID模块** - 实现刷卡开锁
3. **语音模块** - 实现语音控制
4. **IPS屏幕** - 更丰富的人机交互界面

### 软件优化
1. **LVGL界面优化** - 更美观的人机交互显示
2. **事件驱动** - 用 FreeRTOS 信号量替代轮询，进一步降低CPU占用
3. **Bootloader** - 通过串口/蓝牙远程更新固件

## 许可证

本项目采用MIT许可证。详见LICENSE文件。

## 🧸联系方式

- **作者**: Nahida
- **邮箱**: 529060418@qq.com
- **项目地址**: https://github.com/Nahida-Robin/Smart-Lock-System
