# 🏠工程结构

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
│   ├── STAGE.md                   # 开发步骤记录
│   ├── STRUCUTRE.md               # 本文档 项目结构
│   ├── HOLE.md                    # 调试踩坑记录
│   └── STEP.md                    # 快速上手指南
├── MDK-ARM/                       # Keil MDK工程文件
│   ├── DebugConfig/ 
│   ├── RTE/
│   └── test.uvprojx               # 工程文件
├── test.ioc                       # STM32CubeMX配置文件
└── README.md                      # 整体介绍
```


# 🎨硬件架构

## 主控芯片
- **MCU**：STM32F407VET6(ARM Cortex-M4)
- **时钟**： HSE 8MHz, LSE 32.768kHz (RTC时钟源)

## 外设模块
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


# 🍥软件架构

## 分层设计
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

## 核心状态机

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
