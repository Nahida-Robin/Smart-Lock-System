/** 
 * @file StepMotor.c
 * @brief 四相八拍步进电机控制
 * @author Nahida
 * @date 2026.5.24
 */

#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_hal.h"

#define MOTOR_PORT GPIOB
#define MOTOR_PIN_A GPIO_PIN_12
#define MOTOR_PIN_B GPIO_PIN_13
#define MOTOR_PIN_C GPIO_PIN_14
#define MOTOR_PIN_D GPIO_PIN_15

#define MOTOR_DELAY_MS 3
#define MOTOR_90_DEGREE 1024/8

//static uint8_t StepMotor_Busy = 0;

static uint8_t Step_Table[8][4] = {
																		{1, 0, 0, 0}, 
																		{1, 1, 0, 0}, 
																		{0, 1, 0, 0},
																		{0, 1, 1, 0}, 
																		{0, 0, 1, 0}, 
																		{0, 0, 1, 1}, 
																		{0, 0, 0, 1}, 
																		{1, 0, 0, 1}};



//void StepMotor_Delay(uint16_t ms)
//{
//	static uint32_t ticks = 0;
//	
//}	

/**
 *@brief 根据步数坐标控制四相
 *@param step:步数坐标
 *@retval NULL
 */																		
static void StepMotor_Step(uint8_t step)
{
	HAL_GPIO_WritePin(MOTOR_PORT,MOTOR_PIN_A,Step_Table[step][0] ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MOTOR_PORT,MOTOR_PIN_B,Step_Table[step][1] ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MOTOR_PORT,MOTOR_PIN_C,Step_Table[step][2] ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MOTOR_PORT,MOTOR_PIN_D,Step_Table[step][3] ? GPIO_PIN_SET : GPIO_PIN_RESET);
}	

/**
  *@brief 电机停止
  *@param NULL
  *@retval NULL
  */
static void StepMotor_Stop(void)
{
	HAL_GPIO_WritePin(MOTOR_PORT,MOTOR_PIN_A,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MOTOR_PORT,MOTOR_PIN_B,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MOTOR_PORT,MOTOR_PIN_C,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MOTOR_PORT,MOTOR_PIN_D,GPIO_PIN_RESET);	
}

/**
  *@brief 控制电机向一个方向转动一定步数
  *@param steps:八拍循环次数 即转动半步数
	*       dir:  旋转方向 1:正转 -1:反转
  *@retval NULL
  */
static void StepMotor_Forward(uint32_t steps, int8_t dir)
{
	for(uint32_t i = 0; i < steps; i++)
	{
		if(dir == 1)
		{
			for(uint8_t j = 0; j < 8; j++)
			{
				StepMotor_Step(j);
				HAL_Delay(MOTOR_DELAY_MS);//后续改成非阻塞
			}	
		}
		else if(dir == -1)
		{
			for(int8_t j = 7; j >= 0; j--)
			{
				StepMotor_Step(j);
				HAL_Delay(MOTOR_DELAY_MS);//后续改成非阻塞
			}				
		}
	}
	StepMotor_Stop();
}

/**
  *@brief 控制电机按一个方向旋转90度
	*@param dir:方向 1:正向 2:反向
  *@retval NULL
  */
void StepMotor_Forward_90_Degree(int8_t dir)
{
	StepMotor_Forward(MOTOR_90_DEGREE, dir);
}

