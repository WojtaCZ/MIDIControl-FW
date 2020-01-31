#include "bluetooth.h"
#include "devStatus.h"
#include <stm32g4xx_hal.h>
#include <main.h>
#include <stm32g4xx_hal_uart.h>
#include <usart.h>
#include <string.h>
#include "oled.h"



uint8_t bluetoothInit(){

	if(HAL_UART_Receive_DMA(&huart2, btRxBuff, 8) != HAL_OK) return 0;
	HAL_GPIO_WritePin(BT_RST_GPIO_Port, BT_RST_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BT_MODE_GPIO_Port, BT_MODE_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(BT_RST_GPIO_Port, BT_RST_Pin, GPIO_PIN_SET);

	HAL_Delay(1000);

	while(HAL_DMA_GetState(&hdma_usart2_rx) == HAL_DMA_STATE_BUSY);
	if(strcmp("%REBOOT%", (char *)btRxBuff) != 0) return 0;
	HAL_UART_DMAStop(&huart2);


	//Zapne se CMD
	if(!bluetoothCMD_ACK("$$$", "CMD> ")) return 0;

	//Zkontroluje se název
	if(!bluetoothCMD_ACK("GN\r", "MIDIControl\r\nCMD> ")){
		if(!bluetoothCMD_ACK("SN,MIDIControl\r", BT_AOK)) return 0;
	}

	//Nastavi se vyzarovaci vykon
	if(!bluetoothCMD_ACK("SGA,0\r", BT_AOK)) return 0;
	if(!bluetoothCMD_ACK("SGC,0\r", BT_AOK)) return 0;

	if(!bluetoothCMD_ACK("LB\r", BT_AOK)) return 0;

	 return 1;
}


uint8_t bluetoothCMD_ACK(char *cmd, char *ack){
		memset(btRxBuff, 0, sizeof(btRxBuff));


		if(HAL_UART_Transmit_DMA(&huart2, (uint8_t*)cmd, strlen(cmd)) != HAL_OK) return 0;
		while(HAL_DMA_GetState(&hdma_usart2_tx) == HAL_DMA_STATE_BUSY);
		while(HAL_UART_GetState(&huart2) == HAL_UART_STATE_BUSY_TX);
		if(HAL_UART_Receive_DMA(&huart2, btRxBuff, strlen(ack)) != HAL_OK) return 0;
		while(HAL_DMA_GetState(&hdma_usart2_rx) == HAL_DMA_STATE_BUSY);
		while(HAL_UART_GetState(&huart2) == HAL_UART_STATE_BUSY_RX);

		HAL_UART_DMAStop(&huart2);
		if(strstr((char *)btRxBuff, ack) == 0) return 0;

		return 1;

}

uint8_t bluetoothCMD_Until(char *cmd, char *ack){


	return 1;
}
