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
#include <string.h>
#include <ctype.h>

#include "u8g2.h"
#include "u8x8.h"
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
 RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
static u8g2_t u8g2;
static char inTxt[] = "ABCDEabcde";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);
uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
void draw(u8g2_t *u8g2);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
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
  MX_RTC_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(VFD_RESET_GPIO_Port, VFD_RESET_Pin, GPIO_PIN_RESET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(VFD_RESET_GPIO_Port, VFD_RESET_Pin, GPIO_PIN_SET);
  HAL_Delay(1);

  // And turn the filament on
  HAL_GPIO_WritePin(VFD_FILEN_GPIO_Port, VFD_FILEN_Pin, GPIO_PIN_SET);

  u8g2_Setup_futaba_vfd_gp1287ai_256x50_f(&u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi, u8g2_gpio_and_delay_stm32);
  u8g2_InitDisplay(&u8g2);
  u8g2_SetPowerSave(&u8g2, 0);
  u8g2_SetContrast(&u8g2, 10);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint_fast8_t contrast = 0;
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    u8g2_FirstPage(&u8g2);
    do {
      draw(&u8g2);
    } while (u8g2_NextPage(&u8g2));

    uint8_t inChar;
    if(HAL_UART_Receive(&huart1, &inChar, sizeof(inChar), 0) == HAL_TIMEOUT) {
      // HAL_UART_Transmit(&huart1, (uint8_t*)"None\n", 5, 100);
    } else if(isgraph(inChar) || inChar == ' ') {
      int_fast8_t sLen = strlen(inTxt);
      if(sLen == 10) {
    	  sLen = 0;
    	  // HAL_UART_Transmit(&huart1, (uint8_t*)"Boop\n", 5, 100);
      }
      inTxt[sLen] = inChar;
      inTxt[sLen + 1] = '\0';
      // HAL_UART_Transmit(&huart1, &inChar, sizeof(inChar), 100);
      // HAL_UART_Transmit(&huart1, (uint8_t*)"\n", 1, 100);
      // HAL_UART_Transmit(&huart1, inTxt, strlen(inTxt), 100);
      // HAL_UART_Transmit(&huart1, (uint8_t*)"\n", 1, 100);
    }

    const uint_fast8_t CONTRASTOFFSET = 0x01;
    const uint_fast8_t CONTRASTMAX = 0x20;

    if(!(contrast & CONTRASTMAX)) {
    	u8g2_SetContrast(&u8g2, CONTRASTOFFSET + (contrast & (CONTRASTMAX - 1)));
    } else {
    	u8g2_SetContrast(&u8g2, CONTRASTOFFSET + (CONTRASTMAX - 1) - (contrast & (CONTRASTMAX - 1)));
    }
    contrast++;
    HAL_Delay(100);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_LSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, VFD_FILEN_Pin|VFD_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(VFD_CS_GPIO_Port, VFD_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : VFD_FILEN_Pin */
  GPIO_InitStruct.Pin = VFD_FILEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(VFD_FILEN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : VFD_RESET_Pin VFD_CS_Pin */
  GPIO_InitStruct.Pin = VFD_RESET_Pin|VFD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
	switch(msg){
		//Initialize SPI peripheral
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
			/* HAL initialization contains all what we need so we can skip this part. */
			break;
		//Function which implements a delay, arg_int contains the amount of ms
		case U8X8_MSG_DELAY_MILLI:
			HAL_Delay(arg_int);
			break;
		//Function which delays 10us
		case U8X8_MSG_DELAY_10MICRO:
			for (uint16_t n = 0; n < 320; n++) {
				__NOP();
			}
			break;
		//Function which delays 100ns
		case U8X8_MSG_DELAY_100NANO:
			__NOP();
			break;

		//Function which implements a delay, arg_int contains the amount of ns
		case U8X8_MSG_DELAY_NANO:
			for (uint16_t n = 0; n < (arg_int>>4); n++) {
				__NOP();
			}
			break;

		//Function to define the logic level of the clockline
		case U8X8_MSG_GPIO_SPI_CLOCK:
			/*if (arg_int) {
				HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, RESET);
			} else {
				HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, SET);
			}*/
		break;
		//Function to define the logic level of the data line to the display
		case U8X8_MSG_GPIO_SPI_DATA:
			/*if (arg_int) {
				HAL_GPIO_WritePin(MOSI_GPIO_Port, MOSI_Pin, SET);
			} else {
				HAL_GPIO_WritePin(MOSI_GPIO_Port, MOSI_Pin, RESET);
			}*/

		break;
		// Function to define the logic level of the CS line
		case U8X8_MSG_GPIO_CS:
			if (arg_int) {
				HAL_GPIO_WritePin(VFD_CS_GPIO_Port, VFD_CS_Pin, GPIO_PIN_SET);
			} else {
				HAL_GPIO_WritePin(VFD_CS_GPIO_Port, VFD_CS_Pin, GPIO_PIN_RESET);
			}

		break;
		//Function to define the logic level of the Data/ Command line
		case U8X8_MSG_GPIO_DC:
			/*if (arg_int) {
				HAL_GPIO_WritePin(VFD_DC_GPIO_Port, VFD_DC_Pin, GPIO_PIN_SET);
			} else {
				HAL_GPIO_WritePin(VFD_DC_GPIO_Port, VFD_DC_Pin, GPIO_PIN_RESET);
			}*/

		break;
		//Function to define the logic level of the RESET line
		case U8X8_MSG_GPIO_RESET:
			if (arg_int) {
				HAL_GPIO_WritePin(VFD_RESET_GPIO_Port, VFD_RESET_Pin, GPIO_PIN_SET);
			} else {
				HAL_GPIO_WritePin(VFD_RESET_GPIO_Port, VFD_RESET_Pin, GPIO_PIN_RESET);
			}

		break;
		default:
			return 0; //A message was received which is not implemented, return 0 to indicate an error
	}

	return 1; // command processed successfully.
}


uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch (msg) {
    case U8X8_MSG_BYTE_INIT:
      u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
      uint8_t dSend = 0x0F;
      HAL_SPI_Transmit(&hspi1, &dSend, sizeof(dSend), 100);
      break;
    case U8X8_MSG_BYTE_SET_DC:
      u8x8_gpio_SetDC(u8x8, arg_int);
      break;
    case U8X8_MSG_BYTE_START_TRANSFER:
      u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
      break;
    case U8X8_MSG_BYTE_SEND:
      HAL_SPI_Transmit(&hspi1, arg_ptr, arg_int, 100);
      break;
    case U8X8_MSG_BYTE_END_TRANSFER:
      u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
      break;
    default:
      return 0;
  }

  return 1;
}

void draw(u8g2_t *u8g2)
{
  static uint_fast16_t ssLPos = 0;

  u8g2_SetFontMode(u8g2, 1);  // Transparent
  u8g2_SetDrawColor(u8g2,1);

  u8g2_DrawLine(u8g2,0,0,10,0);
  u8g2_DrawLine(u8g2,0,0,0,10);

  u8g2_DrawLine(u8g2,0,49,10,49);
  u8g2_DrawLine(u8g2,0,40,0,50);

  u8g2_DrawLine(u8g2,245,49,255,49);
  u8g2_DrawLine(u8g2,255,40,255,50);

  u8g2_DrawLine(u8g2,245,0,255,0);
  u8g2_DrawLine(u8g2,255,0,255,10);

  u8g2_DrawFrame(u8g2,5,16,30,30);

  u8g2_DrawLine(u8g2,37,45,68,45);
  u8g2_DrawLine(u8g2,52,15,37,45);
  u8g2_DrawLine(u8g2,52,15,68,45);

  u8g2_DrawCircle(u8g2,83,30,15,U8G2_DRAW_ALL);

  u8g2_SetFont(u8g2, u8g2_font_6x12_tf);
  u8g2_DrawStr(u8g2, 5,10,"GP1287BI 256x50");
  u8g2_DrawStr(u8g2, 194,10,"By U8G2");
  char stringLine[25];
  strncpy(stringLine, "Draw String : ", 15);
  strncat(stringLine, inTxt, 10);
  u8g2_DrawStr(u8g2, 110,20,stringLine);
  u8g2_DrawStr(u8g2, 110,30,"Draw Number : 12345678");
  u8g2_DrawStr(u8g2, 110,40,"Draw Float  : 3.141592");

  u8g2_SetDrawColor(u8g2,2);
  u8g2_DrawBox(u8g2,108,2,80,10);
  u8g2_DrawBox(u8g2,108,42,133,7);

  u8g2_DrawStr(u8g2, 110,10,"Color Invert");

  u8g2_DrawLine(u8g2, ssLPos & 0xFF, 0, ssLPos & 0xFF, 49);
  ssLPos++;
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
