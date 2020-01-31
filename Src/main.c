/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "ucpd.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"
#include "ws2812.h"
#include "devStatus.h"
#include "oled.h"
#include "bluetooth.h"

uint16_t GPIO_Pin_Flag;
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
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
  MX_DMA_Init();
  MX_UCPD1_Init();
  MX_USB_Device_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_CRC_Init();
  MX_ADC2_Init();
  MX_TIM15_Init();
  MX_TIM17_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  ws2812_init();

  setStatusAll(0, DEV_LOAD);

  oled_begin();


  if(bluetoothInit()){
	  setStatus(DEV_BLUETOOTH, DEV_OK);
  }else{
	  setStatus(DEV_BLUETOOTH, DEV_ERR);
  }





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

  /** Configure the main internal regulator output voltage 
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV6;
  RCC_OscInitStruct.PLL.PLLN = 36;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV6;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks 
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_USB|RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_SYSCLK;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_SYSCLK;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

	GPIO_Pin_Flag = GPIO_Pin;

	if((GPIO_Pin == GPIO_PIN_0 || GPIO_Pin == GPIO_PIN_1) && HAL_TIM_Base_GetState(&htim1) != HAL_TIM_STATE_BUSY){

			HAL_TIM_Base_Start_IT(&htim1);
			HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
			HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
			HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);
			HAL_NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);

			HAL_NVIC_EnableIRQ(EXTI0_IRQn);
			HAL_NVIC_EnableIRQ(EXTI1_IRQn);

	}

}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){


	if(htim->Instance == TIM1){

		HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);
		HAL_NVIC_DisableIRQ(TIM1_UP_TIM16_IRQn);
		HAL_NVIC_DisableIRQ(TIM1_TRG_COM_TIM17_IRQn);
		HAL_NVIC_DisableIRQ(TIM1_BRK_TIM15_IRQn);
		 HAL_TIM_Base_Stop_IT(&htim1);

		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) != HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) && GPIO_Pin_Flag == GPIO_PIN_0){
			encoderpos++;
		}else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) && GPIO_Pin_Flag == GPIO_PIN_0){
			encoderpos--;
		}

		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) && GPIO_Pin_Flag == GPIO_PIN_1){
			setStatus(FRONT3, DEV_DATA);


			setStatus(DEV_CURRENT, DEV_DATA);
			HAL_GPIO_TogglePin(CURRENT_SOURCE_GPIO_Port, CURRENT_SOURCE_Pin);
			encoderclick = 1;
		}

		HAL_NVIC_EnableIRQ(EXTI0_IRQn);
		HAL_NVIC_ClearPendingIRQ(EXTI0_IRQn);
		HAL_NVIC_EnableIRQ(EXTI1_IRQn);
		HAL_NVIC_ClearPendingIRQ(EXTI1_IRQn);

	}

	if(htim->Instance == TIM2){
		//Obnovi se oled displej
		oled_refresh();
		proccessPendingStatus();
	}

	if(htim->Instance == TIM3){
			//setStatus(FRONT3, DEV_DATA);
			//btRxComplete = 0;
			//CDC_Transmit_FS(btRxBuff, sizeof(btRxBuff));

	}

}

void USB_received_handle(char * buff, uint32_t len){

	//setStatus(DEV_USB, DEV_DATA);

	/*if(len > 4 && buff[0] == 0 && buff[1] == 0 && buff[2] == 0 && buff[3] == 0){
		setStatus(FRONT1, DATA);
	}else{
		HAL_UART_Transmit(&huart3, (uint8_t*)buff, len, HAL_MAX_DELAY);
	}*/

	//HAL_UART_Transmit(&huart2, (uint8_t*)buff, len, HAL_MAX_DELAY);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	if(huart->Instance == USART1){
		setStatus(DEV_DISP, DEV_DATA);
		//Ukazatel
	}else if(huart->Instance == USART2){
		setStatus(DEV_BLUETOOTH, DEV_DATA);


	}else if(huart->Instance == USART3){
		//setStatus(DEV_MIDI, DEV_DATA);
		//MIDI
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	/*if(huart->Instance == USART1){
		//Ukazatel
	}else if(huart->Instance == USART2){

		//setStatus(DEV_BLUETOOTH, DEV_DATA);
		//Spusti se prijem
		btRxComplete = 0;
		btRxBuffIndex = 0;
		HAL_UART_Receive_IT(&huart2, &btRxByte, 1);


	}else if(huart->Instance == USART3){
		//MIDI
	}*/
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
