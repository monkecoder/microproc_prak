/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define alarm_out_Pin GPIO_PIN_5
#define alarm_out_GPIO_Port GPIOC
#define SPI2_RST_Pin GPIO_PIN_1
#define SPI2_RST_GPIO_Port GPIOB
#define SPI2_CS_Pin GPIO_PIN_12
#define SPI2_CS_GPIO_Port GPIOB
#define alarm_trig_in_Pin GPIO_PIN_6
#define alarm_trig_in_GPIO_Port GPIOC
#define btn_in_line_0_Pin GPIO_PIN_7
#define btn_in_line_0_GPIO_Port GPIOC
#define btn_in_line_1_Pin GPIO_PIN_8
#define btn_in_line_1_GPIO_Port GPIOC
#define btn_in_line_2_Pin GPIO_PIN_9
#define btn_in_line_2_GPIO_Port GPIOC
#define btn_in_line_3_Pin GPIO_PIN_8
#define btn_in_line_3_GPIO_Port GPIOA
#define btn_out_line_0_Pin GPIO_PIN_9
#define btn_out_line_0_GPIO_Port GPIOA
#define btn_out_line_1_Pin GPIO_PIN_10
#define btn_out_line_1_GPIO_Port GPIOA
#define btn_out_line_2_Pin GPIO_PIN_11
#define btn_out_line_2_GPIO_Port GPIOA
#define btn_out_line_3_Pin GPIO_PIN_12
#define btn_out_line_3_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
