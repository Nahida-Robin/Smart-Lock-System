#include "stm32f4xx.h"                  // Device header
#include "Delay.h"

#define IR_TR_CHANNEL TIM_CHANNEL_1

extern TIM_HandleTypeDef htim5;

//void IR_Init(void)
//{
//	HAL_TIM_PWM_Start(&htim5, IR_CHANNEL);
//}

static void IR_SendHigh(uint16_t time)
{
	HAL_TIM_PWM_Start(&htim5, IR_TR_CHANNEL);
	Delay_us(time);
}

static void IR_SendLow(uint16_t time)
{
	HAL_TIM_PWM_Stop(&htim5, IR_TR_CHANNEL);
	Delay_us(time);
}

static void IR_SendLeader(void)
{
	IR_SendHigh(9000);
	IR_SendLow(4500);
}

static void IR_SendBit(uint8_t bit)
{
	if(bit)
	{
		IR_SendHigh(560);
		IR_SendLow(1680);
	}
	else
	{
		IR_SendHigh(560);
		IR_SendLow(560);
	}
}

static void IR_SendByte(uint8_t byte)
{
	for(uint8_t i = 0; i < 8; i++)
	{
		uint8_t bit = (byte >> (7 - i)) & 0x01;
		IR_SendBit(bit);
	}
}

void IR_SendData(uint8_t addr, uint8_t data)
{
	IR_SendLeader();
	
	IR_SendByte(addr);
	IR_SendByte(~addr);
	IR_SendByte(data);
	IR_SendByte(~data);
	
	IR_SendHigh(560);
	IR_SendLow(0);
}
