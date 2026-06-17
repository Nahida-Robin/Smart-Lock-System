/** 
 * @file Buzzer.c
 * @brief 蜂鸣器模块驱动
 * @author Nahida
 * @date 2026-5-16
 */
#include "stm32f4xx.h"                 // Device header

#define BUZZER_PORT GPIOB
#define BUZZER_PIN GPIO_PIN_2

/**
 * @brief 蜂鸣器开
 * @param NULL
 * @retval NULL
 */
void Buzzer_on(void)
{
	HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
}

/**
 * @brief 蜂鸣器关
 * @param NULL
 * @retval NULL
 */
void Buzzer_off(void)
{
	HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
}
