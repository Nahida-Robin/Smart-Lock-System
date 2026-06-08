/** 
 * @file Serial.c
 * @brief 串口功能
 * @author Nahida
 * @date 2026.5.26
 */
#include "stm32f4xx.h"                  // Device header
#include "Password.h"
#include "TaskDriver.h"
#include "PWM.h"
#include <stdio.h>
#include <string.h>

#define UW_MAX_SIZE 512
#define PW_MAX_SIZE 512
#define TM_MAX_SIZE 128
#define SR_BUF_SIZE 64

extern UART_HandleTypeDef huart1;
extern uint8_t Vertify_State;
extern uint8_t Temph;
extern uint8_t Templ;
extern uint8_t Humih;
extern uint8_t Humil;
extern uint8_t Warnings;
extern uint8_t Succeses;
extern uint8_t Light;
extern float Smog;
volatile uint8_t Serial_Cmd_Ready = 0;
volatile uint8_t Serial_Rx_Index = 0;
uint8_t Serial_Rx_Buf[SR_BUF_SIZE];

extern uint8_t PasswordMemory[PW_MAX_SIZE];
extern uint8_t UserwordMemory[UW_MAX_SIZE];
extern uint8_t TimeMemory[TM_MAX_SIZE];

extern volatile uint16_t PWMemory_Index;
extern volatile uint16_t UWMemory_Index;
extern volatile uint16_t TMMemory_Index;


/**
  *@brief 串口处理结构体
  *@param NULL
  *@retval NULL
  */
typedef struct{
	const char* Cmd;
	int MinValue;
	int MaxValue;
	void (*Settle)(int value);
	const char* Name;
}Serial_CmdParam_t;

/**
  *@brief 串口显示结构体
  *@param NULL
  *@retval NULL
  */
typedef struct{
	const char* Cmd;
	void (*Show)(void);
	const char* Name;
}Serial_Cmd_t;

/**
  *@brief 串口初始化 
  *@param NULL
  *@retval NULL
  */
void Serial_Init(void)
{
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, Serial_Rx_Buf, SR_BUF_SIZE);
}

/**
  *@brief 串口重定向
  *@param NULL
  *@retval ch 字节
  */
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,HAL_MAX_DELAY);
	return ch;	
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
  *@brief 串口打印状态信息
  *@param NULL
  *@retval NULL
  */
static void Serial_ShowStatus(void)
{
	printf("[Status]Lock:%s Temp:%d.%d Humi:%d.%d Smog:%dppm Light:%d Unlockcnt:%d Warnings:%d\r\n", 
					Vertify_State ? "Open" : "Close", 
					Temph, Templ, Humih, Humil, (uint16_t)Smog, Light, Succeses, Warnings);
}

/**
  *@brief 串口打印历史密码记录
  *@param NULL
  *@retval NULL
  */
static void Serial_ShowRecord(void)
{
		uint16_t Uw_Index = 0;
		uint16_t Tm_Index = 0;
		uint8_t Rec_Num = 0;
		if(UWMemory_Index == 0 || TMMemory_Index == 0)
		{
			printf("[Error]No Records\r\n");
			printf("Records END\r\n");
			return;
		}
		printf("[Records]\r\n");
		while(Uw_Index < UWMemory_Index && Tm_Index < TMMemory_Index)
		{
			uint8_t Hour = TimeMemory[Tm_Index++];
			uint8_t Min = TimeMemory[Tm_Index++];
			uint8_t Sec = TimeMemory[Tm_Index++];
			if(Tm_Index < TMMemory_Index && TimeMemory[Tm_Index] == 'Z'){Tm_Index++;}
			printf("Rec%d Time:%02d:%02d:%02d Pwd:", ++Rec_Num, Hour, Min, Sec);

			while(Uw_Index < UWMemory_Index && UserwordMemory[Uw_Index] != 'A')
			{
				printf("%d", UserwordMemory[Uw_Index++]);
			}
			if(Uw_Index < UWMemory_Index && UserwordMemory[Uw_Index] == 'A'){Uw_Index++;}
			printf("\r\n");
		}
	printf("Records END\r\n");
}

/**
  *@brief 串口关索
  *@param NULL
  *@retval NULL
  */
static void Serial_LockClose(void)
{
	if(Vertify_State == 1)
	{
		extern void Lock_Close(void);
		Lock_Close();
		printf("[Lock]Lock Close\r\n");
	}
	else
	{
		printf("[Lock]Already Close\r\n");
	}
}

