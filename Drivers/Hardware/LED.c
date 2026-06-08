/** 
 * @file LED.c
 * @brief LED控制
 * @author Nahida
 * @date 2026-5-16
 */
#include "stm32f4xx.h"

// 报警LED - PA5
#define LED_PORT GPIOA
#define LED_PIN  GPIO_PIN_5

/**
 * @brief LED开
 * @param NULL
 * @retval NULL
 */
void LED_On(void)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);  // 高电平亮
}

/**
 * @brief LED关
 * @param NULL
 * @retval NULL
 */
void LED_Off(void)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);   // 低电平灭
}

/**
 * @brief LED翻转
 * @param NULL
 * @retval NULL
 */
void LED_Toggle(void)
{
    HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
}
