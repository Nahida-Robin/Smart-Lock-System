/**
 * @file SensorRead.c
 * @brief 传感器读取，调用DHT11、ADC，填充结构体通过队列传递
 * @author Nahida
 * @date 2026.6.2
 */

#include "stm32f4xx.h"
#include "dht11.h"
#include "SensorRead.h"
#include "LED.h"
#include "Buzzer.h"
#include "Motor.h"
#include "PWM.h"
#include <stdint.h>

#define ADC_SIZE 2
#define LIGHT_MAX_VALUE 1500
#define LIGHT_MIN_VALUE 300

volatile uint16_t ADC_BUF[ADC_SIZE];
volatile uint8_t  ADC_Ready = 0;

uint8_t TempThrehold  = 25;
uint8_t HumiThrehold  = 90;
uint16_t SmogThrehold = 3000;
uint8_t LightThrehold = 50;
uint8_t Motor_ON_Tem  = 18;
uint8_t Motor_MAX_Tem = 30;
uint8_t Brightness    = 50;

uint8_t TempAlarm = 0;
uint8_t HumiAlarm = 0;
uint8_t SmogAlarm = 0;

extern ADC_HandleTypeDef hadc1;
// extern TIM_HandleTypeDef htim2;


/**
  *@brief 传感器读取初始化
  *@param NULL
  *@retval NULL
  */
void SensorRead_Init(void)
{
//	HAL_TIM_Base_Start_IT(&htim2);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_BUF, ADC_SIZE);
	PWMLED_Init();
	Motor_Init();
}

/**
  *@brief 中断回调，更新系统tick
  *@param NULL
  *@retval NULL
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//DHT定时读取已通过挂起态实现，不需要定时器定时读取
	// if (htim->Instance == TIM2)
	// {
	// 	DHT_Ready = 1;
	// }
	if (htim->Instance == TIM14)
	{
		HAL_IncTick();
	}
}

/**
  *@brief ADC搬运完成回调，设置ADC_Ready标志以供主循环读取
  *@param hadc：触发回调的ADC句柄
  *@retval NULL
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC1)
	{
		ADC_Ready = 1;
		HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_BUF, ADC_SIZE);
	}
}

/**
  *@brief 执行一次完整的传感器数据采集
  *@param pData 输出参数，存放采集到的传感器数据
  *@retval NULL
  */
void SensorRead_Run(SensorData_t *pData)
{
	uint16_t smog_val = 0;
	uint16_t light_val = 0;

	if(ADC_Ready)
	{
		smog_val   = ADC_BUF[0];
		light_val  = ADC_BUF[1];
		ADC_Ready = 0;
	}

	pData->Temph = 0;
	pData->Templ = 0;
	pData->Humih = 0;
	pData->Humil = 0;
	DHT_Read(&pData->Temph, &pData->Templ, &pData->Humih, &pData->Humil);

	pData->Smog  = (((float)smog_val / 4096.0f * 3.3f - 0.4f) / 0.4f * 1000.0f);
	pData->Light = (LIGHT_MAX_VALUE - light_val) * 100 / (LIGHT_MAX_VALUE - LIGHT_MIN_VALUE);


	if(pData->Temph > TempThrehold) {
		if(TempAlarm == 0) { TempAlarm = 1; printf("TempAlarm\r\n"); }
	} else {
		TempAlarm = 0;
	}
	if(pData->Humih > HumiThrehold) {
		if(HumiAlarm == 0) { HumiAlarm = 1; printf("HumiAlarm\r\n"); }
	} else {
		HumiAlarm = 0;
	}
	if(pData->Smog > SmogThrehold) {
		if(SmogAlarm == 0) { SmogAlarm = 1; printf("SmogAlarm\r\n"); }
	} else {
		SmogAlarm = 0;
	}
	if(TempAlarm || HumiAlarm || SmogAlarm) {
		LED_On(); Buzzer_on();
	} else {
		LED_Off(); Buzzer_off();
	}

	if(pData->Temph > Motor_MAX_Tem) {
		Motor_SetSpeed(100);
	} else if(pData->Temph < Motor_ON_Tem) {
		Motor_SetSpeed(0);
	} else {
		uint16_t Speed = (uint16_t)(pData->Temph - Motor_ON_Tem) * 100
		               / (Motor_MAX_Tem - Motor_ON_Tem);
		Motor_SetSpeed(Speed);
	}
}
