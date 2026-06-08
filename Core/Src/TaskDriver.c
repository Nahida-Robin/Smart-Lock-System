/** 
 * @file TaskDriver.c
 * @brief 驱动层，调用各个功能函数 所有UI界面均可按16返回上一级
 * @author Nahida
 * @date 2026.5.25
 */

#include "stm32f4xx.h"                  // Device header

#include "Matrix.h"
#include "Password.h"
#include "TickTime.h" 
#include "OLED.h"
#include "Delay.h"
#include "Serial.h"
#include "Display.h"
#include "StepMotor.h"
#include "PWM.h"
#include "Servo.h"
#include "BlueTooth.h"
#include "IRSensor.h"
#include "DataManage.h"
#include "Flash.h"
#include "RTCTime.h"

#define IR_LIGHTHOLD 10

extern TIM_HandleTypeDef htim3;
extern uint8_t Vertify_State;
extern uint8_t Succeses;
extern uint8_t Serial_Rx_Byte;
extern uint8_t BlueTooth_Rx_Byte;
extern uint8_t TempThrehold;
extern uint8_t HumiThrehold;
extern uint16_t SmogThrehold;
extern uint8_t LightThrehold;
extern uint8_t Motor_ON_Tem;
extern uint8_t Motor_MAX_Tem;
extern uint8_t Brightness;
extern uint8_t Light;
extern uint8_t Hour, Min, Sec;
extern uint8_t Year, Mon, Week, Date;

uint8_t Read_Flag = 0;
uint8_t View_Flag = 0;
uint8_t View_Pages = 1;
uint8_t View_Index = 1;
uint8_t Control_Mode = 0;
uint8_t RTCSet_Mode = 0;
uint8_t IR_LightState = 0;
uint16_t IR_LightTime = 0;



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

State_t State = STATE_IDLE;

/**
  *@brief 任务驱动初始化，初始化各功能
  *@param NULL
  *@retval NULL
  */
void TaskDriver_Init()
{
//	Flash_EraseChip();
	DWT_Delay_Init();
	HAL_TIM_Base_Start_IT(&htim3);
	Servo_Init();
	TickTime_Init();
	Serial_Init();
	BlueTooth_Init();
	OLED_Init();
	HAL_Delay(20);
	Display_ShowIDLE();
	Data_Read();
}

/**
  *@brief 开锁并记录成功次数
  *@param NULL
  *@retval NULL
  */
void Lock_Open(void)
{
	StepMotor_Forward_90_Degree(1);
	Succeses++;
	
}

/**
  *@brief 关索并清除开锁状态标志位
  *@param NULL
  *@retval NULL
  */
void Lock_Close(void)
{
	StepMotor_Forward_90_Degree(-1);
	Vertify_State = 0;
}


/**
  *@brief 检测错误次数
  *@param NULL
  *@retval NULL
  */
static void Err_Check(void)
{
	uint8_t Err = Open_Error();
	if(Err == 3)
	{
		printf("Normal Alarm");
	}
	else if(Err == 4)
	{
		printf("Special Alarm");
	}
}

/**
  *@brief 控制数值增加
  *@param NULL
  *@retval NULL
  */
static void Control_Increase(void)
{
		switch(Control_Mode)
		{
			case 1: 
				if(TempThrehold >= 60){TempThrehold = 10;}
				else{TempThrehold += 5;}
				Display_ShowTem();
				break;
			case 2:
				if(HumiThrehold >= 100){HumiThrehold = 20;}
				else{HumiThrehold += 5;}
				Display_ShowHum();
				break;
			case 3:
				if(SmogThrehold >= 6000){SmogThrehold = 1000;}
				else{SmogThrehold += 200;}
				Display_ShowSmo();
				break;
			case 4:
				if(Brightness >= 100){Brightness = 0;}
				else{Brightness += 10;}
				PWMLED_SetBright(Brightness);
				Display_ShowLED();
				break;
			case 5:
				if(Motor_ON_Tem >= 60){Motor_ON_Tem = 15;}
				else{Motor_ON_Tem++;}
				Display_ShowMoOn();
				break;
			case 6:
				if(Motor_MAX_Tem >= 60){Motor_MAX_Tem = 15;}
				else{Motor_MAX_Tem++;}
				Display_ShowMoOff();
				break;
		}
}

