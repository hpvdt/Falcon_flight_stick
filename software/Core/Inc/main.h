/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MTX_COL5_Pin GPIO_PIN_1
#define MTX_COL5_GPIO_Port GPIOA
#define MTX_COL4_Pin GPIO_PIN_2
#define MTX_COL4_GPIO_Port GPIOA
#define MTX_COL3_Pin GPIO_PIN_3
#define MTX_COL3_GPIO_Port GPIOA
#define MTX_COL2_Pin GPIO_PIN_4
#define MTX_COL2_GPIO_Port GPIOA
#define MTX_COL1_Pin GPIO_PIN_5
#define MTX_COL1_GPIO_Port GPIOA
#define MTX_ROW1_Pin GPIO_PIN_6
#define MTX_ROW1_GPIO_Port GPIOA
#define MTX_ROW2_Pin GPIO_PIN_7
#define MTX_ROW2_GPIO_Port GPIOA
#define MTX_ROW3_Pin GPIO_PIN_0
#define MTX_ROW3_GPIO_Port GPIOB
#define MTX_ROW4_Pin GPIO_PIN_1
#define MTX_ROW4_GPIO_Port GPIOB
#define MTX_ROW5_Pin GPIO_PIN_2
#define MTX_ROW5_GPIO_Port GPIOB
#define EEP_I2C2_SCL_Pin GPIO_PIN_10
#define EEP_I2C2_SCL_GPIO_Port GPIOB
#define EEP_I2C2_SDA_Pin GPIO_PIN_11
#define EEP_I2C2_SDA_GPIO_Port GPIOB
#define ADC_DRDY_EXTI12_Pin GPIO_PIN_12
#define ADC_DRDY_EXTI12_GPIO_Port GPIOB
#define ADC_CS_Pin GPIO_PIN_9
#define ADC_CS_GPIO_Port GPIOA
#define ADC_RST_Pin GPIO_PIN_10
#define ADC_RST_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
