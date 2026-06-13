/** 
 * @file BlueTooth.c
 * @brief 蓝牙收发
 * @author Nahida
 * @date 2026.5.26
 */
 
#include "stm32f4xx.h"                  // Device header
#include "Password.h"
#include "TaskDriver.h"
#include "PWM.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define UW_MAX_SIZE 512
#define PW_MAX_SIZE 512
#define TM_MAX_SIZE 128
#define BT_BUF_SIZE 64

extern UART_HandleTypeDef huart2;
extern uint8_t Vertify_State;
extern uint8_t Temph;
extern uint8_t Templ;
extern uint8_t Humih;
extern uint8_t Humil;
extern uint8_t Warnings;
extern uint8_t Succeses;
extern uint8_t Light;
extern float Smog;
volatile uint8_t BlueTooth_Cmd_Ready = 0;
uint8_t BlueTooth_Rx_Buf[BT_BUF_SIZE];
uint8_t BlueTooth_Rx_Index = 0;

extern uint8_t PasswordMemory[PW_MAX_SIZE];
extern uint8_t UserwordMemory[UW_MAX_SIZE];
extern uint8_t TimeMemory[TM_MAX_SIZE];

extern volatile uint16_t PWMemory_Index;
extern volatile uint16_t UWMemory_Index;
extern volatile uint16_t TMMemory_Index;

/**
  *@brief 蓝牙处理结构体
  *@param NULL
  *@retval NULL
  */
typedef struct{
	const char* Cmd;
	int MinValue;
	int MaxValue;
	void (*Settle)(int value);
	const char* Name;
}BlueTooth_CmdParam_t;

/**
  *@brief 蓝牙显示结构体
  *@param NULL
  *@retval NULL
  */
typedef struct{
	const char* Cmd;
	void (*Show)(void);
	const char* Name;
}BlueTooth_Cmd_t;

/**
  *@brief 蓝牙通过串口发送字符串
  *@param string 要发送的字符串
  *@retval NULL
  */
static void BlueTooth_Transmit(char *string)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)string, strlen(string), 100);
}

/**
  *@brief 封装发送字符串实现printf功能
  *@param NULL
  *@retval NULL
  */
static void BlueTooth_Printf(const char *format, ...)
{
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    BlueTooth_Transmit(buffer);
}

/**
  *@brief 蓝牙初始化 即开启DMA搬运空闲中断
  *@param NULL
  *@retval NULL
  */
void BlueTooth_Init(void)
{
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, BlueTooth_Rx_Buf, BT_BUF_SIZE);
}

/**
  *@brief 温度设置回调函数
  *@param value 传入的参数
  *@retval NULL
  */
static void SetTempThre(int value)
{
	extern uint8_t TempThrehold;
	TempThrehold = (uint8_t)value;
}

/**
  *@brief 湿度设置回调函数
  *@param value 传入的参数
  *@retval NULL
  */
static void SetHumiThre(int value)
{
	extern uint8_t HumiThrehold;
	HumiThrehold = (uint8_t)value;
}

/**
  *@brief 烟雾设置回调函数
  *@param value 传入的参数
  *@retval NULL
  */
static void SetSmogThre(int value)
{
	extern uint16_t SmogThrehold;
	SmogThrehold = (uint16_t)value;
}

/**
  *@brief LED亮度设置回调函数
  *@param value 传入的参数
  *@retval NULL
  */
static void SetBrightness(int value)
{
	extern uint8_t Brightness;
	Brightness = (uint8_t)value;
	PWMLED_SetBright(Brightness);
}

/**
  *@brief 风扇开启温度设置回调函数
  *@param value 传入的参数
  *@retval NULL
  */
static void SetMotorOn(int value)
{
	extern uint8_t Motor_ON_Tem;
	Motor_ON_Tem = (uint8_t)value;
}

/**
  *@brief 风扇全速温度设置回调函数
  *@param value 传入的参数
  *@retval NULL
  */
static void SetMotorMax(int value)
{
	extern uint8_t Motor_MAX_Tem;
	Motor_MAX_Tem = (uint8_t)value;
}

/**
  *@brief 蓝牙发送当前系统状态
  *@param NULL
  *@retval NULL
  */
static void BlueTooth_ShowStatus(void)
{
	BlueTooth_Printf("[Status]Lock:%s Temp:%d.%d Humi:%d.%d Light:%d Smog:%dppm Unlockcnt:%d Warnings:%d", 
					Vertify_State ? "Open" : "Close", 
					Temph, Templ, Humih, Humil, Light, (uint16_t)Smog, Succeses, Warnings);
}

/**
  *@brief 蓝牙发送历史密码记录
  *@param NULL
  *@retval NULL
  */
static void BlueTooth_ShowRecord(void)
{
		uint16_t Uw_Index = 0;
		uint16_t Tm_Index = 0;
		uint8_t Rec_Num = 0;
		if(UWMemory_Index == 0 || TMMemory_Index == 0)
		{
			BlueTooth_Printf("[Error]No Records");
			return;
		}
		BlueTooth_Printf("[Records]\r\n");
		while(Uw_Index < UWMemory_Index && Tm_Index < TMMemory_Index)
		{
			uint8_t Hour = TimeMemory[Tm_Index++];
			uint8_t Min = TimeMemory[Tm_Index++];
			uint8_t Sec = TimeMemory[Tm_Index++];
			if(Tm_Index < TMMemory_Index && TimeMemory[Tm_Index] == 'Z'){Tm_Index++;}
			BlueTooth_Printf("Rec%d Time:%02d:%02d:%02d Pwd:", ++Rec_Num, Hour, Min, Sec);
			
			while(Uw_Index < UWMemory_Index && UserwordMemory[Uw_Index] != 'A')
			{
				BlueTooth_Printf("%d", UserwordMemory[Uw_Index++]);
			}
			if(Uw_Index < UWMemory_Index && UserwordMemory[Uw_Index] == 'A'){Uw_Index++;}
			BlueTooth_Printf("\r\n");
		}
	BlueTooth_Printf("Records END\r\n");
}

