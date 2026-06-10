# Smart Lock & Environmental Monitor System

基于STM32F407VET6的智能门锁与环控系统，集成了密码验证、环境监测、RTC实时时钟、步进电机&舵机控制、串口通信、蓝牙控制、红外遥控、OLED显示、Flash存储等功能。
> - **迭代记录**：由之前手搓的基于STM32F103C8T6标准库的智能家居和智能仓储合并而来
> - **开发记录**：详细开发步骤、踩坑复盘、调试经验，请查阅 [Doc/STAGE.md](Doc/STAGE.md)（开发步骤）和 [Doc/HOLE.md](Doc/HOLE.md)（踩坑记录）。
> - **操作指南**：[Doc/STEP.md](Doc/STEP.md)：从零上手指南（硬件连接、软件配置、功能验证）

🍃 *“成长是一场旅行，我们在旅途中遇见自己，也遇见更好的自己。”* — 纳西妲

## 😶‍🌫️项目概述

本项目是基于STM32平台的智能门锁系统，同时集成了温湿度、烟雾、光照等环境参数的监测，具有完整的用户交互界面和多通信接口。

### 核心功能

- **智能门锁**：矩阵键盘输入，认证记录存储和错误次数报警
- **环境检测**：DHT11 监测温湿度、MQ2 监测烟雾、光敏电阻(或可选 BH1750 )监测光照
- **数据存储**：外接 W25Q16 存储，保存正确密码、用户密码、操作记录、系统参数
- **人机交互**：丰富人机交互功能，OLED 实时显示、蜂鸣器、红外传感器、LED 指示、多接口通信(串口、蓝牙)
- **自动控制**：温控风扇、红外感应 LED 灯

## 🥇项目亮点

以下是一些本项目值得关注的亮点(或许)：

### 1. 工程文件夹目录清晰
- **目录规范**：目录整洁有序，结构层次分明，各类文件分类存放、一目了然
- **易于维护**：底层驱动统一置于 Drivers 目录，应用层代码归入 Core 目录，技术文档存放于 Doc 目录，职责清晰，便于管理与扩展

### 2. 优雅的状态机设计
- **架构清晰**：采用 typedef enum + switch-case 的9状态分层状态机，提高系统效率
- **可维护性**：每个状态有独立的 Task 函数，主循环仅做调度，逻辑分离明确
- **可扩展性**：新增状态只需在枚举中添加、编写状态任务然后在 switch 中添加分支即可

### 3. 非阻塞式系统设计
- **定时器驱动**：传感器采集通过TIM2定时中断触发，主循环只检测 DHT_Ready 变量，避免阻塞
- **DMA传输**：使用 ADC + DMA 搬运，主循环只检测 ADC_Ready 变量
- **状态机轮询**：SensorRead_Run 同样采用状态机方式处理传感器数据

### 4. 可扩展的串口命令系统
```c
有参数表：
typedef struct{
	const char* Cmd;
	int MinValue;
	int MaxValue;
	void (*Settle)(int value);
	const char* Name;
}Serial_CmdParam_t;
```
```c
无参数表：
typedef struct{
	const char* Cmd;
	void (*Show)(void);
	const char* Name;
}Serial_Cmd_t;
```
- **回调函数表**：命令、参数范围、处理函数统一管理，避免冗长的 if-else
- **易于扩展**：新增命令只需在表中添加条目，无需修改命令处理逻辑
- **非阻塞式**：串口、蓝牙接收采用 DMA 搬运+空闲中断，通过 XX_Cmd_Ready 通知 CPU 处理接收完的命令，减少系统负担

### 5. 详细的密码验证反馈
- **多重错误码**：返回1/2/-1/-2/-3(文件顶部宏定义，便于维护)等不同错误码，区分长度错误、密码错误、存储错误
- **分级报警**：3次普通错误→普通报警，3次以上→特殊报警
- **用户友好**：不同错误在 OLED 上显示不同的提示信息

