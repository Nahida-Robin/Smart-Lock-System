/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LS_Pin GPIO_PIN_0
#define LS_GPIO_Port GPIOC
#define IRTRAN_Pin GPIO_PIN_0
#define IRTRAN_GPIO_Port GPIOA
#define MQ2_Pin GPIO_PIN_1
#define MQ2_GPIO_Port GPIOA
#define IR_Pin GPIO_PIN_4
#define IR_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_5
#define LED2_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_6
#define LED_GPIO_Port GPIOA
#define MOTOR_Pin GPIO_PIN_7
#define MOTOR_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_0
#define SPI1_CS_GPIO_Port GPIOB
#define SERVO_Pin GPIO_PIN_1
#define SERVO_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_2
#define BUZZER_GPIO_Port GPIOB
#define StepA_Pin GPIO_PIN_12
#define StepA_GPIO_Port GPIOB
#define StepB_Pin GPIO_PIN_13
#define StepB_GPIO_Port GPIOB
#define StepC_Pin GPIO_PIN_14
#define StepC_GPIO_Port GPIOB
#define StepD_Pin GPIO_PIN_15
#define StepD_GPIO_Port GPIOB
#define SERVOD12_Pin GPIO_PIN_12
#define SERVOD12_GPIO_Port GPIOD
#define IRRECE_Pin GPIO_PIN_6
#define IRRECE_GPIO_Port GPIOC
#define MatrixKeyRow1_Pin GPIO_PIN_0
#define MatrixKeyRow1_GPIO_Port GPIOD
#define MatrixKeyRow2_Pin GPIO_PIN_1
#define MatrixKeyRow2_GPIO_Port GPIOD
#define MatrixKeyRow3_Pin GPIO_PIN_2
#define MatrixKeyRow3_GPIO_Port GPIOD
#define MatrixKeyRow4_Pin GPIO_PIN_3
#define MatrixKeyRow4_GPIO_Port GPIOD
#define MatrixKeyCul1_Pin GPIO_PIN_4
#define MatrixKeyCul1_GPIO_Port GPIOD
#define MatrixKeyCul2_Pin GPIO_PIN_5
#define MatrixKeyCul2_GPIO_Port GPIOD
#define MatrixKeyCul3_Pin GPIO_PIN_6
#define MatrixKeyCul3_GPIO_Port GPIOD
#define MatrixKeyCul4_Pin GPIO_PIN_7
#define MatrixKeyCul4_GPIO_Port GPIOD
#define OLEDSCL_Pin GPIO_PIN_8
#define OLEDSCL_GPIO_Port GPIOB
#define OLEDSDA_Pin GPIO_PIN_9
#define OLEDSDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