/**
  *@brief 蓝牙控制关索
  *@param NULL
  *@retval NULL
  */
static void BlueTooth_LockClose(void)
{
	if(Vertify_State == 1)
	{
		extern void Lock_Close(void);
		Lock_Close();
		BlueTooth_Printf("[Lock]Lock Close");
	}
	else
	{
		BlueTooth_Printf("[Lock]Already Close");
	}
}

/**
  *@brief 蓝牙控制开关
  *@param NULL
  *@retval NULL
  */
static void BlueTooth_LockOpen(void)
{
	if(Vertify_State == 0)
	{
		extern void Lock_Open(void);
		Lock_Open();
		BlueTooth_Printf("[Lock]Lock Open");
	}
	else
	{
		BlueTooth_Printf("[Lock]Already Open");
	}
}


/**
  *@brief 蓝牙发送帮助信息
  *@param NULL
  *@retval NULL
  */
static void BlueTooth_ShowHelp(void)
{
	BlueTooth_Printf("[HELP]Commands: STATUS, OPEN, CLOSE, RECORD\r\n");
	BlueTooth_Printf("[HELP]Commands:TEMP_ALARM = n, HUMI_ALARM = n, SMOG_ALARM = n\r\n");
	BlueTooth_Printf("[HELP]Commands:MOTOR_ON = n, MOTOR_MAX = n, LED = n\r\n");
}

/**
  *@brief 蓝牙命令处理结构体赋值
  *@param value 传入的参数
  *@retval NULL
  */
static const BlueTooth_CmdParam_t BlueTooth_CmdParam[] =
{
	{"TEMP_ALARM=%d", 20, 60, SetTempThre, "TempThrehold"},
	{"HUMI_ALARM=%d", 40, 100, SetHumiThre, "HumiThrehold"},
	{"SMOG_ALARM=%d", 1000, 6000, SetSmogThre, "SmogThrehold"},
	{"LED=%d", 0, 100, SetBrightness, "Brightness"},
	{"MOTOR_ON=%d", 15, 60, SetMotorOn, "Motor_On_Tem"},
	{"MOTOR_MAX=%d", 15, 60, SetMotorMax, "Motor_Max_Tem"},
};

/**
  *@brief 蓝牙命令显示结构体赋值
  *@param NULL
  *@retval NULL
  */
static const BlueTooth_Cmd_t BlueTooth_Cmd[]=
{
	{"STATUS", BlueTooth_ShowStatus, "STATUS"},
	{"OPEN", BlueTooth_LockOpen, "OPEN"},
	{"CLOSE", BlueTooth_LockClose, "CLOSE"},
	{"RECORD", BlueTooth_ShowRecord, "RECORD"},
	{"HELP", BlueTooth_ShowHelp, "HELP"},
};

static uint8_t CmdParam_Size = sizeof(BlueTooth_CmdParam) / sizeof(BlueTooth_CmdParam[0]);
static uint8_t Cmd_Size = sizeof(BlueTooth_Cmd) / sizeof(BlueTooth_Cmd[0]);

/**
  *@brief 蓝牙接收中断回调函数
  *@param huart 调用回调的串口句柄
	*@param Size 串口接收到的数据长度
  *@retval NULL
  */
void BlueTooth_Rx_Callback(uint16_t Size)
{
	if(Size < BT_BUF_SIZE)
	{
		BlueTooth_Rx_Buf[Size] = '\0';
	}
	else 
	{
		BlueTooth_Rx_Buf[BT_BUF_SIZE - 1] = '\0';
	}
	
	BlueTooth_Rx_Buf[strcspn((char*)BlueTooth_Rx_Buf, "\r\n")] = '\0';
	
	BlueTooth_Cmd_Ready = 1;
	
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, BlueTooth_Rx_Buf, BT_BUF_SIZE);
}

/**
  *@brief 蓝牙命令处理
  *@param NULL
  *@retval NULL
  */
void BlueTooth_ProcessCmd(void)
{
	if(BlueTooth_Cmd_Ready == 0){return;}
	BlueTooth_Cmd_Ready = 0;
	
	int value = 0;
	
	for(uint8_t i = 0; i < Cmd_Size; i++)
	{
		if(strcmp((const char*)BlueTooth_Rx_Buf, BlueTooth_Cmd[i].Cmd) == 0)
		{
			BlueTooth_Cmd[i].Show();
			return;
		}
	}
	
	for(uint8_t i = 0; i < CmdParam_Size; i++)
	{
		if(sscanf((const char*)BlueTooth_Rx_Buf, BlueTooth_CmdParam[i].Cmd, &value) == 1)
		{
			if(value >= BlueTooth_CmdParam[i].MinValue && value <= BlueTooth_CmdParam[i].MaxValue)
			{
				BlueTooth_CmdParam[i].Settle(value);
				BlueTooth_Printf("[Set]%s to %d\r\n", BlueTooth_CmdParam[i].Name, value);
			}
			else
			{
				BlueTooth_Printf("[ERR]%s Range %d to %d\r\n", BlueTooth_CmdParam[i].Name, BlueTooth_CmdParam[i].MinValue, BlueTooth_CmdParam[i].MaxValue);
			}
			return;
		}
	}
	BlueTooth_Printf("[ERR]Unkonwn CMD Send HELP To Get CMD\r\n");
}
