#include "bluetooth.h"
#include "devStatus.h"
#include <stm32g4xx_hal.h>
#include <main.h>
#include <stm32g4xx_hal_uart.h>
#include <usart.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



uint8_t bluetoothInit(){

	if(HAL_UART_Receive_DMA(&huart2, btRxBuff, 8) != HAL_OK) return 0;
	HAL_GPIO_WritePin(BT_RST_GPIO_Port, BT_RST_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BT_MODE_GPIO_Port, BT_MODE_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(BT_RST_GPIO_Port, BT_RST_Pin, GPIO_PIN_SET);

	HAL_Delay(100);

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

	bluetoothGetScannedDevices();

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

uint8_t bluetoothCMD_Until(char *cmd, char *terminator, char *recvBuffer){
	btRxStatus = 2;
	btRxIndex = 0;
	if(HAL_UART_Receive_DMA(&huart2, (uint8_t*)&btRxByte, 1) != HAL_OK) return 0;
	if(HAL_UART_Transmit_DMA(&huart2, (uint8_t*)cmd, strlen(cmd)) != HAL_OK) return 0;

	//Ceka nez prijde terminator
	while(!strstr((char *)btRxBuff, terminator) && btRxIndex < BT_RX_BUFF_SIZE);

	if(strstr((char *)btRxBuff, terminator) == 0) return 0;

	memcpy(recvBuffer, (char *)btRxBuff, btRxIndex);

	HAL_UART_DMAStop(&huart2);

	return 1;
}

uint8_t bluetoothCMD_Time(char *cmd, uint8_t s, char *recvBuffer){
	btRxStatus = 2;
	btRxIndex = 0;
	if(HAL_UART_Receive_DMA(&huart2, (uint8_t*)&btRxByte, 1) != HAL_OK) return 0;
	if(HAL_UART_Transmit_DMA(&huart2, (uint8_t*)cmd, strlen(cmd)) != HAL_OK) return 0;


	long long int start = HAL_GetTick();

	//Ceka dany cas
	while(btRxIndex < BT_RX_BUFF_SIZE && HAL_GetTick() < (start+s*1000));


	//HAL_GetTick()
	//sprintf(oledHeader,"Done");

	//if(strstr((char *)btRxBuff, terminator) == 0) return 0;
	memcpy(recvBuffer, (char *)btRxBuff, btRxIndex-1);

	HAL_UART_DMAStop(&huart2);

	return 1;
}


uint8_t bluetoothGetScannedDevices(){
	char buff[100];
	//memset(oledHeader, 0, 30);
	//memset(buff, 0, 100);

	if(!bluetoothCMD_Time("F\r", 20, buff)) return 0;

	char *devices[20];
	btScannedCount = splitString(buff, "\n", devices);

	/*
	replacechar(buff, '\n', ' ');*/



	for(uint8_t i = 0; i <= btScannedCount; i++){
		replacechar(devices[i+1], ',', ' ');
		replacechar(devices[i+1], '%', ' ');
		char * mac = (char*)malloc(30);
		sscanf(devices[i+1], " %s ", mac);
		btScanedDevices[i].font = &Font_7x10;
		btScanedDevices[i].name = mac;
		btScanedDevices[i].selected = 0;
		btScanedDevices[i].hasSpecialSelector = 0;
		btScanedDevices[i].specharNotSelected = 0;
		btScanedDevices[i].specharSelected = 0;
		btScanedDevices[i].submenuLevel = 3;
		btScanedDevices[i].parentItem = &bluetoothmenu[0].name;
	}

	return 1;

}

uint32_t countOccurances(char * buff){
	uint32_t count = 0;
	const char * tmp = buff;
	while((tmp = strstr(tmp, "\n")))
	{
		count++;
		tmp++;
	}

	return count;
}

uint32_t replacechar(char *str, char orig, char rep){
    char *ix = str;
    uint32_t n = 0;
    while((ix = strchr(ix, orig)) != NULL) {
        *ix++ = rep;
        n++;
    }
    return n;
}

int splitString(char * string, char * delim, char ** array){

	int count = 0;
	char * token = strtok(string, delim);
	array[count++] = token;

	while( token != NULL ) {
	      token = strtok(NULL, delim);
	      array[count++] = token;
	}

	return 1;
}