/**
  *@brief 控制数值减少
  *@param NULL
  *@retval NULL
  */
static void Control_Decrease(void)
{
		switch(Control_Mode)
		{
			case 1: 
				if(TempThrehold <= 10){TempThrehold = 60;}
				else{TempThrehold -= 5;}
				Display_ShowTem();
				break;
			case 2:
				if(HumiThrehold <= 20){HumiThrehold = 100;}
				else{HumiThrehold -= 5;}	
				Display_ShowHum();
				break;
			case 3:
				if(SmogThrehold <= 1000){SmogThrehold = 6000;}
				else{SmogThrehold -= 200;}	
				Display_ShowSmo();
				break;
			case 4:
				if(Brightness == 0){Brightness = 100;}
				else{Brightness -= 10;}
				PWMLED_SetBright(Brightness);
				Display_ShowLED();
				break;
			case 5:
				if(Motor_ON_Tem <= 15){Motor_ON_Tem = 60;}
				else{Motor_ON_Tem--;}
				Display_ShowMoOn();
				break;
			case 6:
				if(Motor_MAX_Tem <= 15){Motor_MAX_Tem = 60;}
				else{Motor_MAX_Tem--;}
				Display_ShowMoOff();
				break;
		}	
}

/**
  *@brief 控制数值增加
  *@param NULL
  *@retval NULL
  */
static void RTC_Increase(void)
{
	switch(RTCSet_Mode)
	{
		case 1:
			if(Year >= 99){Year = 0;}
			else{Year++;}
			Display_ShowDate();
			break;
		case 2:
			if(Mon >= 12){Mon = 1;}
			else{Mon++;}
			Display_ShowDate();
			break;
		case 3:
			if(Date >= 31){Date = 1;}//待匹配月份
			else{Date++;}
			Display_ShowDate();
			break;
		case 4:
			if(Hour >= 23){Hour = 0;}
			else{Hour++;}
			Display_ShowTime();
			break;
		case 5:
			if(Min >= 59){Min = 0;}
			else{Min++;}
			Display_ShowTime();
			break;
		case 6:
			if(Sec >= 59){Sec = 0;}
			else{Sec++;}
			Display_ShowTime();
			break;
		case 7:
			if(Week >= 7){Week = 1;}
			else{Week++;}
			Display_ShowDate();
			break;
	}
}

/**
  *@brief 控制数值减少
  *@param NULL
  *@retval NULL
  */
static void RTC_Decrease(void)
{
	switch(RTCSet_Mode)
	{
		case 1:
			if(Year <= 0){Year = 99;}
			else{Year--;}
			Display_ShowDate();
			break;
		case 2:
			if(Mon <= 1){Mon = 12;}
			else{Mon--;}
			Display_ShowDate();
			break;
		case 3:
			if(Date <= 1){Date = 31;}//待匹配月份
			else{Date--;}
			Display_ShowDate();
			break;
		case 4:
			if(Hour <= 0){Hour = 23;}
			else{Hour--;}
			Display_ShowTime();
			break;
		case 5:
			if(Min <= 0){Min = 59;}
			else{Min--;}
			Display_ShowTime();
			break;
		case 6:
			if(Sec <= 0){Sec = 59;}
			else{Sec--;}
			Display_ShowTime();
			break;
		case 7:
			if(Week <= 1){Week = 7;}
			else{Week--;}
			Display_ShowDate();
			break;
	}
}

/**
  *@brief 红外点灯逻辑
  *@param NULL
  *@retval NULL
  */
