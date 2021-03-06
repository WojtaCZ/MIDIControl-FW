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
#include "rtc.h"
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
#include "midiControl.h"
#include "msgDecoder.h"

uint16_t GPIO_Pin_Flag;
extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);
extern uint8_t MIDI_Transmit_FS(uint8_t* Buf, uint16_t Len);
uint8_t b;



int readBytes = 0;
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
extern struct menuitem mainmenu[5];
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

  BootloaderCheck();

  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */

  /*Configure GPIO pin : PtPin */

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USB_Device_Init();
  MX_UCPD1_Init();
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
  MX_RTC_Init();
  MX_TIM4_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */




  //Inicializuje se system MIDIControl
  midiControl_init();

  //Restartuje attiny na desce
  midiControl_midiIO_init();

  HAL_RTC_Init(&hrtc);

  ws2812_init();

  oled_begin();

  setStatusAll(0, DEV_LOAD);

  if(bluetoothInit()){
	  setStatus(DEV_BLUETOOTH, DEV_OK);
	  bluetoothStatus = BT_STATUS_OK;
  }else{
	  setStatus(DEV_BLUETOOTH, DEV_ERR);
	  bluetoothStatus = BT_STATUS_ERR;
  }

  //Prohleda zarizeni v okoli a pripoji se ke znamemu ovladaci
  if(bluetoothConnectKnown()){
	  btStreamOpen = 1;
	  btCmdMode = 0;
  }

  midiControl_midiIO_getState();


 /*if(!alivePC){
	  oled_setDisplayedSplash(oled_UsbWaitingSplash, "");
	  oledType = OLED_SPLASH;
	  while(!alivePC){
		  if(oledType != OLED_SPLASH) oledType = OLED_SPLASH;
		  if(btMsgReceivedFlag){
			  decodeMessage(uartMsgDecodeBuff, ((uartMsgDecodeBuff[6] & 0x04) >> 2));
			  btMsgReceivedFlag = 0;
		  }
	  }
  }*/

  //Ziska se aktualni cas
  midiControl_get_time();


  //Skoci do menu
  oledType = OLED_MENU;

  HAL_UART_Receive_IT(&huart3, &midiFifo[midiFifoIndex++], 1);
  HAL_UART_Receive_DMA(&huart1, dispData, 9);
  HAL_TIM_Base_Start_IT(&htim7);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  //Pokud byl request na skenovani
	  if(workerBtScanDev.assert){
		 if(bluetoothGetScannedDevices()){
			 oled_setDisplayedMenu("btScanedDevices", &btScanedDevices, sizeof(btScanedDevices)-(20-btScannedCount-1)*sizeof(btScanedDevices[19]), 0);
		 }else oled_setDisplayedSplash(oled_NothingFound, NULL);
		 workerBtScanDev.assert = 0;
	  }

	  if(workerBtBondDev.assert){
		 if(bluetoothGetBondedDevices()){
			 oled_setDisplayedMenu("btBondedDevicesMenu", &btBondedDevicesMenu, sizeof(btBondedDevicesMenu)-(10-btBondedCount-1)*sizeof(btBondedDevicesMenu[9]), 0);
		 }else oled_setDisplayedSplash(oled_NothingFound, NULL);
		 workerBtBondDev.assert = 0;
	  }

	  if(btMsgReceivedFlag){
		  uint8_t dest = uartMsgDecodeBuff[6] & 0x03;
		  uint8_t broadcast = (uartMsgDecodeBuff[6] & 0x04) >> 2;
		  uint16_t msgLen = ((uartMsgDecodeBuff[4]&0xff)<<8 | (uartMsgDecodeBuff[5]&0xff));

		  if(dest == ADDRESS_MAIN || broadcast){
			  decodeMessage(uartMsgDecodeBuff, ((uartMsgDecodeBuff[6] & 0x04) >> 2));
		  }else{
			 // sprintf(oledHeader, "D:%d B:%d", dest, broadcast);
			  CDC_Transmit_FS(uartMsgDecodeBuff, msgLen+6);
		  }

		  btMsgReceivedFlag = 0;
	  }

	  if(workerBtRemoveController.assert){
	  		if(!btCmdMode) bluetoothEnterCMD();
	  		char cmd[10];
	  		sprintf(cmd,"U,%d\r", (btSelectedController+1));
	  		bluetoothCMD_ACK(cmd, BT_AOK);
	  		if(btCmdMode) bluetoothLeaveCMD();
	  		oled_setDisplayedMenu("mainmenu",&mainmenu, sizeof(mainmenu), 0);
	  		workerBtRemoveController.assert = 0;
	  }

	  if(workerBtConnect.assert){
		  bluetoothConnect(workerBtConnectMAC);
		  //workerAssert(&workerBtBond);
		  workerBtConnect.assert = 0;
	  }


	  if(workerBtBond.assert){
	  		  bluetoothBond();
	  		  workerBtBond.assert = 0;
	  }

	  if(workerBtScanBondable.assert){
		  if(bluetoothGetBondableDevices()){
			  oled_setDisplayedMenu("btBondableDevices", &btBondableDevices, sizeof(btBondableDevices)-(20-btBondableCount-1)*sizeof(btBondableDevices[19]), 0);
		  }else oled_setDisplayedSplash(oled_NothingFound, NULL);
		  workerBtScanBondable.assert = 0;
	  }

	  if(workerGetSongs.assert){
		  if(workerGetSongs.status == WORKER_OK){
		      oled_setDisplayedMenu("songmenu",&songMenu, (songMenuSize+1)*sizeof(struct menuitem), 1);
			  workerGetSongs.assert = 0;
		  }else if(workerGetSongs.status == WORKER_ERR){
			  oled_setDisplayedSplash(oled_ErrorSplash, "pri nacitani pisni");
			  workerGetSongs.assert = 0;
		  }
	  }

	  if(workerMiscellaneous.assert){
		   //Kontroluje statusy periferii
		   midiControl_display_getState();

		   midiControl_midiIO_getState();

		  //Odesle informaci o svoji pritonosti
		  char msg[] = {INTERNAL_COM, INTERNAL_COM_KEEPALIVE};
		  sendMsg(ADDRESS_MAIN, ADDRESS_OTHER, 1, INTERNAL, msg, 2);

		  workerDesert(&workerMiscellaneous);
	  }

	  if(workerDispRefresh.assert){
		  char buff[20];
		  uint8_t changed = 0;

		  //Pokud se lisi nastavene a existujici cislo pisne
		  sprintf(buff, "%c%c%c%c", dispSong[3], dispSong[2], dispSong[1], dispSong[0]);
		  if(strcmp(numDispSong.enteredValue,buff)){
			  dispSong[3] = numDispSong.enteredValue[0];
			  dispSong[2] = numDispSong.enteredValue[1];
			  dispSong[1] = numDispSong.enteredValue[2];
			  dispSong[0] = numDispSong.enteredValue[3];
			  changed = 1;
		  }

		  //Pokud se lisi nastavene a existujici cislo sloky
		  sprintf(buff, "%c%c", dispVerse[1], dispVerse[0]);
		  if(strcmp(numDispSong.enteredValue,buff)){
			  dispVerse[1] = numDispVerse.enteredValue[0];
			  dispVerse[0] = numDispVerse.enteredValue[1];
		  	  changed = 1;
		  }

		  //Pokud se lisi nastavene a existujici pismeno
		  if(numDispLetter.enteredValue[0] != dispLetter){
			  dispLetter = numDispLetter.enteredValue[0];
			  changed = 1;
		  }

		  if(dispLED != dispLEDOld){
			  changed = 1;
		  }

		  if(changed){
			  uint8_t data[9];
			  data[0] = 0xB0;
			  data[1] =  (dispVerse[0] <= '9' && dispVerse[0] >= '0') ? dispVerse[0]-48 : 0xE0;
			  data[2] =  (dispVerse[1] <= '9' && dispVerse[1] >= '0') ? dispVerse[1]-48 : 0xE0;
			  data[3] =  (dispSong[0] <= '9' && dispSong[0] >= '0') ? dispSong[0]-48 : 0xE0;
			  data[4] =  (dispSong[1] <= '9' && dispSong[1] >= '0') ? dispSong[1]-48 : 0xE0;
			  data[5] =  (dispSong[2] <= '9' && dispSong[2] >= '0') ? dispSong[2]-48 : 0xE0;
			  data[6] =  (dispSong[3] <= '9' && dispSong[3] >= '0') ? dispSong[3]-48 : 0xE0;
			  data[7] =  (dispLetter <= 'D' && dispLetter >= 'A') ? dispLetter-55 : 0xE0;
			  data[8] =	 (dispLED <= 3 && dispLetter >= 0) ? 0x20 | dispLED : 0xE0;
			  midiControl_setDisplayRaw(data, 9);
			  changed = 0;
		  }

		  workerDesert(&workerDispRefresh);
	  }

	  if(workerRecord.assert){
		  if(workerRecord.status == WORKER_REQUEST){
			  char msg[50];
			  msg[0] = INTERNAL_COM;
			  msg[1] = INTERNAL_COM_CHECK_NAME;
			  sprintf(&msg[2], "%s", numRecordSong.enteredValue);
			  sendMsg(ADDRESS_MAIN, ADDRESS_PC, 0, INTERNAL, msg, strlen(numRecordSong.enteredValue)+2);
			  workerRecord.status = WORKER_WAITING;
		  }else if(workerRecord.status == WORKER_OK){
			  midiControl_record(ADDRESS_MAIN, numRecordSong.enteredValue);
			  workerDesert(&workerRecord);
		  }else if(workerRecord.status == WORKER_ERR){
			  oled_setDisplayedSplash(oled_NameExistsSplash, "");
			  workerDesert(&workerRecord);
		  }

	  }


	 /* if(!alivePC){
		  setStatusAll(1, DEV_CLR);
	  	  oled_setDisplayedSplash(oled_UsbWaitingSplash, "");
	  	  oledType = OLED_SPLASH;
	  	  oled_refresh();
	  	  while(!alivePC){
	  		if(oledType != OLED_SPLASH) oledType = OLED_SPLASH;
	  		if(btMsgReceivedFlag){
				decodeMessage(uartMsgDecodeBuff, ((uartMsgDecodeBuff[6] & 0x04) >> 2));
				btMsgReceivedFlag = 0;
	  		}
	  	  }
	  	  oledType = OLED_MENU;
	  }*/

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_USART3
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_USB
                              |RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_SYSCLK;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_SYSCLK;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{



	if(GPIO_Pin == GPIO_PIN_0 || GPIO_Pin == GPIO_PIN_1){
			GPIO_Pin_Flag = GPIO_Pin;
			if(HAL_TIM_Base_GetState(&htim1) != HAL_TIM_STATE_BUSY){
					HAL_TIM_Base_Start_IT(&htim1);
					HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
					HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
					HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);
					HAL_NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);

					HAL_NVIC_EnableIRQ(EXTI0_IRQn);
					HAL_NVIC_EnableIRQ(EXTI1_IRQn);
			}

	}

	if(GPIO_Pin == GPIO_PIN_5 || GPIO_Pin == GPIO_PIN_6 || GPIO_Pin == GPIO_PIN_7){



	}

}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){


	if(htim->Instance == TIM1){

		HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);
		HAL_NVIC_DisableIRQ(TIM1_UP_TIM16_IRQn);
		HAL_NVIC_DisableIRQ(TIM1_TRG_COM_TIM17_IRQn);
		HAL_NVIC_DisableIRQ(TIM1_BRK_TIM15_IRQn);
		HAL_TIM_Base_Stop_IT(&htim1);

		if(!encoderDirSwap){
			if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) != HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) && GPIO_Pin_Flag == GPIO_PIN_0){
				encoderpos++;
			}else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) && GPIO_Pin_Flag == GPIO_PIN_0){
				encoderpos--;
			}
		}else{
			if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) != HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) && GPIO_Pin_Flag == GPIO_PIN_0){
				encoderpos--;
			}else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) && GPIO_Pin_Flag == GPIO_PIN_0){
				encoderpos++;
			}
		}


		//Dopocita se pozice v dispmenu
		if(encoderpos >= (signed int)(dispmenusize)-1){
			encoderpos = (signed int)(dispmenusize)-1;
		}else if(encoderpos < (signed int)0){
			encoderpos = 0;
		}

		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET/* && GPIO_Pin_Flag == GPIO_PIN_1*/){
			encoderclick = 1;
			if(refreshHalt) oled_refresh();
		}

		HAL_NVIC_EnableIRQ(EXTI0_IRQn);
		HAL_NVIC_ClearPendingIRQ(EXTI0_IRQn);
		HAL_NVIC_EnableIRQ(EXTI1_IRQn);
		HAL_NVIC_ClearPendingIRQ(EXTI1_IRQn);

	}

	if(htim->Instance == TIM4){

		midiControl_keepalive_process();

		if(loadingStat == 2){
			workerAssert(&workerMiscellaneous);
		}

		//Tady se dela scrollovani
		if(scrollPauseDone){
			if(scrollIndex <= scrollMax){
				scrollIndex++;
			}else{
				scrollPauseDone = 0;
			}
		}else scrollPause++;

		if(scrollPause == OLED_MENU_SCROLL_PAUSE){
			if(scrollIndex > 0){
				scrollPauseDone = 0;
			}else scrollPauseDone = 1;

			scrollPause = 0;
			scrollIndex = 0;
		}

		if(loadingStat < 4){
			loadingStat <<= 1;
		}else loadingStat = 1;

		loadingToggle = ~loadingToggle;

		/*if(loadingToggle){
			uint8_t note[] = {0x08, 0x90, 0x47, 0x47};
			MIDI_Transmit_FS(note, 4);
		}else{
			uint8_t note[] = {0x08, 0x80, 0x47, 0x47};
			MIDI_Transmit_FS(note, 4);
		}*/

		if(btStatusMsg){
			btStatusMsgWD++;

			if(btStatusMsgWD >= 2){
				bluetoothMsgFifoFlush();
				btStatusMsg = 0;
				btStatusMsgWD = 0;
			}
		}else btStatusMsgWD = 0;

		if(refreshHalt) oled_refresh();
	}

	if(htim->Instance == TIM2){
		//Obnovi se oled displej
		if(!refreshHalt) oled_refresh();
		proccessPendingStatus();
	}

	if(htim->Instance == TIM7){
		if(btData && btDataIcon == -1){
			btDataIcon = 0;
		}

		if(btDataIcon != -1 && btDataIcon < 3){
			btDataIcon++;
		}else if(btDataIcon != -1){
			btDataIcon = -1;
			btData = 0;
		}
	}


}