### 6. 红外发射与接收驱动
```c
static void IR_SendBit(uint8_t bit)
{
	if(bit)
	{
		IR_SendHigh(560);
		IR_SendLow(1680);
	}
	else
	{
		IR_SendHigh(560);
		IR_SendLow(560);
	}
}
```
- **发射**：38KHz载波，封装红外发射步骤，内部调用函数即可
- **接收**：双边沿捕获解析NEC协议
- **应用**：红外遥控，参数调节(预留)

### 7. 模块化与高内聚设计
- **独立模块**：各模块驱动逻辑独立，内部函数变量用static限制作用域，仅留统一接口由 TaskDriver 等应用层调度
- **分层清晰**：硬件抽象层隔离具体硬件细节
- **代码复用**：相似功能采用相同设计模式，提高代码可维护性
- **低耦合度**：底层驱动间互不干扰，对外接口二次封装，设置应用层调用

## 🎨硬件架构

### 主控芯片
- **MCU**：STM32F407VET6(ARM Cortex-M4)
- **时钟**： HSE 8MHz, LSE 32.768kHz (RTC时钟源)

### 外设模块
| 模块 | 型号/类型 | 接口 | 功能说明 |
|------|-----------|------|----------|
| 显示模块 | OLED SSD1306 | I2C (PB8/9) | 128×64图形显示 |
| 输入模块 | 4×4自制矩阵键盘 | GPIO (PD0-7) | 用户密码输入 |
| 电源模块 | 自制12V转5V&3.3V电压模块 | 3V3 GND | 提供稳定电压 |
| 温湿度 | DHT11 | 单总线 (PB6) | 温度湿度采集 |
| 烟雾检测 | MQ-2 | ADC (PA1) | 烟雾浓度检测 |
| 光照检测 | 光敏电阻 | ADC (PC0) | 环境光照度 |
| 存储芯片 | W25Q16 | SPI (PB0 PB3-5) | 2MB Flash存储 |
| 蓝牙模块 | DXBT24 | USART2 (PA2/3) | 无线控制接口 |
| 调试串口 | USART1 | USART1 (PA9/10) | 串口命令控制 |
| 红外传感器 | 反射式红外 | GPIO (PA4) | 人体检测 |
| 步进电机 | 28BYJ-48 | GPIO (PB12-15) | 门锁机械控制 |
| 舵机 | SG90 | TIM4 PWM | 可选锁机构 |
| 风扇 | PWM风扇 | TIM3 CH2 (PA7) | 温控风扇 |
| LED灯 | PWM LED | TIM3 CH1 (PA6) | 可调亮度 |
| 蜂鸣器 | 无源蜂鸣器 | GPIO (PC13) | 报警提示 |
| 指示灯 | LED | GPIO (PB9) | 状态指示 |
| 红外发送 | V1221 | TIM5 CH1 (PA0) | 预留 |
| 红外接收 | V1222 | TIM8 CH1 (PC6) | 接收命令 |

## 🏠工程结构

```
Merge/
├── Core/                          # STM32 HAL库核心文件
│   ├── Inc/                       # 头文件目录
│   │   ├── main.h
│   │   ├── TaskDriver.h           # 主任务驱动头文件
│   │   ├── stm32f4xx_hal_conf.h   # HAL配置
│   │   └── ...
│   └── Src/                       # 源文件目录
│       ├── main.c                 # 主程序入口
│       ├── TaskDriver.c           # 主状态机驱动
│       ├── system_stm32f4xx.c
│       └── ...
├── Drivers/                       # 驱动层
│   ├── CMSIS/                     # ARM CMSIS核心
│   ├── STM32F4xx_HAL_Driver/      # STM32 HAL驱动库
│   └── Hardware/                  # 硬件驱动层
│       ├── BlueTooth.c/.h         # 蓝牙通信
│       ├── DataManage.c/.h        # Flash数据管理
│       ├── Display.c/.h           # OLED显示驱动
│       ├── Flash.c/.h             # W25Q16 Flash驱动
│       ├── Matrix.c/.h            # 矩阵键盘
│       ├── Motor.c/.h             # 风扇控制
│       ├── OLED.c/.h              # OLED底层驱动
│       ├── Password.c/.h          # 密码验证逻辑
│       ├── PWM.c/.h               # PWM输出
│       ├── RTCTime.c/.h           # RTC时间管理
│       ├── SensorRead.c/.h        # 传感器数据采集
│       ├── Serial.c/.h            # 串口通信
│       ├── Servo.c/.h             # 舵机控制
│       ├── StepMotor.c/.h         # 步进电机
│       └── ...
├── Doc/                           # 项目文档
│   ├── Datasheet                  # 芯片和模块数据手册
│   │   ├── stm32f407vet6          # 主控芯片的数据手册
│   │   ├── DHT11                  # DHT11的原理图和手册
│   │   ├── SG90                   # 舵机原理文档
│   │   ├── W25Q16                 # Flash数据手册
│   │   └── Power                  # 电源模块原理图和PCB
│   ├── HOLE.md                    # 调试踩坑记录
│   └── STEP.md                    # 开发步骤记录
├── MDK-ARM/                       # Keil MDK工程文件
│   ├── DebugConfig/ 
│   ├── RTE/
│   └── test.uvprojx               # 工程文件
├── test.ioc                       # STM32CubeMX配置文件
└── README.md                      # 本文档
```

