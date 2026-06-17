/** 
 * @file Servo.c
 * @brief 舵机控制
 * @author Nahida
 * @date 2026.5.30
 */
#include "stm32f4xx.h"                  // Device header

#define SERVO_CHANNEL TIM_CHANNEL_1

extern TIM_HandleTypeDef htim4;

/**
  *@brief 舵机控制初始化 
  *@param NULL
  *@retval NULL
  */
void Servo_Init(void)
{
	HAL_TIM_PWM_Start(&htim4, SERVO_CHANNEL);
}

/**
  *@brief 舵机根据角度映射占空比
  *@param angle 要设置的角度
  *@retval NULL
  */
void Servo_SetAngle(uint8_t angle)
{
	if(angle > 180) angle = 180;
	uint16_t Compare = angle * 2000 / 180 + 500;
	__HAL_TIM_SetCompare(&htim4, SERVO_CHANNEL, Compare);
}