void USB_CDC_received_handle(char * buff, uint32_t len){

	setStatus(DEV_USB, DEV_DATA);

	uint16_t msgLen = ((buff[4]&0xff)<<8 | (buff[5]&0xff));

	memcpy(&decoderBuffer, buff, msgLen+6);

	//Pokud je pro toto zarizeni
	if((buff[6] & 0x03) == ADDRESS_MAIN && ((buff[6] & 0x04) >> 2) == 0){
		decodeMessage(decoderBuffer, (buff[6] & 0x04) >> 3);
	}else if((buff[6] & 0x04) >> 2){
		//Pokud je broadcast
		//Dekoduje a preposle na BT
		if(!btCmdMode && btStreamOpen){
			HAL_UART_Transmit_IT(&huart2, decoderBuffer, msgLen+6);
		}

		decodeMessage(decoderBuffer, (buff[6] & 0x04) >> 3);

	}else{
		//Jen preposle na BT
		if(!btCmdMode && btStreamOpen){
			HAL_UART_Transmit_IT(&huart2, decoderBuffer, msgLen+6);
		}
	}

}

void USB_CDC_transmit_handle(char * buff, uint32_t len){
	setStatus(DEV_USB, DEV_DATA);
}

void USB_MIDI_received_handle(char * buff, uint32_t len){
	setStatus(FRONT3, DEV_DATA);
	HAL_UART_Transmit_IT(&huart3, (uint8_t*)buff+1, len-1);
}