static void IR_LightControl(void)
{
	if(Light < LightThrehold && IRSensor_Detected())
	{
		PWMLED_SetBright(50);
		IR_LightState = 1;
		IR_LightTime = IR_LIGHTHOLD;
	}
	else if(IR_LightTime)
	{
		IR_LightTime--;
	}
	else if(IR_LightTime == 0 && IR_LightState)
	{
		IR_LightState = 0;
		PWMLED_SetBright(Brightness);
	}
}

/**
  *@brief 空闲任务 显示当前时间 等待密码输入
  *@param NULL
  *@retval NULL
  */
static void IDLE_Task(void)
{
	Display_ShowTime();
	Display_ShowDate();
	uint8_t Key = Matrix_GetNum();
	switch(Key)
	{
		case 11:
			State = STATE_VERTIFY;
			Display_InputInt();
			return;			
		case 12:
			State = STATE_MENU;
			Display_ShowMainMenu();
			return;			
	}
}

/**
  *@brief 菜单任务函数 刷新各个数据 显示当前参数
  *@param NULL
  *@retval NULL
  */
static void MENU_Task(void)
{
	Display_ShowLockSuc();
	Display_ShowLockErr();
//	Display_ShowLockState();
	Display_ShowDHT();
	Display_ShowSmog();
	Display_ShowLight();
	uint8_t Key = Matrix_GetNum();
	
	switch(Key)
	{
		case 15:
			State = STATE_RTC;
			Display_ShowRTC();
			return;
		case 16:
			State = STATE_IDLE;
			Display_ShowIDLE();
			return;
	}
}

/**
  *@brief 密码验证函数 调用password接口 按返回值进行不同处理
  *@param NULL
  *@retval NULL
  */
static void VERTIFY_Task(void)
{
	int8_t Result = Password_Vertify();
	
	switch(Result)
	{
		case 1:
			Display_ShowInput();
			break;
		case 2:
//			Lock_Open();
		  Display_ShowOK();
			Data_Save();
		  State = STATE_OPEN;
		  Display_ShowOpen();
		  return;
		case -1:
			Display_ShowLenErr();
			Data_Save();
		  Display_InputInt();
			break;
		case -2:
			Err_Check();
		  Display_ShowPwdErr();
			Data_Save();
		  Display_InputInt();
			break;
		case -3:
			Display_ShowMemErr();
			Data_Save();
		  Display_InputInt();
			break;
		case 6:
			State = STATE_IDLE;
			Display_ShowIDLE();
			break;
	}
}

/**
  *@brief 开锁状态函数 等待按键按下 控制阈值 进入空闲 更改密码 查看历史记录
  *@param NULL
  *@retval NULL
  */
static void OPEN_Task()
{
	uint8_t Key = Matrix_GetNum();
	
	switch(Key)
	{
		case 12:
			State = STATE_CONT;
		  Display_ShowControl();
			break;
		case 13:
			State = STATE_SEL;
		  Display_SelInt();
			break;
		case 14:
			State = STATE_CHAPWD;
		  Display_ChaInt();
			break;
		case 15:
			Read_Flag = 0;
			View_Flag = 0;
			View_Index = 1;
			State = STATE_VIEW;
			Data_Read();
		  Display_ViewInt();
			break;
		case 16:
//			Lock_Close();
		  State = STATE_IDLE;
		  Display_ShowMainMenu();
			break;
	}
}

/**
  *@brief 更改密码函数 调用password接口 根据返回值判断更改成功与否
  *@param NULL
  *@retval NULL
  */
static void CHAPWD_Task()
{
	int8_t Result = Password_Change();
	
	switch(Result)
	{
		case 1:
			Display_ShowChaPwd();
		  return;
		case 5:
			Display_ShowChaOK();
		  State = STATE_OPEN;
		  Display_ShowOpen();
			break;
		case -1:
			Display_ShowChaErr();
		  State = STATE_OPEN;
		  Display_ShowOpen();
			break;
		case 6:
			State = STATE_OPEN;
		  Display_ShowOpen();
			break;
	}
}

/**
  *@brief 查看历史记录函数 显示历史密码输入和时间
  *@param NULL
  *@retval NULL
  */