## 🍥软件架构

### 分层设计
```
应用层:   TaskDriver.c (状态机)  ← 用户交互逻辑
          |    |
驱动层:   Display.c     Password.c     SensorRead.c
          |    |              |              |
          └────┴──────────────┴──────────────┘
硬件抽象: OLED.c    Matrix.c   DHT11.c    ADC采集
          |    |       |          |          |
硬件层:   SSD1306    矩阵键盘    DHT11    传感器
```

### 核心状态机

```c
typedef enum{
	STATE_IDLE = 0,
	STATE_MENU,
	STATE_VERTIFY,
	STATE_OPEN,
	STATE_CHAPWD,
	STATE_VIEW,
	STATE_SEL,
	STATE_CONT,
	STATE_RTC
}State_t;
```

| 状态 | 功能描述 | 显示内容 |
|------|----------|----------|
| `STATE_IDLE` | 待机界面 | 显示时间日期，等待输入 |
| `STATE_MENU` | 主菜单 | 显示传感器数据，开锁统计 |
| `STATE_VERTIFY` | 密码验证 | 输入密码，验证逻辑 |
| `STATE_OPEN` | 开锁状态 | 操作选项菜单 |
| `STATE_CHAPWD` | 修改密码 | 输入新密码 |
| `STATE_VIEW` | 查看记录 | 历史开锁记录分页显示 |
| `STATE_SEL` | 密码选择 | 选择不同的预设密码 |
| `STATE_CONT` | 参数控制 | 调整环境阈值参数 |
| `STATE_RTC` | 时间设置 | 设置RTC实时时钟 |

## 🍃相关文档

| 文档 | 内容 | 推荐阅读 |
|------|------|----------|
| **[HOLE.md](Doc/HOLE.md)** | 调试过程中遇到的 **16个典型问题** 与解决方案 | 硬件调试、软件bug、时序问题 |
| **[STAGE.md](Doc/STAGE.md)** | 项目 **18个开发阶段** 的步骤复盘 | 架构设计、模块编写、集成测试 |
| **[STEP.md](Doc/STEP.md)** | 工程 **快速上手** 指南 | 硬件连接、操作方式、功能验证 |
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
1. 打开 `MDK-ARM/test.uvprojx`
2. 根据硬件修改引脚配置
3. 编译（F7）→ 下载（F8）→ 复位运行

## 💮将来扩展功能

### 硬件扩展
1. **WiFi模块** - 联网实现远程控制
2. **RFID模块** - 实现刷卡开锁
3. **语音模块** - 实现语音控制
4. **IPS屏幕** - 更丰富的人机交互界面

### 软件优化
1. **逻辑优化** - 全部状态机化，用系统Tick驱动
1. **FreeRTOS移植** - 多任务管理
2. **LVGL界面优化** - 更美观的人机交互显示

## 许可证

本项目采用MIT许可证。详见LICENSE文件。

## 🧸联系方式

- **作者**: Nahida
- **邮箱**: 529060418@qq.com
- **项目地址**: https://github.com/Nahida-Robin/Smart-Lock-System