/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_ucpd.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_cortex.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_system.h"
#include "stm32g4xx_ll_utils.h"
#include "stm32g4xx_ll_pwr.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_dma.h"

#include "stm32g4xx_ll_exti.h"

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
void USB_transmit_handle(char * buff, uint32_t len);
void USB_received_handle(char * buff, uint32_t len);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DISP_SENSE_Pin GPIO_PIN_13
#define DISP_SENSE_GPIO_Port GPIOC
#define BT_MODE_Pin GPIO_PIN_0
#define BT_MODE_GPIO_Port GPIOA
#define BT_RST_Pin GPIO_PIN_1
#define BT_RST_GPIO_Port GPIOA
#define CURRENT_SOURCE_Pin GPIO_PIN_4
#define CURRENT_SOURCE_GPIO_Port GPIOA
#define MIDI_IO_SELECTED_Pin GPIO_PIN_5
#define MIDI_IO_SELECTED_GPIO_Port GPIOA
#define MIDI_ACTIVE_Pin GPIO_PIN_6
#define MIDI_ACTIVE_GPIO_Port GPIOA
#define MIDI_SEARCHING_Pin GPIO_PIN_7
#define MIDI_SEARCHING_GPIO_Port GPIOA
#define ENCODER_A_Pin GPIO_PIN_0
#define ENCODER_A_GPIO_Port GPIOB
#define ENCODER_A_EXTI_IRQn EXTI0_IRQn
#define ENCODER_SW_Pin GPIO_PIN_1
#define ENCODER_SW_GPIO_Port GPIOB
#define ENCODER_SW_EXTI_IRQn EXTI1_IRQn
#define ENCODER_B_Pin GPIO_PIN_2
#define ENCODER_B_GPIO_Port GPIOB
#define MIDI_TX_Pin GPIO_PIN_10
#define MIDI_TX_GPIO_Port GPIOB
#define MIDI_RX_Pin GPIO_PIN_11
#define MIDI_RX_GPIO_Port GPIOB
#define FRONT_LED_DIN_Pin GPIO_PIN_14
#define FRONT_LED_DIN_GPIO_Port GPIOB
#define UKAZATEL_VCC_SENSE_Pin GPIO_PIN_15
#define UKAZATEL_VCC_SENSE_GPIO_Port GPIOB
#define UKAZATEL_TX_Pin GPIO_PIN_9
#define UKAZATEL_TX_GPIO_Port GPIOA
#define UKAZATEL_RX_Pin GPIO_PIN_10
#define UKAZATEL_RX_GPIO_Port GPIOA
#define BACK_LED_DIN_Pin GPIO_PIN_5
#define BACK_LED_DIN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