static void VIEW_Task()
{
	int8_t Result = Display_ReadView();
	Display_ShowHistory();
	uint8_t Key = Matrix_GetNum();
	switch(Key)
	{
		case 11:
			if(View_Pages >= 1 && View_Index < View_Pages)
			{
				View_Index++;
				View_Flag = 0;
				Display_ShowHistory();
			}
			break;
		case 12:
			if(View_Pages >= 1 && View_Index > 1)
			{
				View_Index--;
				View_Flag = 0;
				Display_ShowHistory();
			}
			break;
		case 14:
			Read_Flag = 0;
			View_Flag = 0;
			View_Index = 1;
			Data_Erase();
			State = STATE_OPEN;
		  Display_ShowOpen();
			break;
		case 16:
			Read_Flag = 0;
			View_Flag = 0;
			View_Index = 1;
		  State = STATE_OPEN;
		  Display_ShowOpen();
			break;
	}
}

/**
  *@brief 密码选择函数
  *@param NULL
  *@retval NULL
  */
static void PWSEL_Task(void)
{
	int8_t Result = Password_Select();
	
	switch(Result)
	{
		case 1:
			Display_Showsel();
			break;
		case 7:
			State = STATE_OPEN;
		  Display_ShowOpen();
			break;
		case 8:
			Display_SelInt();
		  return;
		case 6:
			State = STATE_OPEN;	
		  Display_ShowOpen();
			break;
	}
}

/**
  *@brief 控制任务 模式切换 数值增加 数值减少 返回上一级
  *@param NULL
  *@retval NULL
  */
static void CONT_Task(void)
{
	uint8_t Key = Matrix_GetNum();
	
	switch(Key)
	{
		case 12:
			Control_Mode++;
			if(Control_Mode >= 7) {Control_Mode = 1;}
			Display_ShowSet();
			break;
		case 13:
			Control_Increase();
			break;
		case 14:
			Control_Decrease();
			break;
		case 16:
			State = STATE_OPEN;
			Control_Mode = 0;
		  Display_ShowOpen();
			break;
	}
}

/**
  *@brief RTC设置函数
  *@param NULL
  *@retval NULL
  */
static void RTC_Task(void)
{
	uint8_t Key = Matrix_GetNum();
	
	switch(Key)
	{
		case 11:
			RTC_SetTime(Hour, Min, Sec);
			RTC_SetDate(Year, Mon, Date, Week);
			State = STATE_IDLE;
			Display_ShowIDLE();
			break;
		case 12:
			if(RTCSet_Mode == 0){Display_ShowRTCSetInc();}
			RTCSet_Mode++;
			if(RTCSet_Mode >= 8){RTCSet_Mode = 1;}
			Display_ShowRTCSet();
			break;
		case 13:
			RTC_Increase();
			break;
		case 14:
			RTC_Decrease();
			break;
		case 16:
			State = STATE_MENU;
		  RTCSet_Mode = 0;
			Display_ShowMainMenu();
			break;
	}
}

/**
  *@brief 串口接收统一回调
  *@param NULL
  *@retval NULL
  */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == USART1)
	{
		Serial_Rx_Callback(Size);
	}
	if(huart->Instance == USART2)
	{
		BlueTooth_Rx_Callback(Size);
	}
}

/**
  *@brief 任务驱动运行函数 主函数里循环执行此函数
  *@param NULL
  *@retval NULL
  */
void TaskDriver_Run(void)
{
	IR_LightControl();
	
	Serial_ProcessCmd();
	BlueTooth_ProcessCmd();
	
	switch(State)
	{
		case STATE_IDLE: IDLE_Task();break;
		case STATE_MENU: MENU_Task();break;
		case STATE_VERTIFY: VERTIFY_Task();break;
		case STATE_OPEN: OPEN_Task();break;
		case STATE_CHAPWD: CHAPWD_Task();break;
		case STATE_VIEW: VIEW_Task();break;
		case STATE_SEL: PWSEL_Task();break;
		case STATE_CONT: CONT_Task();break;
		case STATE_RTC: RTC_Task();break;
	}
	
}


