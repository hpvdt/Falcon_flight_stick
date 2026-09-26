/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
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
CAN_HandleTypeDef hcan; //test

I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);
static void MX_I2C2_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
/* ADS131M03: 5-word SPI frame; cmd24 is MSB-first 24-bit command; first RX word is 24-bit (16-bit register MSB-aligned in bits 23:8). */
static uint32_t ads131m03_frame_exchange(uint32_t cmd24);
static void ads131m03_reset(void);
static uint32_t ads131m03_rreg_opcode(uint8_t reg_addr);
static uint16_t ads131m03_read_reg16(uint8_t reg_addr);
static void ads131m03_apply_spi_probe(uint32_t cpol_phase, uint32_t prescaler);
static int ads131m03_id_looks_valid(uint16_t id_reg);
static void ads131m03_run_connection_test(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define LOG_BUF_SIZE 256
static char log_buf[LOG_BUF_SIZE];

/* Send one character to USART1 for printf() / puts() */
int __io_putchar(int ch)
{
  uint8_t c = (uint8_t)(ch & 0xFF);
  HAL_UART_Transmit(&huart1, &c, 1, 100);
  return ch;
}

/* Log a string (adds \\r\\n); uses fixed buffer to avoid stack overflow */
static void log_msg(const char *msg)
{
  size_t len = strlen(msg);
  if (len + 2 > LOG_BUF_SIZE)
    len = LOG_BUF_SIZE - 2;
  memcpy(log_buf, msg, len);
  log_buf[len++] = '\r';
  log_buf[len++] = '\n';
  HAL_UART_Transmit(&huart1, (uint8_t *)log_buf, (uint16_t)len, 500);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_I2C2_Init();
  MX_SPI2_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  /* MCO on PA8: use HSE (8 MHz per Cube config), not SYSCLK. Datasheet CLKIN 2 MHz–8.2 MHz (SBAS889A). */
  HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);

  HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, GPIO_PIN_SET);
  HAL_Delay(50);

  ads131m03_run_connection_test();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /* MCO (PA8 -> ADS131M03 CLKIN) is enabled in main after GPIO_Init */
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 16;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */
  /* ADS131M03 requires CPOL=0, CPHA=1 (data latched on falling edge, output on rising) */
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  /* Slower SPI for debugging: prescaler 8 (was 2) in case of signal integrity / timing */
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, MTX_COL5_Pin|MTX_COL4_Pin|MTX_COL3_Pin|MTX_COL2_Pin
                          |MTX_COL1_Pin|ADC_CS_Pin|ADC_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RGB_PIN_GPIO_Port, RGB_PIN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : MTX_COL5_Pin MTX_COL4_Pin MTX_COL3_Pin MTX_COL2_Pin
                           MTX_COL1_Pin ADC_CS_Pin ADC_RST_Pin */
  GPIO_InitStruct.Pin = MTX_COL5_Pin|MTX_COL4_Pin|MTX_COL3_Pin|MTX_COL2_Pin
                          |MTX_COL1_Pin|ADC_CS_Pin|ADC_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : MTX_ROW1_Pin MTX_ROW2_Pin */
  GPIO_InitStruct.Pin = MTX_ROW1_Pin|MTX_ROW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : MTX_ROW3_Pin MTX_ROW4_Pin MTX_ROW5_Pin */
  GPIO_InitStruct.Pin = MTX_ROW3_Pin|MTX_ROW4_Pin|MTX_ROW5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : ADC_DRDY_EXTI12_Pin */
  GPIO_InitStruct.Pin = ADC_DRDY_EXTI12_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ADC_DRDY_EXTI12_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 (MCO / ADC_MCO - master clock output for ADS131M03) */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  /* fast slew for clock output */
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : RGB_PIN_Pin */
  GPIO_InitStruct.Pin = RGB_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RGB_PIN_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
#define ADS131M03_FRAME_WORDS  5
#define ADS131M03_FRAME_BYTES  (ADS131M03_FRAME_WORDS * 3)
#define ADS131M03_SPI_TIMEOUT 100

static uint32_t ads131m03_frame_exchange(uint32_t cmd24)
{
  uint8_t tx[ADS131M03_FRAME_BYTES];
  uint8_t rx[ADS131M03_FRAME_BYTES];
  uint32_t first_word24;

  tx[0] = (uint8_t)((cmd24 >> 16) & 0xFF);
  tx[1] = (uint8_t)((cmd24 >> 8) & 0xFF);
  tx[2] = (uint8_t)(cmd24 & 0xFF);
  memset(tx + 3, 0, ADS131M03_FRAME_BYTES - 3);

  HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi2, tx, rx, ADS131M03_FRAME_BYTES, ADS131M03_SPI_TIMEOUT);
  HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, GPIO_PIN_SET);

  first_word24 = ((uint32_t)rx[0] << 16) | ((uint32_t)rx[1] << 8) | (uint32_t)rx[2];
  return first_word24;
}

static uint32_t ads131m03_rreg_opcode(uint8_t reg_addr)
{
  return 0xA00000u | (((uint32_t)reg_addr & 0x3Fu) << 14);
}