/**
  *@brief 串口开锁
  *@param NULL
  *@retval NULL
  */
static void Serial_LockOpen(void)
{
	if(Vertify_State == 0)
	{
		extern void Lock_Open(void);
		Lock_Open();
		printf("[Lock]Lock Open\r\n");
	}
	else
	{
		printf("[Lock]Already Open\r\n");
	}
}

/**
  *@brief 串口打印帮助信息
  *@param NULL
  *@retval NULL
  */
static void Serial_ShowHelp(void)
{
	printf("[HELP]Commands: STATUS, OPEN, CLOSE, RECORD\r\n");
	printf("[HELP]Commands:TEMP_ALARM = n, HUMI_ALARM = n, SMOG_ALARM = n\r\n");
	printf("[HELP]Commands:MOTOR_ON = n, MOTOR_MAX = n, LED = n\r\n");
}

/**
  *@brief 串口命令结构体赋值
  *@param value 传入的参数
  *@retval NULL
  */
static const Serial_CmdParam_t Serial_CmdParam[] =
{
	{"TEMP_ALARM=%d", 20, 60, SetTempThre, "TempThrehold"},
	{"HUMI_ALARM=%d", 40, 100, SetHumiThre, "HumiThrehold"},
	{"SMOG_ALARM=%d", 1000, 6000, SetSmogThre, "SmogThrehold"},
	{"LED=%d", 0, 100, SetBrightness, "Brightness"},
	{"MOTOR_ON=%d", 15, 60, SetMotorOn, "Motor_On_Tem"},
	{"MOTOR_MAX=%d", 15, 60, SetMotorMax, "Motor_Max_Tem"},
};

/**
  *@brief 串口显示结构体赋值
  *@param value 传入的参数
  *@retval NULL
  */
static const Serial_Cmd_t Serial_Cmd[]=
{
	{"STATUS", Serial_ShowStatus, "STATUS"},
	{"OPEN", Serial_LockOpen, "OPEN"},
	{"CLOSE", Serial_LockClose, "CLOSE"},
	{"RECORD", Serial_ShowRecord, "RECORD"},
	{"HELP", Serial_ShowHelp, "HELP"},
};

static uint8_t CmdParam_Size = sizeof(Serial_CmdParam) / sizeof(Serial_CmdParam[0]);
static uint8_t Cmd_Size = sizeof(Serial_Cmd) / sizeof(Serial_Cmd[0]);

/**
  *@brief 串口接收回调
  *@param huart 调用回调的串口句柄
	*@param Size 串口接收到的数据长度
  *@retval NULL
  */
void Serial_Rx_Callback(uint16_t Size)
{
	if(Size < SR_BUF_SIZE)
	{
		Serial_Rx_Buf[Size] = '\0';
	}
	else
	{
		Serial_Rx_Buf[SR_BUF_SIZE - 1] = '\0';
	}
		
	Serial_Rx_Buf[strcspn((char*)Serial_Rx_Buf, "\r\n")] = '\0';
	
	Serial_Cmd_Ready = 1;
	
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, Serial_Rx_Buf, SR_BUF_SIZE);	
}

/**
  *@brief 串口接收命令处理信息
  *@param NULL
  *@retval NULL
  */
void Serial_ProcessCmd(void)
{
	if(Serial_Cmd_Ready == 0){return;}
	Serial_Cmd_Ready = 0;
	
	int value = 0;
	
	for(uint8_t i = 0; i < Cmd_Size; i++)
	{
		if(strcmp((const char*)Serial_Rx_Buf, Serial_Cmd[i].Cmd) == 0)
		{
			Serial_Cmd[i].Show();
			return;
		}
	}
	
	for(uint8_t i = 0; i < CmdParam_Size; i++)
	{
		if(sscanf((const char*)Serial_Rx_Buf, Serial_CmdParam[i].Cmd, &value) == 1)
		{
			if(value >= Serial_CmdParam[i].MinValue && value <= Serial_CmdParam[i].MaxValue)
			{
				Serial_CmdParam[i].Settle(value);
				printf("[Set]%s to %d\r\n", Serial_CmdParam[i].Name, value);
			}
			else
			{
				printf("[ERR]%s Range %d to %d\r\n", Serial_CmdParam[i].Name, Serial_CmdParam[i].MinValue, Serial_CmdParam[i].MaxValue);
			}
			return;
		}
	}
	printf("[ERR]Unkonwn CMD Send HELP To Get CMD\r\n");
}
