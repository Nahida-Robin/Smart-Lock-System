#ifndef __WATCHDOG_H
#define __WATCHDOG_H

#include <stdint.h>
#include "cmsis_os.h"

//看门狗检查周期 每100ms检查一次各任务有没有喂小狗
#define WDG_CHECK_PERIOD 100

typedef enum {
    WDG_UI = 0,
    WDG_KEYSCAN,
    WDG_SENSOR,
    WDG_COMM,
    WDG_HWCTRL,
    WDG_TaskNum
} WDG_Task_t;

void WDG_Init(void);
void WDG_RegisterTask(WDG_Task_t task, osThreadId_t taskHandle);
void WDG_TaskFeed(WDG_Task_t task);
uint8_t WDG_Check(void);

#endif
