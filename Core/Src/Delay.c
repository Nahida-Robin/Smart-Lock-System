/** 
 * @file Delay.c
 * @brief DWT外设下的延时
 * @author Nahida
 * @date 2026.5.24
 */

#include "stm32f4xx.h"                  // Device header

/**
  *@brief DWT外设初始化
  *@param NULL
  *@retval NULL
  */
void DWT_Delay_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;  // DWT外设使能
    DWT->CYCCNT = 0;                                 // 计数器清零
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;             // CYCCNT使能
}

/**
  *@brief us级延时
  *@param us 延时的微妙数
  *@retval NULL
  */
void Delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000);

    while ((DWT->CYCCNT - start) < ticks);
}

/**
  *@brief ms级延时
  *@param ms 延时的毫秒数
  *@retval NULL
  */
void Delay_ms(uint32_t ms)
{
    while (ms--)
    {
        Delay_us(1000);
    }
}
