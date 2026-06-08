/** 
 * @file Matrix.c
 * @brief 矩阵键盘消抖驱动
 * @author Nahida
 * @date 2026.5.22
 */

#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_hal.h"

#define Matrix_PORT GPIOD
#define Matrix0_PIN GPIO_PIN_0
#define Matrix1_PIN GPIO_PIN_1
#define Matrix2_PIN GPIO_PIN_2
#define Matrix3_PIN GPIO_PIN_3
#define Matrix4_PIN GPIO_PIN_4
#define Matrix5_PIN GPIO_PIN_5
#define Matrix6_PIN GPIO_PIN_6
#define Matrix7_PIN GPIO_PIN_7

#define Debounce_Time 20

/**
  *@brief 矩阵键盘列全高
  *@param NULL
  *@retval NULL
  */
static void Matrix_Set()
{
	HAL_GPIO_WritePin(Matrix_PORT, Matrix0_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(Matrix_PORT, Matrix1_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(Matrix_PORT, Matrix2_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(Matrix_PORT, Matrix3_PIN, GPIO_PIN_SET);
}

/**
  *@brief 矩阵键盘扫描行
  *@param NULL
  *@retval 扫描得到的键值
  */
static unsigned char Matrix_Scan_Raw()
{
	unsigned char KeyNum = 0;
	
	Matrix_Set();
	HAL_GPIO_WritePin(Matrix_PORT, Matrix0_PIN, GPIO_PIN_RESET);
	if(HAL_GPIO_ReadPin(Matrix_PORT,Matrix7_PIN)==0){KeyNum = 4;}
	if(HAL_GPIO_ReadPin(Matrix_PORT,Matrix6_PIN)==0){KeyNum = 8;}
	if(HAL_GPIO_ReadPin(Matrix_PORT,Matrix5_PIN)==0){KeyNum = 12;}
	if(HAL_GPIO_ReadPin(Matrix_PORT,Matrix4_PIN)==0){KeyNum = 16;}
	
	Matrix_Set();
	HAL_GPIO_WritePin(Matrix_PORT, Matrix1_PIN, GPIO_PIN_RESET);
	if(HAL_GPIO_ReadPin(Matrix_PORT,Matrix7_PIN)==0){KeyNum = 3;}
	if(HAL_GPIO_ReadPin(Matrix_PORT,Matrix6_PIN)==0){KeyNum = 7;}
	if(HAL_GPIO_ReadPin(Matrix_PORT,Matrix5_PIN)==0){KeyNum = 11;}
	if(HAL_GPIO_ReadPin(Matrix_PORT,Matrix4_PIN)==0){KeyNum = 15;}

	Matrix_Set();
	HAL_GPIO_WritePin(Matrix_PORT, Matrix2_PIN, GPIO_PIN_RESET);
	if(HAL_GPIO_ReadPin(Matrix_PORT,Matrix7_PIN)==0){KeyNum = 2;}
	if(HAL_GPIO_ReadPin(Matrix_PORT,Matrix6_PIN)==0){KeyNum = 6;}
	if(HAL_GPIO_ReadPin(Matrix_PORT,Matrix5_PIN)==0){KeyNum = 10;}
	if(HAL_GPIO_ReadPin(Matrix_PORT,Matrix4_PIN)==0){KeyNum = 14;}

	Matrix_Set();
	HAL_GPIO_WritePin(Matrix_PORT, Matrix3_PIN, GPIO_PIN_RESET);
	if(HAL_GPIO_ReadPin(Matrix_PORT,Matrix7_PIN)==0){KeyNum = 1;}
	if(HAL_GPIO_ReadPin(Matrix_PORT,Matrix6_PIN)==0){KeyNum = 5;}
	if(HAL_GPIO_ReadPin(Matrix_PORT,Matrix5_PIN)==0){KeyNum = 9;}
	if(HAL_GPIO_ReadPin(Matrix_PORT,Matrix4_PIN)==0){KeyNum = 13;}
	
	return KeyNum;
}

/**
  *@brief 矩阵键盘消抖取值
  *@param NULL
  *@retval 消抖后返回的键值
  */
uint8_t Matrix_GetNum()
{
	static uint8_t Last_Raw = 0;
	static uint32_t Ticks = 0;
	static uint8_t Last_Send=0;
	
	uint8_t Raw = Matrix_Scan_Raw();
	uint32_t Now = HAL_GetTick();
	
	//如果行扫描值变化
	if(Raw != Last_Raw)
	{
		Last_Raw = Raw;
		//重新开始计时
		Ticks = Now;
		return 0;
	}
	
	//如果计时超过消抖时间
	if(Now - Ticks >= Debounce_Time)
	{
		//如果是有效键值且未上报过
		if(Raw!=0&&Last_Send==0)
		{
			//标记为已上报
			Last_Send = Raw;
			Last_Raw = Raw;
			return Raw;
		}
		
		if(Raw==0)
		{
			Last_Send = 0;
		}
		
		return 0;
	}
	
	return 0;
}