void USB_MIDI_transmit_handle(char * buff, uint32_t len){
	setStatus(FRONT3, DEV_DATA);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	if(huart->Instance == USART3){
		if(midiStatus == MIDI_A){
			setStatus(DEV_MIDIA, DEV_DATA);
		}else if(midiStatus == MIDI_B){
			setStatus(DEV_MIDIB, DEV_DATA);
		}

		//Precte se typ zpravy
		uint8_t msgType = midiFifo[0];

		//Pokud prijde byte validni MIDI zpravy
		if((msgType & 0xF0) >= 0x80 && !midiGotMessage){
			midiGotMessage = 1;

			//Zpravy co maji 2 byty
			if((msgType >= 0x80 && msgType <= 0xBF) || (msgType & 0xF0) == 0xE0 || msgType == 0xF2 || msgType == 0xF0){
				HAL_UART_Receive_IT(&huart3, &midiFifo[midiFifoIndex], 2);
				midiFifoIndex += 2;
			}else if((msgType & 0xF0) == 0xC0 ||  (msgType & 0xF0) == 0xD0 || msgType == 0xF3){
				HAL_UART_Receive_IT(&huart3, &midiFifo[midiFifoIndex++], 1);
			}else{
				midiFifoIndex = 0;
				HAL_UART_Receive_IT(&huart3, &midiFifo[midiFifoIndex++], 1);
			}


		}else if(midiGotMessage){
			sprintf(oledHeader, "%x %x %x %x",msgType,midiFifo[midiFifoIndex-1], midiFifo[midiFifoIndex], midiFifo[midiFifoIndex+1]);
			//Pokud byla zprava sysex a prisel sysex end
			if(msgType == 0xF0 && midiFifo[midiFifoIndex-1] == 0xF7){
				midiFifoIndex = 0;
				midiFifo[midiFifoIndex] = 0;
				midiGotMessage = 0;
				HAL_UART_Receive_IT(&huart3, &midiFifo[midiFifoIndex++], 1);
			}else if(msgType == 0xF0 && midiFifo[midiFifoIndex-1] != 0xF7){
				//Pokud byla zprava sysex a prisly data
				HAL_UART_Receive_IT(&huart3, &midiFifo[midiFifoIndex++], 1);
			}else{
				//Ostatni zpravy
				uint8_t buffer[4];
				//Vynuluje se buffer
				memset(buffer,0,4);
				//Vytvori se CN a CIN
				buffer[0] = ((midiFifo[0] >> 4) & 0x0F);
				//Data se presunou do bufferu
				memcpy(&buffer[1], &midiFifo[0], midiFifoIndex);


				MIDI_Transmit_FS(buffer, 4);
				//Zacne se novy prijem
				midiFifoIndex = 0;
				midiGotMessage = 0;
				midiFifo[midiFifoIndex] = 0;
				HAL_UART_Receive_IT(&huart3, &midiFifo[midiFifoIndex++], 1);
			}
		}else{
			//Pokud nema validni MIDI zpravu, prijima dal
			midiGotMessage = 0;
			midiFifoIndex = 0;
			midiFifo[midiFifoIndex] = 0;
			HAL_UART_Receive_IT(&huart3, &midiFifo[midiFifoIndex++], 1);
		}


	}else if(huart->Instance == USART1){
		setStatus(DEV_DISP, DEV_DATA);
		//Ukazatel

		midiControl_refreshDisplayData();

		HAL_UART_Receive_DMA(&huart1, dispData, 9);


	}else if(huart->Instance == USART2){
		setStatus(DEV_BLUETOOTH, DEV_DATA);

		//Pokud dostal status message od modulu
		if((btFifoByte == '%' || btStatusMsg) && !btCmdMode){
			if(btFifoByte == '%') btStatusMsg = ~btStatusMsg;
			if(btFifoByte == '%' && !btStatusMsg) bluetoothDecodeMsg();
			btMsgFifo[btMsgFifoIndex++] = btFifoByte;
		}else if(!btStatusMsg){

			if(!btCmdMode && btFifoByte == 0 && btNullCounter < 4 && btComMessageSizeFlag < 2){
				//Odpocitaji se 4 nully
				btNullCounter++;
			}else if(btNullCounter == 4 && btComMessageSizeFlag < 2){
				btMsgReceivedFlag = 0;
				//Odpocita se velikost
				btComMessageSizeFlag++;
				btMessageLen = 0;
			}else if(btNullCounter == 4 && btComMessageSizeFlag == 2 && btMessageLen == 0){
				//Zapise se index zacatku zpravy
				btComMessageStartIndex = btFifoIndex-6;

				btMessageLen = ((btFifo[btComMessageStartIndex+4] << 8) & 0xff00) | (btFifo[btComMessageStartIndex+5] & 0xff);

			}

			btFifo[btFifoIndex++] = btFifoByte;

			if(btMessageLen > 0 && (btFifoIndex) >= btMessageLen+btComMessageStartIndex+6 && btNullCounter == 4 && btComMessageSizeFlag == 2){

				memcpy(uartMsgDecodeBuff, btFifo+btComMessageStartIndex, btMessageLen+6);
				btMsgReceivedFlag = 1;
				bluetoothFifoFlush();
				btNullCounter = 0;
				btComMessageSizeFlag = 0;
			}

		}

		HAL_UART_Receive_IT(&huart2, &btFifoByte, 1);

	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == USART1){
		setStatus(DEV_DISP, DEV_DATA);
		HAL_UART_Receive_DMA(&huart1, dispData, 9);
		//Ukazatel
	}else if(huart->Instance == USART2){
		setStatus(DEV_BLUETOOTH, DEV_DATA);
		//Spusti se prijem
		//btRxComplete = 0;
		//btRxBuffIndex = 0;
		//HAL_UART_Receive_IT(&huart2, &btRxByte, 1);


	}else if(huart->Instance == USART3){
		if(midiStatus == MIDI_A){
			setStatus(DEV_MIDIB, DEV_DATA);
		}else if(midiStatus == MIDI_B){
			setStatus(DEV_MIDIA, DEV_DATA);
		}
	}
}


