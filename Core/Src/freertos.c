/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "TaskDriver.h"
#include "SensorRead.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

//队列句柄定义
osMessageQueueId_t KeyQueueHandle;
osMessageQueueId_t HwCtrlQueueHandle;

//互斥量句柄定义
osMutexId_t OLED_MutexHandle;

//任务句柄
osThreadId_t UITaskHandle;
osThreadId_t KeyScanTaskHandle;
osThreadId_t SensorTaskHandle;
osThreadId_t CommTaskHandle;
osThreadId_t HwControlTaskHandle;

//UI任务
const osThreadAttr_t UITask_attributes = {
  .name = "UITask",
  .stack_size = 512,//嵌套最深，所以栈空间最大
  .priority = (osPriority_t) osPriorityNormal,
};
//按键任务
const osThreadAttr_t KeyScanTask_attributes = {
  .name = "KeyScanTask",
  .stack_size = 256,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
//传感器任务
const osThreadAttr_t SensorTask_attributes = {
  .name = "SensorTask",
  .stack_size = 256,
  .priority = (osPriority_t) osPriorityLow,
};
//通信任务
const osThreadAttr_t CommTask_attributes = {
  .name = "CommTask",
  .stack_size = 256,
  .priority = (osPriority_t) osPriorityLow,
};
//硬件任务
const osThreadAttr_t HwControlTask_attributes = {
  .name = "HwControlTask",
  .stack_size = 256,
  .priority = (osPriority_t) osPriorityLow,
};
/* USER CODE END Variables */


/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  //创建OLED互斥量
  OLED_MutexHandle = osMutexNew(NULL);
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  //创建按键队列
  KeyQueueHandle = osMessageQueueNew(10, sizeof(uint8_t), NULL);

  //创建硬件命令队列  
  HwCtrlQueueHandle = osMessageQueueNew(10, sizeof(HwCtrlMsg_t), NULL);
  /* USER CODE END RTOS_QUEUES */



  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  //创建ui任务
  UITaskHandle = osThreadNew(StartUI_Task, NULL, &UITask_attributes);

  //创建按键任务
  KeyScanTaskHandle = osThreadNew(StartKeyScan_Task, NULL, &KeyScanTask_attributes);

  //创建传感器任务
//  SensorTaskHandle = osThreadNew(StartSensor_Task, NULL, &SensorTask_attributes);

  //创建通信任务
  CommTaskHandle = osThreadNew(StartComm_Task, NULL, &CommTask_attributes);

  //创建硬件任务
  HwControlTaskHandle = osThreadNew(StartHwControl_Task, NULL, &HwControlTask_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/* USER CODE END Header_StartDefaultTask */
/* USER CODE BEGIN Application */

/* USER CODE END Application */

