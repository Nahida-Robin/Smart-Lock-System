/**
 * @file SensorRead.c
 * @brief 多传感器读取模块，集成DHT11温湿度传感器、ADC读取烟雾和光照传感器。通过定时器中断和ADC转换完成回调函数实现定时读取数据更新
 * @author Nahida
 * @date 2026.6.2
 */

#include "stm32f4xx.h"                  // Device header
#include "dht11.h"
#include "LED.h"
#include "Buzzer.h"
#include "Motor.h"
#include "PWM.h"

#define ADC_SIZE 2
#define LIGHT_MAX_VALUE 1500
#define LIGHT_MIN_VALUE 300

volatile uint16_t ADC_BUF[ADC_SIZE];
volatile uint8_t ADC_Ready = 0;
volatile uint8_t DHT_Ready = 0;

uint8_t Temph = 0;
uint8_t Templ = 0;
uint8_t Humih = 0;
uint8_t Humil = 0;
uint8_t Light = 0;
float Smog = 0;
uint8_t Brightness = 0;
static uint16_t Smog_Value = 0;
static uint16_t Light_Value = 0;

uint8_t TempThrehold = 25;
uint8_t HumiThrehold = 90;
uint16_t SmogThrehold = 3000;
uint8_t LightThrehold = 50;
uint8_t Motor_ON_Tem = 18;
uint8_t Motor_MAX_Tem = 30;

uint8_t TempAlarm = 0;
uint8_t HumiAlarm = 0;
uint8_t SmogAlarm = 0;

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;

typedef enum{
	SENSOR_IDLE = 0,
	SENSOR_READ,
	SENSOR_CHECK
}Sensor_t;

Sensor_t Sensor = SENSOR_IDLE;

/**
  *@brief 传感器读取初始化
  *@param NULL
  *@retval NULL
  */
void SensorRead_Init(void)
{
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_BUF, ADC_SIZE);
	PWMLED_Init();
	Motor_Init();
}

/**
  *@brief 报警输出
  *@param NULL
  *@retval NULL
  */
static void Alarm_Output(void)
{
	if(TempAlarm || HumiAlarm || SmogAlarm)
	{
		LED_On();
		Buzzer_on();
	}
	else
	{
		LED_Off();
		Buzzer_off();
	}
}

/**
  *@brief 温度报警检测
  *@param NULL
  *@retval NULL
  */
static void TemAlarm_Check(void)
{
	if(Temph > TempThrehold)
	{
		if(TempAlarm == 0)
		{
			TempAlarm = 1;
			printf("TempAlarm\r\n");
		}
	}
	else
	{
		TempAlarm = 0;
	}

	Alarm_Output();
}

/**
  *@brief 湿度报警检测
  *@param NULL
  *@retval NULL
  */
static void HumAlarm_Check(void)
{
	if(Humih > HumiThrehold)
	{
		if(HumiAlarm == 0)
		{
			HumiAlarm = 1;
			printf("HumiAlarm\r\n");
		}
	}
	else
	{
		HumiAlarm = 0;
	}

	Alarm_Output();
}

/**
  *@brief 烟雾报警检测
  *@param NULL
  *@retval NULL
  */
static void SmoAlarm_Check(void)
{
	if(Smog > SmogThrehold)
	{
		if(SmogAlarm == 0)
		{
			SmogAlarm = 1;
			printf("SmogAlarm\r\n");
		}
	}
	else
	{
		SmogAlarm = 0;
	}

	Alarm_Output();
}

/**
  *@brief 风扇控制逻辑
  *@param NULL
  *@retval NULL
  */
static void Motor_TemControl(void)
{
	if(Temph > Motor_MAX_Tem){Motor_SetSpeed(100);}
	else if(Temph < Motor_ON_Tem){Motor_SetSpeed(0);}
	else if(Temph > Motor_ON_Tem && Temph < Motor_MAX_Tem)
	{
		uint16_t Speed = (uint16_t)(Temph - Motor_ON_Tem) * 100 / (Motor_MAX_Tem - Motor_ON_Tem);
		Motor_SetSpeed(Speed);
	}
}

/**
  *@brief TIM2定时器中断回调 每2s触发中断一次
  *@param htim 定时器句柄
  *@retval NULL
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
    DHT_Ready = 1;
  }
}

/**
  *@brief ADC转换完成回调函数
  *@param NULL
  *@retval NULL
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC1)
	{
		Smog_Value = ADC_BUF[0];
		Light_Value = ADC_BUF[1];
		ADC_Ready = 1;
		HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_BUF, ADC_SIZE);
	}
}

/**
  *@brief 传感器读取主循环 状态机实现
  *@param NULL
  *@retval NULL
  */
void SensorRead_Run(void)
{
	switch(Sensor)
	{
		case SENSOR_IDLE:
			if(DHT_Ready || ADC_Ready){Sensor = SENSOR_READ;}
			break;
		case SENSOR_READ:
			DHT_Read(&Temph, &Templ, &Humih, &Humil);
			Smog = (((float)Smog_Value / 4096.0f * 3.3f - 0.4f) / 0.4f * 1000.0f);
			Light = (LIGHT_MAX_VALUE - Light_Value) * 100 / (LIGHT_MAX_VALUE - LIGHT_MIN_VALUE);
			DHT_Ready = 0;
			ADC_Ready = 0;
			Sensor = SENSOR_CHECK;
			break;
		case SENSOR_CHECK:
			TemAlarm_Check();
			HumAlarm_Check();
			SmoAlarm_Check();
			Motor_TemControl();
			Sensor = SENSOR_IDLE;
			break;
	}
}
