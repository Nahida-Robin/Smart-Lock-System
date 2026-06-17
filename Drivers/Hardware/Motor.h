/** 
 * @file Motor.h
 * @brief 风扇驱动->使用PWM控制风扇转速->PA7 (TIM3_CH2)
 * @author Nahida
 * @date 2026-5-16
 */
#ifndef __MOTOR_H
#define __MOTOR_H

void Motor_Init(void);
void Motor_SetSpeed(uint16_t speed);

#endif
