/**
 * @file PWM.c
 * @brief PWMLED配置
 * @author Nahida
 * @date 2026-5-16
 */
#include "stm32f4xx.h"                  // Device header

#define PWMLED_CHANNEL TIM_CHANNEL_1

extern TIM_HandleTypeDef htim3;

/**
 * @brief PWMLED初始化
 * @param NULL
 * @retval NULL
 */
void PWMLED_Init(void)
{
	HAL_TIM_PWM_Start(&htim3, PWMLED_CHANNEL);
}

/**
 * @brief PWMLED设置亮度
 * @param bright 要设置的亮度
 * @retval NULL
 */
void PWMLED_SetBright(uint16_t bright)
{
		if(bright > 100) bright = 100;
    __HAL_TIM_SetCompare(&htim3, PWMLED_CHANNEL, bright * 10);
}
