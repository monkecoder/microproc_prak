/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "KEYPAD.h"
#include "icmp.h"
#include "enc28j60.h"
#include "ssd1306.h"
#include <stdio.h>
#include "ssd1306_fonts.h"
#include <string.h>
#include "ethernet.h"
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
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim9;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
int _write(int fd, char* ptr, int len) { // printf to uart
    HAL_UART_Transmit(&huart2, (uint8_t *) ptr, len, 300);
    return len;
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM5_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM9_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
const uint8_t eth_data[] = "alarm activated! someone is intruding the house!";
uint8_t ipAddr[IP_ADDRESS_BYTES_NUM] = {192, 168, 0, 2};
ENC28J60_Frame frame;
ETH_Frame eth_frame = {
		{0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
		{0x00, 0x17, 0x22, 0xed, 0xa5, 0x01},
		0x2220,
		"alarm activated! someone is intruding the house!"
};

uint8_t send_eth_alarm_frame = 0;
uint8_t func_state = 0; // function chosen in main
uint8_t button_state[16] = {0, }; // button state array for keypad
uint8_t true_password[5] = {0, }; // true values array for keypad input (alarm password)
uint8_t input_password[5] = {0, }; // entered values array for keypad input
char char_input_password[5] = {'0', }; // entered values CHAR array for keypad input
uint8_t sec_cnt_activate = 0; // activate time counter
uint8_t sec_cnt_x = 0, sec_cnt_y = 0; // time output positions for LCD
uint16_t sec_cnt = 0; // time counter
uint8_t y = 0; // y position for LCD
uint8_t led_activate = 0; // activate led blinking
uint8_t sec_cnt_bias = 0; // bias for time counter indicate
const uint8_t default_input[5] = {10, }; // default values array for keypad input


void write_string_lcd(uint8_t str_x, uint8_t str_y, char *str);
uint8_t raw_btn_code();
uint8_t conv_btn_code();
void indicate_num(uint8_t num);
void password_input();
void set_password();
void alarm_activate();
void alarm_off();
void alarm_on();


void write_string_lcd(uint8_t str_x, uint8_t str_y, char *str) { // set cursor and write a string
	ssd1306_SetCursor(str_x, str_y);
	ssd1306_WriteString(str, Font_11x18, White);
}

uint8_t raw_btn_code() { // raw code for pressed keypad button
	uint8_t i_saved = 16;
	for(int i = 0; i < 16; i++) {
		if(button_state[i]) {
			HAL_Delay(50);
			if(button_state[i]) {
				i_saved = i;
				HAL_Delay(100);
			}
		}
	}
	return i_saved;
}

uint8_t conv_btn_code() { // converted code for pressed keypad button
	switch(raw_btn_code()) {
		case 0:  return 1;  // number 1
		case 1:  return 2;  // number 2
		case 2:  return 3;  // number 3
		case 3:  return 12; // function A
		case 4:  return 4;  // number 4
		case 5:  return 5;  // number 5
		case 6:  return 6;  // number 6
		case 7:  return 13; // function B
		case 8:  return 7;  // number 7
		case 9:  return 8;  // number 8
		case 10: return 9;  // number 9
		case 11: return 14; // function C
		case 12: return 11; // OK (input validation)
		case 13: return 0;  // number 0
		case 14: return 10; // <- (delete one input character)
		case 15: return 15; // function D
		default: return 16; // no key pressed
	}
}

void indicate_num(uint8_t num) { // number indicate for lcd (used for seconds displaying)
	char char_num[3] = {0, };
	sprintf(char_num, "%d", num);
	if(num < 10) {
		char_num[1] = char_num[0];
		char_num[0] = '0';
	}
	ssd1306_SetCursor(sec_cnt_x, sec_cnt_y);
	ssd1306_WriteString(char_num, Font_11x18, White);
	ssd1306_UpdateScreen();
}

void password_input() { // scan input password from keypad
	memcpy(input_password, default_input, 5); // erasing input
	uint8_t btn_code = 16;
	uint8_t input_cursor = 0;

	ssd1306_SetCursor(2, y);
	ssd1306_WriteString("-----", Font_11x18, White);
	ssd1306_UpdateScreen();

	while(!(input_cursor == 5 && btn_code == 11)) {
		btn_code = conv_btn_code();

		if(input_cursor < 5 && btn_code < 10) {
			input_password[input_cursor] = btn_code;
			char_input_password[input_cursor] = btn_code + '0';
			ssd1306_SetCursor(2 + input_cursor * 11, y);
			ssd1306_WriteChar('*', Font_11x18, White);
			ssd1306_UpdateScreen();
			input_cursor++;
		} else
		if(input_cursor > 0 && btn_code == 10) {
			input_cursor--;
			input_password[input_cursor] = 10;
			ssd1306_SetCursor(2 + input_cursor * 11, y);
			ssd1306_WriteChar('-', Font_11x18, White);
			ssd1306_UpdateScreen();
		} else
		if(sec_cnt > sec_cnt_bias && sec_cnt_activate) return;
	}
}

void set_password() { // function 0 - set/change password
	printf("setting new password\r\n");

	ssd1306_Fill(Black);
	y = 0;
	write_string_lcd(2, y, "Enter your");
	y += 18;
	write_string_lcd(2, y, "p-word(0-9)");
	y += 18;
	write_string_lcd(68, y, "OK/<-");

	password_input();
	memcpy(true_password, input_password, 5); // setting password

	y = 0;
	ssd1306_Fill(Black);
	write_string_lcd(2, y, "That's your");
	y += 18;
	write_string_lcd(2, y, "p-word");
	y += 18;
	write_string_lcd(2, y, char_input_password);
	write_string_lcd(68, y, "OK");
	ssd1306_UpdateScreen();
	printf("password is %s\r\n", char_input_password);

	while(conv_btn_code() != 11) {
		ssd1306_SetCursor(90, y);
		ssd1306_WriteString("---", Font_11x18, White);
		ssd1306_UpdateScreen();
		HAL_Delay(100);
		for(uint8_t i = 0; i < 3; i++) {
			ssd1306_SetCursor(90 + i * 11, y);
			ssd1306_WriteChar('>', Font_11x18, White);
			ssd1306_UpdateScreen();
			HAL_Delay(100);
		}
	}

	func_state = 1;
}

void alarm_off() { // function 1 - alarm is turned off
	printf("alarm is off\r\n");

	ssd1306_Fill(Black);
	y = 0;
	write_string_lcd(2, y, "ALARM OFF");
	y += 18;
	write_string_lcd(2, y, "alarm on:A");
	y += 18;
	write_string_lcd(2, y, "password:B");
	ssd1306_UpdateScreen();
	while(conv_btn_code() != 12) {
		if(conv_btn_code() == 13) {
			func_state = 0;
			return;
		}
	}
	ssd1306_Fill(Black);
	y = 0;
	write_string_lcd(2, y, "alarm on");
	y += 18;
	write_string_lcd(2, y, "after:");
	y += 18;
	write_string_lcd(2, y, "skip:OK");
	y -= 18;

	sec_cnt = 0;
	sec_cnt_activate = 1;
	sec_cnt_x = 68;
	sec_cnt_y = y;
	indicate_num(sec_cnt_bias);

	while(sec_cnt <= sec_cnt_bias && conv_btn_code() != 11) {}

	sec_cnt_activate = 0;
	func_state = 2;
}

void alarm_on() { // function 2 - alarm is turned on
	printf("alarm is on\r\n");
	ssd1306_Fill(Black);
	y = 0;
	write_string_lcd(2, y, "ALARM ON");
	y += 18;
	write_string_lcd(2, y, "alarm off:");
	y += 18;
	write_string_lcd(2, y, "any key");
	ssd1306_UpdateScreen();

	while(conv_btn_code() == 16) {
		if (!HAL_GPIO_ReadPin(alarm_trig_in_GPIO_Port, alarm_trig_in_Pin)) break;
	}

	ssd1306_Fill(Black);
	y = 0;
	write_string_lcd(2, y, "Enter");
	y += 18;
	write_string_lcd(2, y, "p-word(0-9)");
	y -= 18;
	write_string_lcd(68, y, "t:");

	sec_cnt = 0;
	sec_cnt_activate = 1;
	sec_cnt_x = 90;
	sec_cnt_y = y;
	indicate_num(sec_cnt_bias);

	y += 36;
	write_string_lcd(68, y, "OK/<-");

	while(sec_cnt <= sec_cnt_bias) {
		password_input();
		if(memcmp(input_password, true_password, 5) == 0) { // comparing input and password
			sec_cnt_activate = 0;
			func_state = 1;
			return;
		}
	}

	sec_cnt_activate = 0;
	func_state = 3;
}

void alarm_activate() { // function 3 - alarm is activated
	HAL_GPIO_WritePin(alarm_out_GPIO_Port, alarm_out_Pin, GPIO_PIN_SET);
	send_eth_alarm_frame = 1;
	printf("!!!ALARM ACTIVATED!!!\r\n");
	led_activate = 1;
	ssd1306_Fill(Black);
	y = 0;
	write_string_lcd(2, y, "---ALARM---");
	y += 18;
	write_string_lcd(2, y, "-ACTIVATED-");
	y += 18;
	write_string_lcd(40, y, "t:");
	ssd1306_UpdateScreen();

	sec_cnt_bias = 60;
	sec_cnt_x = 62;
	sec_cnt_y = y;
	indicate_num(sec_cnt_bias);
	sec_cnt = 0;
	sec_cnt_activate = 1;

	while(sec_cnt <= sec_cnt_bias);

	sec_cnt_activate = 0;
	led_activate = 0;
	sec_cnt_bias = 20;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(alarm_out_GPIO_Port, alarm_out_Pin, GPIO_PIN_RESET);
	send_eth_alarm_frame = 0;
	func_state = 2;
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
  MX_USART2_UART_Init();
  MX_SPI2_Init();
  MX_TIM5_Init();
  MX_I2C1_Init();
  MX_TIM9_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start_IT(&htim5);
  HAL_TIM_Base_Start_IT(&htim9);
  KEYPAD_Init(0, button_state);
  ENC28J60_Init();
  ssd1306_Init();
  ssd1306_Reset();

  printf("alarm system started\r\n");
  printf("ethernet frame length: %d\r\n", sizeof(eth_frame));
  ssd1306_Fill(Black);
  y = 10;
  write_string_lcd(11, y, "Welcome");
  write_string_lcd(93, y, "to");
  y = 30;
  write_string_lcd(0, y, "alarm");
  write_string_lcd(60, y, "system");
  ssd1306_UpdateScreen();
  while(conv_btn_code() == 16) {}
  sec_cnt_bias = 20;

//  memcpy(icmp_req.data, main, 48);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while(1)
  {
	  switch(func_state) {
	  	  case 0: set_password(); break;
	  	  case 1: alarm_off(); break;
	  	  case 2: alarm_on(); break;
	  	  case 3: alarm_activate(); break;
	  }
  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 15;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 100;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 10000;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM9_Init(void)
{

  /* USER CODE BEGIN TIM9_Init 0 */

  /* USER CODE END TIM9_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  /* USER CODE BEGIN TIM9_Init 1 */

  /* USER CODE END TIM9_Init 1 */
  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 10000;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 10000;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM9_Init 2 */

  /* USER CODE END TIM9_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|btn_out_line_0_Pin|btn_out_line_1_Pin|btn_out_line_2_Pin
                          |btn_out_line_3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(alarm_out_GPIO_Port, alarm_out_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SPI2_RST_Pin|SPI2_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin btn_out_line_0_Pin btn_out_line_1_Pin btn_out_line_2_Pin
                           btn_out_line_3_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|btn_out_line_0_Pin|btn_out_line_1_Pin|btn_out_line_2_Pin
                          |btn_out_line_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : alarm_out_Pin */
  GPIO_InitStruct.Pin = alarm_out_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(alarm_out_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI2_RST_Pin SPI2_CS_Pin */
  GPIO_InitStruct.Pin = SPI2_RST_Pin|SPI2_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : alarm_trig_in_Pin */
  GPIO_InitStruct.Pin = alarm_trig_in_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(alarm_trig_in_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : btn_in_line_0_Pin btn_in_line_1_Pin btn_in_line_2_Pin */
  GPIO_InitStruct.Pin = btn_in_line_0_Pin|btn_in_line_1_Pin|btn_in_line_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : btn_in_line_3_Pin */
  GPIO_InitStruct.Pin = btn_in_line_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(btn_in_line_3_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
	if(htim->Instance == TIM5) {
		KEYPAD_Scan(0);
		ETH_Process(&frame);
	}
	if(htim->Instance == TIM9) {
		if(led_activate) {
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		}
		if(sec_cnt_activate) {
			indicate_num(sec_cnt_bias - sec_cnt);
			sec_cnt++;
		}
		if(send_eth_alarm_frame) {
			ENC28J60_TransmitFrame((uint8_t*)&eth_frame, sizeof(ETH_Frame) + sizeof(eth_data));
		}
	}
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
