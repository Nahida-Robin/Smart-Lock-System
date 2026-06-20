/**
 * @file TaskDriver.c
 * @brief 智能锁系统 5个RTOS任务，通过队列通信
 * @author Nahida
 * @date 2026.5.25
 */

#include "stm32f4xx.h"                  // Device header
#include "cmsis_os.h"
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
#include "SensorRead.h"
#include "TaskDriver.h"

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
extern uint8_t Hour, Min, Sec;
extern uint8_t Year, Mon, Week, Date;

uint8_t View_Pages = 1;
extern osMessageQueueId_t KeyQueueHandle;
extern osMessageQueueId_t HwCtrlQueueHandle;
extern osThreadId_t CommTaskHandle;

extern osMutexId_t OLED_MutexHandle;

uint8_t Read_Flag = 0;
uint8_t View_Flag = 0;
uint8_t View_Index = 1;
uint8_t Control_Mode = 0;
uint8_t RTCSet_Mode = 0;
uint8_t IR_LightState = 0;
uint16_t IR_LightTime = 0;
uint8_t Month_Day[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

SensorData_t g_SensorData = {0};

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
  *@brief 任务驱动初始化，初始化各外设和驱动
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
  *@brief 关锁并清除开锁状态标志位
  *@param NULL
  *@retval NULL
  */
void Lock_Close(void)
{
	StepMotor_Forward_90_Degree(-1);
	Vertify_State = 0;
}

/**
  *@brief 检测错误次数，触发报警
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
			{
				HwCtrlMsg_t hwMsg = {HW_CMD_SET_BRIGHTNESS, Brightness};
				osMessageQueuePut(HwCtrlQueueHandle, &hwMsg, 0, 0);
			}
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
  *@brief RTC数值增加
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
			Display_ShowSetDate();
			if(Year % 4 == 0 && Year % 100 != 0){Month_Day[1] = 29;}
			else {Month_Day[1] = 28;}
			break;
		case 2:
			if(Mon >= 12){Mon = 1;}
			else{Mon++;}
			Display_ShowSetDate();
			break;
		case 3:
			if(Date >= Month_Day[Mon - 1]){Date = 1;}
			else{Date++;}
			Display_ShowSetDate();
			break;
		case 4:
			if(Hour >= 23){Hour = 0;}
			else{Hour++;}
			Display_ShowSetTime();
			break;
		case 5:
			if(Min >= 59){Min = 0;}
			else{Min++;}
			Display_ShowSetTime();
			break;
		case 6:
			if(Sec >= 59){Sec = 0;}
			else{Sec++;}
			Display_ShowSetTime();
			break;
		case 7:
			if(Week >= 7){Week = 1;}
			else{Week++;}
			Display_ShowSetDate();
			break;
	}
}

/**
  *@brief RTC数值减少
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
			Display_ShowSetDate();
			if(Year % 4 == 0 && Year % 100 != 0){Month_Day[1] = 29;}
			else {Month_Day[1] = 28;}
			break;
		case 2:
			if(Mon <= 1){Mon = 12;}
			else{Mon--;}
			Display_ShowSetDate();
			break;
		case 3:
			if(Date <= 1){Date = Month_Day[Mon - 1];}
			else{Date--;}
			Display_ShowSetDate();
			break;
		case 4:
			if(Hour <= 0){Hour = 23;}
			else{Hour--;}
			Display_ShowSetTime();
			break;
		case 5:
			if(Min <= 0){Min = 59;}
			else{Min--;}
			Display_ShowSetTime();
			break;
		case 6:
			if(Sec <= 0){Sec = 59;}
			else{Sec--;}
			Display_ShowSetTime();
			break;
		case 7:
			if(Week <= 1){Week = 7;}
			else{Week--;}
			Display_ShowSetDate();
			break;
	}
}

/**
  *@brief 红外自动补光控制
  *@param NULL
  *@retval NULL
  */
static void IR_LightControl(void)
{
	if(g_SensorData.Light < LightThrehold && IRSensor_Detected())
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
  *@brief 按键扫描任务 通过队列发送给UI任务
  *@param NULL
  *@retval NULL
  */
void StartKeyScan_Task(void *argument)
{
	uint8_t key_buf;

	for(;;)
	{
		uint8_t key = Matrix_GetNum();
		if(key != 0)
		{
			key_buf = key;
			osMessageQueuePut(KeyQueueHandle, &key_buf, 0, 0);
		}
		osDelay(20);
	}
}

/**
  *@brief UI主任务 处理所有UI界面状态切换，通过队列接收按键和传感器数据
  *@param NULL
  *@retval NULL
  */
void StartUI_Task(void *argument)
{
	for(;;)
	{
//    static uint32_t lastPrint = 0;
//    if(HAL_GetTick() - lastPrint > 1000)
//    {
//        lastPrint = HAL_GetTick();
//        UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
//        printf("UITask stack free: %d bytes\r\n", stackHighWaterMark * 4); 
//    }
		
		osMutexAcquire(OLED_MutexHandle, osWaitForever);

		switch(State)
		{
			case STATE_IDLE:
			{
				Display_ShowTime();
				Display_ShowDate();
				uint8_t key = 0;
				if(osMessageQueueGet(KeyQueueHandle, &key, NULL, 0) == osOK)
				{
					switch(key)
					{
						case 11:
							State = STATE_VERTIFY;
							Display_InputInt();
							break;
						case 12:
							State = STATE_MENU;
							Display_ShowMainMenu();
							break;
					}
				}
				break;
			}
			case STATE_MENU:
			{
				Display_ShowLockSuc();
				Display_ShowLockErr();
				Display_ShowDHT(&g_SensorData);
				Display_ShowSmog(&g_SensorData);
				Display_ShowLight(&g_SensorData);
				uint8_t key = 0;
				if(osMessageQueueGet(KeyQueueHandle, &key, NULL, 0) == osOK)
				{
					switch(key)
					{
						case 15:
							State = STATE_RTC;
							Display_ShowRTC();
							break;
						case 16:
							State = STATE_IDLE;
							Display_ShowIDLE();
							break;
					}
				}
				break;
			}
			case STATE_VERTIFY:
			{
				uint8_t key = 0;
				if(osMessageQueueGet(KeyQueueHandle, &key, NULL, 0) == osOK)
				{
					int8_t Result = Password_Vertify(key);
					switch(Result)
					{
						case 1:
							Display_ShowInput();
							break;
						case 2:
							Display_ShowOK();
							Data_Save();
							State = STATE_OPEN;
							Display_ShowOpen();
							break;
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
				break;
			}
			case STATE_OPEN:
			{
				uint8_t key = 0;
				if(osMessageQueueGet(KeyQueueHandle, &key, NULL, 0) == osOK)
				{
					switch(key)
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
							State = STATE_IDLE;
							Display_ShowIDLE();
							break;
					}
				}
				break;
			}
			case STATE_CHAPWD:
			{
				uint8_t key = 0;
				if(osMessageQueueGet(KeyQueueHandle, &key, NULL, 0) == osOK)
				{
					int8_t Result = Password_Change(key);
					switch(Result)
					{
						case 1:
							Display_ShowChaPwd();
							break;
						case 5:
							Data_Save();
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
				break;
			}
			case STATE_VIEW:
			{
				int8_t Result = Display_ReadView();
				Display_ShowHistory();
				uint8_t key = 0;
				if(osMessageQueueGet(KeyQueueHandle, &key, NULL, 0) == osOK)
				{
					switch(key)
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
//							Read_Flag = 0;
//							View_Flag = 0;
//							View_Index = 1;
							State = STATE_OPEN;
							Display_ShowOpen();
							break;
					}
				}
				break;
			}
			case STATE_SEL:
			{
				uint8_t key = 0;
				if(osMessageQueueGet(KeyQueueHandle, &key, NULL, 0) == osOK)
				{
					int8_t Result = Password_Select(key);
					switch(Result)
					{
						case 1:
							Display_Showsel();
							break;
						case 7:
							Data_Save();
							State = STATE_OPEN;
							Display_ShowOpen();
							break;
						case 8:
							Display_SelInt();
							break;
						case 6:
							State = STATE_OPEN;
							Display_ShowOpen();
							break;
					}
				}
				break;
			}
			case STATE_CONT:
			{
				uint8_t key = 0;
				if(osMessageQueueGet(KeyQueueHandle, &key, NULL, 0) == osOK)
				{
					switch(key)
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
				break;
			}
			case STATE_RTC:
			{
				uint8_t key = 0;
				if(osMessageQueueGet(KeyQueueHandle, &key, NULL, 0) == osOK)
				{
					switch(key)
					{
						case 11:
							RTC_SetTime(Hour, Min, Sec);
							RTC_SetDate(Year, Mon, Week, Date);
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
				break;
			}
		}

		osMutexRelease(OLED_MutexHandle);
		osDelay(50);
	}
}

/**
  *@brief 传感器采集任务
  *@param NULL
  *@retval NULL
  */
void StartSensor_Task(void *argument)
{
	SensorRead_Init();

	for(;;)
	{
		SensorData_t sensorData;

		SensorRead_Run(&sensorData);

		taskENTER_CRITICAL();
		g_SensorData = sensorData;
		taskEXIT_CRITICAL();

		// osMessageQueuePut(SensorDataQueueHandle, &sensorData, 0, 0);

		osDelay(2000);
	}
}

/**
  *@brief 通信任务 
  *@param NULL
  *@retval NULL
  */
void StartComm_Task(void *argument)
{
	for(;;)
	{
		osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);

		Serial_ProcessCmd();
		BlueTooth_ProcessCmd();
		// osDelay(50);
	}
}

/**
  *@brief 硬件任务
  *@param NULL
  *@retval NULL
  */
void StartHwControl_Task(void *argument)
{
	for(;;)
	{
		HwCtrlMsg_t msg;
		if(osMessageQueueGet(HwCtrlQueueHandle, &msg, NULL, 0) == osOK)
		{
			switch(msg.cmd)
			{
				case HW_CMD_SET_BRIGHTNESS:
					PWMLED_SetBright(msg.data);
					break;
				default:
					break;
			}
		}

		IR_LightControl();
		osDelay(100);
	}
}

/**
  *@brief 串口接收统一回调
  *@param huart 触发回调的串口句柄
  *@param Size 本次接收的数据长度
  *@retval NULL
  */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == USART1)
	{
		Serial_Rx_Callback(Size);
		osThreadFlagsSet(CommTaskHandle, 0x01);
	}
	if(huart->Instance == USART2)
	{
		BlueTooth_Rx_Callback(Size);
		osThreadFlagsSet(CommTaskHandle, 0x01);
	}
}
