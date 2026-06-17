/**
 * @file dht11.c
 * @brief dht11温湿度模块驱动
 * @author Nahida
 * @date 2026-5-16
 */

#include "stm32f4xx.h"                  // Device header
#include "Delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

#define DHT11_PORT 		GPIOA
#define DHT11_PIN		GPIO_PIN_0
char Data[5];

/**
 * @brief DHT输入模式
 * @param NULL
 * @retval NULL
 */
static void DHT_GPIO_INPUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin=DHT11_PIN;
	GPIO_InitStructure.Mode=GPIO_MODE_INPUT;
	GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DHT11_PORT,&GPIO_InitStructure);
}

/**
 * @brief DHT输出模式
 * @param NULL
 * @retval NULL
 */
static void DHT_GPIO_OUTPUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin=DHT11_PIN;
	GPIO_InitStructure.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DHT11_PORT,&GPIO_InitStructure);
}
/**
 * @brief DHT起始信号
 * @param NULL
 * @retval NULL
 */
static void DHT11_Start(void)
{
	DHT_GPIO_OUTPUT();
	HAL_GPIO_WritePin(DHT11_PORT,DHT11_PIN,GPIO_PIN_SET);
	HAL_GPIO_WritePin(DHT11_PORT,DHT11_PIN,GPIO_PIN_RESET);
	Delay_ms(20);  //至少延时18ms
	HAL_GPIO_WritePin(DHT11_PORT,DHT11_PIN,GPIO_PIN_SET);

	DHT_GPIO_INPUT();

	//等待DHT响应
	while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN));
	while(!HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN));
	while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN));
}

/**
 * @brief DHT读一个字节
 * @param NULL
 * @retval ReadData
 */
static uint8_t DHT_Read_Byte(void)
{
    uint8_t i;
    uint8_t ReadData = 0;
	uint8_t temp;

    for(i=0;i<8;i++){
        while(!HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN));
        Delay_us(50);
        if(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == 1){
            temp = 1;
            while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN));
        }else{
            temp = 0;
        }
        ReadData = ReadData << 1;
        ReadData |= temp;
    }
    return ReadData;
}
/**
 * @brief DHT读取温湿度数据，临界区保护防止任务切换破坏时序
 * @param temph：温度整数位
 * @param templ：温度小数位
 * @param humih：湿度整数位
 * @param humil：湿度小数位
 * @retval 1：成功 0：失败
 */
uint8_t DHT_Read(uint8_t *temph, uint8_t *templ, uint8_t *humih, uint8_t *humil)
{
	uint8_t i;
	uint8_t ret = 0;

	//关调度器，临界保护
	taskENTER_CRITICAL();

	DHT11_Start();
	DHT_GPIO_INPUT();

	for(i=0;i<5;i++){
		Data[i] = DHT_Read_Byte();
	}

	taskEXIT_CRITICAL();

    if((Data[0]+Data[1]+Data[2]+Data[3])==Data[4])
    {
		*temph = Data[2];
		*templ = Data[3];
		*humih = Data[0];
		*humil = Data[1];
		ret = 1;
    }
	return ret;
}
