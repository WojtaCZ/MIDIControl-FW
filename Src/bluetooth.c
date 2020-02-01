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

uint8_t bluetoothCMD_Time(char *cmd, uint8_t s, char (*recvBuffer)[]){
	btRxStatus = 2;
	btRxIndex = 0;
	if(HAL_UART_Receive_DMA(&huart2, (uint8_t*)&btRxByte, 1) != HAL_OK) return 0;
	if(HAL_UART_Transmit_DMA(&huart2, (uint8_t*)cmd, strlen(cmd)) != HAL_OK) return 0;


	long long int start = HAL_GetTick();

	//Ceka dany cas
	while(btRxIndex < BT_RX_BUFF_SIZE && HAL_GetTick() < (start+s*1000));

	memcpy(recvBuffer, (char *)btRxBuff, btRxIndex-1);

	HAL_UART_DMAStop(&huart2);

	return 1;
}


uint8_t bluetoothGetScannedDevices(){
	char buff[300];
	//memset(oledHeader, 0, 30);
	//memset(buff, 0, 100);

	if(!bluetoothCMD_Time("F\r", 15, &buff)) return 0;

	char *devices[20];

	btScannedCount = countOccurances(buff, "%")/2;
	replacechar(buff, '\r', ' ');
	splitString(buff, "\n", devices);

	sprintf(oledHeader, "Count: %d", btScannedCount);

	uint8_t i;

	for(i = 0; i < btScannedCount; i++){
		char * mac = (char*)malloc(30);
		char * name = (char*)malloc(30);
		char * macType = (char*)malloc(30);
		char * uuid = (char*)malloc(30);
		char * rssi = (char*)malloc(30);

		if(strstr((char *)devices[i+1], ",,") - (char *)devices[i+1] < 16 && strstr((char *)devices[i+1], ",,")){
			sscanf((char *)devices[i+1], "%*[%]%[^,],%[0-9],,%[^,],%[^%]", name, macType, uuid, rssi);
			unsigned int a,b,c,d,e,f;
		    sscanf(name, "%02X%02X%02X%02X%02X%02X", &a, &b, &c, &d, &e, &f);
		    sprintf(name, "%02X-%02X-%02X-%02X-%02X-%02X", a, b, c, d, e, f);
		}else if(strstr((char *)devices[i+1], ",,") - (char *)devices[i+1] >= 16){
			sscanf((char *)devices[i+1], "%*[%]%[^,],%[0-9],%[^,],,%[^%]", mac, macType, name, rssi);
		}else sscanf((char *)devices[i+1], "%*[%]%[^,],%[0-9],%[^,],%[^,],%[^%]", mac, macType, name, uuid, rssi);

		char *ptr;
		char rssiInt = (signed char)strtol(rssi, &ptr , 16);


		btScanedDevices[i].font = &Font_11x18;
		btScanedDevices[i].name = name;
		btScanedDevices[i].selected = 0;
		btScanedDevices[i].hasSpecialSelector = 0;
		btScanedDevices[i].specharNotSelected = 0;
		btScanedDevices[i].specharSelected = 0;
		btScanedDevices[i].submenuLevel = 3;
		btScanedDevices[i].parentItem = &bluetoothmenu[0].name;
	}


	btScanedDevices[btScannedCount].font = &Font_11x18;
	btScanedDevices[btScannedCount].name = "Zpet";
	btScanedDevices[btScannedCount].selected = 0;
	btScanedDevices[btScannedCount].hasSpecialSelector = 1;
	btScanedDevices[btScannedCount].specharNotSelected = 36;
	btScanedDevices[btScannedCount].specharSelected = 37;
	btScanedDevices[btScannedCount].submenuLevel = 3;
	btScanedDevices[btScannedCount].parentItem = 0;

	return 1;

}

uint32_t countOccurances(char * buff, char * what){
	uint32_t count = 0;
	const char * tmp = buff;
	while((tmp = strstr(tmp, what)))
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

uint32_t splitString(char * string, char * delim, char ** array){

	uint32_t count = 0;
	char * token = strtok(string, delim);
	array[count++] = token;

	while( token != NULL ) {
	      token = strtok(NULL, delim);
	      array[count++] = token;
	}

	return count;
}
