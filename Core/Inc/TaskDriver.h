#ifndef __TASKDRIVER_H
#define __TASKDRIVER_H

#include "cmsis_os.h"

void TaskDriver_Init(void);

typedef enum {
    HW_CMD_SET_BRIGHTNESS = 1,
} HwCtrlCmd_t;

/**
  *@brief 硬件控制消息结构体
  *@param NULL
  *@retval NULL
  */
typedef struct {
    HwCtrlCmd_t cmd;
    uint8_t data;
} HwCtrlMsg_t;

extern osMessageQueueId_t KeyQueueHandle;
extern osMessageQueueId_t HwCtrlQueueHandle;

extern osMutexId_t OLED_MutexHandle;

void StartUI_Task(void *argument);
void StartKeyScan_Task(void *argument);
void StartSensor_Task(void *argument);
void StartComm_Task(void *argument);
void StartHwControl_Task(void *argument);

#endif
