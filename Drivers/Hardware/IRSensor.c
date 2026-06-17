/** 
 * @file IRSensor.c
 * @brief 对射式红外传感器 - PB13 (GPIO输入)
 *        输出信号：遮断时低电平 -> 检测到人经过
 * @author Nahida
 * @date 2026-5-16
 */
#include "stm32f4xx.h"

#define IR_PORT GPIOA
#define IR_PIN  GPIO_PIN_4

/**
 * @brief 红外传感器检测
 * @param NULL
 * @retval 1：有人经过 2：未检测到
 */
uint8_t IRSensor_Detected(void)
{
    return (HAL_GPIO_ReadPin(IR_PORT, IR_PIN) == GPIO_PIN_RESET) ? 1 : 0;
}
