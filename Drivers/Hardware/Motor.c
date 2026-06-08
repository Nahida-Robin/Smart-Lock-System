/**
 * @file Motor.c
 * @brief 风扇驱动->使用PWM控制风扇转速->PA7 (TIM3_CH2)
 * @author Nahida
 * @date 2026-5-16
 */
#include "stm32f4xx.h"

#define MOTOR_CHANNEL TIM_CHANNEL_2

extern TIM_HandleTypeDef htim3;

/**
  *@brief 风扇控制初始化
  *@param NULL
  *@retval NULL
  */
void Motor_Init(void)
{
	HAL_TIM_PWM_Start(&htim3, MOTOR_CHANNEL);
}

/**
 * @brief 风扇根据速度映射占空比
 * @param Speed：设置的速度
 * @retval NULL
 */
void Motor_SetSpeed(uint16_t speed)
{
    if (speed > 100) speed = 100;
		__HAL_TIM_SetCompare(&htim3, MOTOR_CHANNEL, speed * 10);
}
