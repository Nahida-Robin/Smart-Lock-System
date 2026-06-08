/** 
 * @file TickTime.c
 * @brief 根据tick获取当前时间
 * @author Nahida
 * @date 2026.5.23
 */

#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_hal.h"

static uint32_t Raw_Time = 0;
static uint32_t Total_Sec = 0;

/**
  *@brief 时间初始化，用于记录初次运行时的tick
  *@param NULL
  *@retval NULL
  */
void TickTime_Init(void)
{
	Raw_Time = HAL_GetTick();
}

/**
  *@brief 获取当前时间
  *@param hour 时
	*	      min  分
	*       sec  秒
  *@retval NULL
  */
void Tick_GetTime(uint8_t *hour, uint8_t *min, uint8_t *sec)
{
	Total_Sec = (HAL_GetTick() - Raw_Time) / 1000;
	*sec = Total_Sec % 60;
	*min = Total_Sec / 60 % 60;
	*hour = Total_Sec /3600;
}