static uint16_t ads131m03_read_reg16(uint8_t reg_addr)
{
  ads131m03_frame_exchange(ads131m03_rreg_opcode(reg_addr));
  {
    uint32_t w = ads131m03_frame_exchange(0u);
    return (uint16_t)((w >> 8) & 0xFFFFu);
  }
}

static void ads131m03_apply_spi_probe(uint32_t cpol_phase, uint32_t prescaler)
{
  /* cpol_phase: 0..3 = (CPOL,CPHA) 00,01,10,11 per STM32 naming */
  hspi2.Init.CLKPolarity = (cpol_phase & 2u) ? SPI_POLARITY_HIGH : SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = (cpol_phase & 1u) ? SPI_PHASE_2EDGE : SPI_PHASE_1EDGE;
  hspi2.Init.BaudRatePrescaler = prescaler;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
    Error_Handler();
}

static int ads131m03_id_looks_valid(uint16_t id_reg)
{
  /* ID reset 23xxh: bits 15:12 = 0010b, bits 11:8 = 0011b (CHANCNT=3); bits 7:0 vary (Table 8-14). */
  return (id_reg & 0xFF00u) == 0x2300u;
}

typedef struct
{
  uint8_t addr;
  uint16_t mask;
  uint16_t expect;
  const char *name;
} Ads131RegExpect;

/* Table 8-12 SBAS889A reset values; 0x05 not in map (reserved gap). */
static const Ads131RegExpect ads131_expects[] = {
  { 0x01u, 0xFFF8u, 0x0500u, "STATUS" },
  { 0x02u, 0xFFFFu, 0x0510u, "MODE" },
  { 0x03u, 0xFFFFu, 0x080Eu, "CLOCK" },
  { 0x04u, 0xFFFFu, 0x0000u, "GAIN" },
  { 0x06u, 0xFFFFu, 0x0600u, "CFG" },
  { 0x07u, 0xFFFFu, 0x0000u, "THRSHLD_MSB" },
  { 0x08u, 0xFFFFu, 0x0000u, "THRSHLD_LSB" },
  { 0x09u, 0xFFFFu, 0x0000u, "CH0_CFG" },
  { 0x0Au, 0xFFFFu, 0x0000u, "CH0_OCAL_MSB" },
  { 0x0Bu, 0xFFFFu, 0x0000u, "CH0_OCAL_LSB" },
  { 0x0Cu, 0xFFFFu, 0x8000u, "CH0_GCAL_MSB" },
  { 0x0Du, 0xFFFFu, 0x0000u, "CH0_GCAL_LSB" },
  { 0x0Eu, 0xFFFFu, 0x0000u, "CH1_CFG" },
  { 0x0Fu, 0xFFFFu, 0x0000u, "CH1_OCAL_MSB" },
  { 0x10u, 0xFFFFu, 0x0000u, "CH1_OCAL_LSB" },
  { 0x11u, 0xFFFFu, 0x8000u, "CH1_GCAL_MSB" },
  { 0x12u, 0xFFFFu, 0x0000u, "CH1_GCAL_LSB" },
  { 0x13u, 0xFFFFu, 0x0000u, "CH2_CFG" },
  { 0x14u, 0xFFFFu, 0x0000u, "CH2_OCAL_MSB" },
  { 0x15u, 0xFFFFu, 0x0000u, "CH2_OCAL_LSB" },
  { 0x16u, 0xFFFFu, 0x8000u, "CH2_GCAL_MSB" },
  { 0x17u, 0xFFFFu, 0x0000u, "CH2_GCAL_LSB" },
  { 0x3Eu, 0xFFFFu, 0x0000u, "REGMAP_CRC" },
};

static const uint8_t ads131_reserved_probe[] = {
  0x05u, 0x18u, 0x19u, 0x1Au, 0x1Bu, 0x1Cu, 0x1Du, 0x1Eu, 0x1Fu,
  0x20u, 0x21u, 0x22u, 0x23u, 0x24u, 0x25u, 0x26u, 0x27u, 0x28u, 0x29u, 0x2Au, 0x2Bu, 0x2Cu, 0x2Du, 0x2Eu, 0x2Fu,
  0x30u, 0x31u, 0x32u, 0x33u, 0x34u, 0x35u, 0x36u, 0x37u, 0x38u, 0x39u, 0x3Au, 0x3Bu, 0x3Cu, 0x3Du, 0x3Fu
};

