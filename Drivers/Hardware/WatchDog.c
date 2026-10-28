/**
 * @file WatchDog.c
 * @brief IWDG 独立看门狗 分配给各任务小狗 检查小狗情况来喂狗
 * @author Nahida
 * @date 2026.10.28
 */

#include "WatchDog.h"
#include "main.h"

//每个任务单独喂小狗的间隔最大值
#define WDG_TimeOut_Ms 800

extern IWDG_HandleTypeDef hiwdg;

static osThreadId_t WDG_Task[WDG_TaskNum];//小狗列表
static volatile uint32_t WDG_FeedTime[WDG_TaskNum];//喂小狗的时间
static uint8_t WDG_FeedInit = 0;

/**
  *@brief 看门狗模块初始化
  *@param NULL
  *@retval NULL
  */
void WDG_Init(void)
{
    for(uint8_t i = 0; i < WDG_TaskNum; i++)
    {
        WDG_Task[i] = NULL;
        WDG_FeedTime[i] = 0;
    }
    WDG_FeedInit = 0;
}

/**
  *@brief 登记一个需要被检查的任务
  *@param id 任务编号
  *@param thread 任务句柄
  *@retval NULL
  */
void WDG_RegisterTask(WDG_Task_t task, osThreadId_t taskHandle)
{
    if(task < WDG_TaskNum)
    {
        WDG_Task[task] = taskHandle;
    }
}

/**
  *@brief 任务单独喂小狗
  *@param id 任务编号
  *@retval NULL
  */
void WDG_TaskFeed(WDG_Task_t task)
{
    if(task < WDG_TaskNum)
    {
        WDG_FeedTime[task] = osKernelGetTickCount();
    }
}

/**
  *@brief 检查所有任务是否按时喂小狗，全部正常才喂狗
  *@param NULL
  *@retval 1：已喂小狗 0：有任务超时，停止喂狗等待硬件复位
  */
uint8_t WDG_Check(void)
{
    uint32_t currentTime = osKernelGetTickCount();

    if(WDG_FeedInit == 0)
    {
        for(uint8_t i = 0; i < WDG_TaskNum; i++)
        {
            WDG_FeedTime[i] = currentTime;
        }
        WDG_FeedInit = 1;
        HAL_IWDG_Refresh(&hiwdg);
        return 1;
    }

    for(uint8_t i = 0; i < WDG_TaskNum; i++)
    {
        if(WDG_Task[i] != NULL)
        {
            if((currentTime - WDG_FeedTime[i]) > WDG_TimeOut_Ms)
            {
                return 0;
            }
        }
    }

    HAL_IWDG_Refresh(&hiwdg);
    return 1;
}