void BootloaderCheck(){
	GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = ENCODER_SW_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ENCODER_SW_GPIO_Port, &GPIO_InitStruct);

	if(HAL_GPIO_ReadPin(ENCODER_SW_GPIO_Port, ENCODER_SW_Pin) == GPIO_PIN_RESET){
		__HAL_RCC_GPIOB_CLK_DISABLE();
		HAL_GPIO_DeInit(ENCODER_SW_GPIO_Port, ENCODER_SW_Pin);
		BootloaderInit(1);
	}

	__HAL_RCC_GPIOB_CLK_DISABLE();
	HAL_GPIO_DeInit(ENCODER_SW_GPIO_Port, ENCODER_SW_Pin);
}

void BootloaderInit(uint8_t status){

	volatile uint32_t addr = 0x1FFF0000;

	if(status == 1){

		HAL_RCC_DeInit();
		HAL_DeInit();

		SysTick->CTRL = 0;
		SysTick->LOAD = 0;
		SysTick->VAL  = 0;

		__HAL_RCC_SYSCFG_CLK_ENABLE();
		__HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();

		SCB->VTOR = *(__IO uint32_t*) addr;
		SysMemBootJump = (void (*)(void)) (*((uint32_t *)(addr + 4)));
		__set_MSP(*(__IO uint32_t*) addr);

		SysMemBootJump();

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