static void ads131m03_run_connection_test(void)
{
  static const uint32_t prescalers[] = {
    SPI_BAUDRATEPRESCALER_256, SPI_BAUDRATEPRESCALER_128, SPI_BAUDRATEPRESCALER_64,
    SPI_BAUDRATEPRESCALER_32, SPI_BAUDRATEPRESCALER_16, SPI_BAUDRATEPRESCALER_8,
    SPI_BAUDRATEPRESCALER_4, SPI_BAUDRATEPRESCALER_2
  };
  static const unsigned int pre_divisor[] = { 256u, 128u, 64u, 32u, 16u, 8u, 4u, 2u };
  const char *mode_names[] = { "CPOL0_CPHA0", "CPOL0_CPHA1", "CPOL1_CPHA0", "CPOL1_CPHA1" };
  uint32_t best_mode = 0;
  uint32_t best_pre = SPI_BAUDRATEPRESCALER_256;
  int found = 0;
  unsigned int ui, uj;
  uint16_t id_sample;
  int pass_count = 0;
  int check_count = 0;

  printf("\r\n======== ADS131M03 connection check ========\r\n");
  printf("Pins: PA8 MCO->CLKIN, PA9 CS, PA10 SYNC/RST, PB13 SCK PB14 MISO PB15 MOSI\r\n");
  printf("UART: USART1 remap PB6 TX / PB7 RX @ 115200\r\n");
  printf("MCO source: HSE (~8 MHz). Datasheet CLKIN 2-8.2 MHz.\r\n\r\n");

  printf("Sweep SPI mode (CPOL/CPHA) x prescaler; look for ID CHANCNT=3...\r\n");
  for (ui = 0; ui < 4u; ui++)
  {
    for (uj = 0; uj < sizeof(prescalers) / sizeof(prescalers[0]); uj++)
    {
      ads131m03_apply_spi_probe(ui, prescalers[uj]);
      ads131m03_reset();
      (void)ads131m03_frame_exchange(0u);
      (void)ads131m03_frame_exchange(ads131m03_rreg_opcode(0u));
      id_sample = (uint16_t)((ads131m03_frame_exchange(0u) >> 8) & 0xFFFFu);
      printf("  mode=%s pre=/%-3u -> ID=0x%04X %s\r\n", mode_names[ui],
             pre_divisor[uj], (unsigned int)id_sample,
             ads131m03_id_looks_valid(id_sample) ? "OK" : "--");
      if (ads131m03_id_looks_valid(id_sample) && !found)
      {
        found = 1;
        best_mode = ui;
        best_pre = prescalers[uj];
      }
    }
  }

  if (!found)
  {
    printf("\r\nSUMMARY: No valid ID on any SPI mode/prescaler. Check wiring, power, CLKIN on PA8, CS on PA9.\r\n");
    /* Restore datasheet-recommended Mode 1 for any manual probing */
    ads131m03_apply_spi_probe(1u, SPI_BAUDRATEPRESCALER_8);
    return;
  }

  printf("\r\nUsing first hit: %s, prescaler enc=0x%lX (re-init for register sweep).\r\n",
         mode_names[best_mode], (unsigned long)best_pre);
  ads131m03_apply_spi_probe(best_mode, best_pre);
  ads131m03_reset();
  (void)ads131m03_frame_exchange(0u);

  printf("\r\n--- Register reads vs Table 8-12 reset (mask where noted) ---\r\n");
  id_sample = ads131m03_read_reg16(0u);
  check_count++;
  if (ads131m03_id_looks_valid(id_sample))
  {
    pass_count++;
    printf("  [OK] 0x00 ID           read 0x%04X (expect 0x23xx)\r\n", (unsigned int)id_sample);
  }
  else
  {
    printf("  [BAD] 0x00 ID           read 0x%04X\r\n", (unsigned int)id_sample);
  }

  for (ui = 0; ui < sizeof(ads131_expects) / sizeof(ads131_expects[0]); ui++)
  {
    const Ads131RegExpect *e = &ads131_expects[ui];
    uint16_t v = ads131m03_read_reg16(e->addr);
    check_count++;
    if ((v & e->mask) == (e->expect & e->mask))
    {
      pass_count++;
      printf("  [OK] 0x%02X %-14s read 0x%04X\r\n", (unsigned int)e->addr, e->name, (unsigned int)v);
    }
    else
    {
      printf("  [XX] 0x%02X %-14s read 0x%04X (want 0x%04X & 0x%04X)\r\n",
             (unsigned int)e->addr, e->name, (unsigned int)v, (unsigned int)e->expect, (unsigned int)e->mask);
    }
  }

  printf("\r\n--- Reserved / unlisted addresses (read only, no pass/fail) ---\r\n");
  for (ui = 0; ui < sizeof(ads131_reserved_probe) / sizeof(ads131_reserved_probe[0]); ui++)
  {
    uint8_t a = ads131_reserved_probe[ui];
    uint16_t v = ads131m03_read_reg16(a);
    printf("  0x%02X -> 0x%04X\r\n", (unsigned int)a, (unsigned int)v);
  }

  printf("\r\n======== SUMMARY: %d / %d checks matched expected reset ========\r\n", pass_count, check_count);
  if (pass_count == check_count)
    printf("ADS131M03 SPI link OK.\r\n");
  else
    printf("Some mismatches (DRDY on STATUS, prior writes, or noise). Re-run after power cycle.\r\n");
}

static void ads131m03_reset(void)
{
  /* SYNC/RESET low > 2048 CLKIN cycles (~0.25 ms at 8 MHz); use 1 ms then release */
  HAL_GPIO_WritePin(ADC_RST_GPIO_Port, ADC_RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(ADC_RST_GPIO_Port, ADC_RST_Pin, GPIO_PIN_SET);
  HAL_Delay(50);  /* allow internal clocks to stabilize before first SPI access */
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
