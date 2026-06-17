/** 
 * @file IRSensor.h
 * @brief 对射式红外传感器 - PB13 (GPIO输入)
 *        输出信号：遮断时低电平 -> 检测到人经过
 * @author Nahida
 * @date 2026-5-16
 */

#ifndef __IRSENSOR_H
#define __IRSENSOR_H

uint8_t IRSensor_Detected(void);  // 返回1检测到人经过，0未检测到

#endif
