#include "bluetooth.h"
#include "devStatus.h"
#include <stm32g4xx_hal.h>
#include <main.h>
#include <stm32g4xx_hal_uart.h>
#include <usart.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "midiControl.h"

extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);

//ZADOST PRIPOJENI
//AOK{0D}{0A}CMD> %CONNECT,1,699238C2F7D5%%KEY:123456%%CONN_PARAM,0006,0000,01F4%1,699238C2F7D5,1{0D}{0A}END

uint8_t bluetoothInit(){
	btFifoIndex = 0;
	btMsgFifoIndex = 0;
	btCmdMode = 1;
	btStatusMsg = 0;
	btStreamOpen = 0;
	btNullCounter = 0;
	btComMessageFlag = 0;
	btComMessageSizeFlag = 0;
	btMessageLen = 0;

	//Zecne se prijem
	HAL_UART_Receive_IT(&huart2, &btFifoByte, 1);

	HAL_GPIO_WritePin(BT_RST_GPIO_Port, BT_RST_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BT_MODE_GPIO_Port, BT_MODE_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(BT_RST_GPIO_Port, BT_RST_Pin, GPIO_PIN_SET);

	HAL_Delay(100);

	//Zapne se CMD
	if(!bluetoothEnterCMD()) return 0;


	//Dev info a UART
	if(!bluetoothCMD_ACK("GS\r", "C0")){
		if(!bluetoothCMD_ACK("SS,C0\r", BT_AOK)) return 0;
		if(!bluetoothCMD_ACK("R,1\r", "REBOOT")) return 0;
	}

	//Zkontroluje se název
	if(!bluetoothCMD_ACK("GN\r", "MIDIControl")){
		if(!bluetoothCMD_ACK("SN,MIDIControl\r", BT_AOK)) return 0;
	}

	//Nastavi se vyzarovaci vykon
	if(!bluetoothCMD_ACK("SGA,0\r", BT_AOK)) return 0;
	if(!bluetoothCMD_ACK("SGC,0\r", BT_AOK)) return 0;

	//Apperance jako Media Player - Remote je 0180
	if(!bluetoothCMD_ACK("SDA,0280\r", BT_AOK)) return 0;

	//Vyrobce
	if(!bluetoothCMD_ACK("SDN,Vojtech Vosahlo\r", BT_AOK)) return 0;

	//Automaticky potvrdi pin
	if(!bluetoothCMD_ACK("SA,4\r", BT_AOK)) return 0;

	//Vypne CMD
	if(!bluetoothLeaveCMD()) return 0;


	return 1;
}

uint8_t bluetooth_refreshSelfInfo(){
	char buff[30];
	memset(buff, 0, 30);

	if(!bluetoothCMD_Until("GP\r", "\n", &buff)) return 0;

	sprintf(oledHeader, "%s", buff);

	return 1;
}


void bluetoothFifoFlush(){
	memset(btFifo, 0, btFifoIndex);
	btFifoIndex = 0;
}

void bluetoothMsgFifoFlush(){
	memset(btMsgFifo, 0, btMsgFifoIndex);
	btMsgFifoIndex = 0;
}

uint8_t bluetoothEnterCMD(){
	if(!bluetoothCMD_ACK("$$$", "CMD>")) return 0;
	btCmdMode = 1;
	return 1;
}

uint8_t bluetoothLeaveCMD(){
	if(!bluetoothCMD_ACK("---\r", "END")) return 0;
	btCmdMode = 0;
	return 1;
}

uint8_t bluetoothDecodeMsg(){
	char * index = 0;
	if(strstr((char *)btMsgFifo, "%BONDED") != 0){
		btStreamOpen = 1;
	}

	if(strstr((char *)btMsgFifo, "%CONNECT") != 0){
		index = strstr((char *)btMsgFifo, "%CONNECT");
		sscanf((char *)index+9, "%*d,%02X%02X%02X%02X%02X%02X", &btPairReq.mac[0], &btPairReq.mac[1], &btPairReq.mac[2], &btPairReq.mac[3], &btPairReq.mac[4]);
		sprintf(btPairReq.name, "%02X-%02X-%02X-%02X-%02X-%02X", btPairReq.mac[0], btPairReq.mac[1], btPairReq.mac[2], btPairReq.mac[3], btPairReq.mac[4], btPairReq.mac[5]);
	}

	if(strstr((char *)btMsgFifo, "%DISCONNECT") != 0){
		oledType = OLED_MENU;
		btStreamOpen = 0;
	}

	if(strstr((char *)btMsgFifo, "%KEY:") != 0){
		index = strstr((char *)btMsgFifo, "%KEY:");
		sscanf((char *)index+5, "%06ld", &btPairReq.pin);
		oled_setDisplayedSplash(oled_BtDevPairRequestSplash, &btPairReq);
		//sprintf(oledHeader, "%s", index+5);
	}

	if(strstr((char *)btMsgFifo, "%STREAM_OPEN") != 0){
		btStreamOpen = 1;
	}

	if(strstr((char *)btMsgFifo, "%SECURED") != 0){
		btStreamOpen = 1;
	}

	bluetoothMsgFifoFlush();

	return 1;
}

uint8_t bluetoothCMD_ACK(char *cmd, char *ack){

		bluetoothFifoFlush();

		if(strlen(cmd) > 0){
			if(HAL_UART_Transmit_IT(&huart2, (uint8_t*)cmd, strlen(cmd)) != HAL_OK) return 0;
		}

		uint32_t now = HAL_GetTick();
		while(strstr((char *)btFifo, ack) == 0 && HAL_GetTick() - now < BT_TIMEOUT);

		bluetoothFifoFlush();

		if(HAL_GetTick() - now >= BT_TIMEOUT) return 0;

		return 1;

}

uint8_t bluetoothCMD_Until(char *cmd, char *terminator, char (*recvBuffer)[]){

	bluetoothFifoFlush();

	if(strlen(cmd) > 0){
		if(HAL_UART_Transmit_IT(&huart2, (uint8_t*)cmd, strlen(cmd)) != HAL_OK) return 0;
	}

	uint32_t now = HAL_GetTick();
	while(strstr((char *)btFifo, terminator) == 0 && HAL_GetTick() - now < BT_TIMEOUT);

	memcpy(recvBuffer, (char *)btFifo, btFifoIndex-1);

	bluetoothFifoFlush();

	if(HAL_GetTick() - now >= BT_TIMEOUT) return 0;

	return 1;
}

uint8_t bluetoothCMD_Time(char *cmd, uint8_t s, char (*recvBuffer)[]){

	bluetoothFifoFlush();

	if(strlen(cmd) > 0){
		if(HAL_UART_Transmit_IT(&huart2, (uint8_t*)cmd, strlen(cmd)) != HAL_OK) return 0;
	}

	uint32_t now = HAL_GetTick();
	while(HAL_GetTick() - now < (s*1000));

	memcpy(recvBuffer, (char *)btFifo, btFifoIndex-1);

	bluetoothFifoFlush();

	return 1;
}


uint8_t bluetoothConnectKnown(){
	bluetoothGetScannedDevices();
	bluetoothGetBondedDevices();

	int8_t match = -1;

	for(uint8_t b = 0; b < btBondedCount; b++){
		for(uint8_t s = 0; s < btScannedCount; s++){
			match = b;
			for(uint8_t i = 0; i < 6; i++){
				if(btScanned[s].mac[i] != btBonded[b].mac[i]){
					match = -1;
					break;
				}
			}
		}
	}

	if(match != -1){
		return 1;
	}

	return 0;
}

uint8_t bluetoothGetScannedDevices(){

	char buff[300];
	memset(buff, 0, 300);
	//memset(oledHeader, 0, 30);
	//memset(buff, 0, 100);

	if(!bluetoothEnterCMD()) return 0;

	if(!bluetoothCMD_Time("F\r", 15, &buff)){
		if(!bluetoothLeaveCMD()) return 0;
		return 0;
	}


	char *devices[20];
	/*for(int i=0; i < 20; i++){
		devices[i] = (char*)malloc(100);
		memset(devices[i], 0, 100);
	}*/

	btScannedCount = countOccurances(buff, "%")/2;
	replacechar(buff, '\r', ' ');
	splitString(buff, "\n", devices);

	//sprintf(oledHeader, "Count: %d", btScannedCount);


	uint8_t i;

	for(i = 0; i < btScannedCount; i++){
		//Nactou se informace do seznamu zarizeni
		if(strstr((char *)devices[i+1], ",,") - (char *)devices[i+1] < 16 && strstr((char *)devices[i+1], ",,")){
			//Pokud prijde response kde neni jmeno
			sscanf((char *)devices[i+1], "%*[%]%02X%02X%02X%02X%02X%02X,%d,,%[^,],%02X", &btScanned[i].mac[0], &btScanned[i].mac[1], &btScanned[i].mac[2], &btScanned[i].mac[3], &btScanned[i].mac[4], &btScanned[i].mac[5], &btScanned[i].mactype, btScanned[i].uuid, &btScanned[i].rssi);
		    sprintf(btScanned[i].name, "%02X-%02X-%02X-%02X-%02X-%02X", btScanned[i].mac[0], btScanned[i].mac[1], btScanned[i].mac[2], btScanned[i].mac[3], btScanned[i].mac[4], btScanned[i].mac[5]);
		}else if(strstr((char *)devices[i+1], ",,") - (char *)devices[i+1] >= 16){
			//Pokud prijde response bez uuid
			sscanf((char *)devices[i+1], "%*[%]%02X%02X%02X%02X%02X%02X,%d,%[^,],,%02X", &btScanned[i].mac[0], &btScanned[i].mac[1], &btScanned[i].mac[2], &btScanned[i].mac[3], &btScanned[i].mac[4], &btScanned[i].mac[5], &btScanned[i].mactype, btScanned[i].name, &btScanned[i].rssi);
		}else if(strstr((char *)devices[i+1], "Brcst") - (char *)devices[i+1] >= 16){
			//Pokud prijde response s broadcastem
			sscanf((char *)devices[i+1], "%*[%]%02X%02X%02X%02X%02X%02X,%d,%02X,%*[^%]", &btScanned[i].mac[0], &btScanned[i].mac[1], &btScanned[i].mac[2], &btScanned[i].mac[3], &btScanned[i].mac[4], &btScanned[i].mac[5], &btScanned[i].mactype, &btScanned[i].rssi);
			sprintf(btScanned[i].name, "%02X-%02X-%02X-%02X-%02X-%02X", btScanned[i].mac[0], btScanned[i].mac[1], btScanned[i].mac[2], btScanned[i].mac[3], btScanned[i].mac[4], btScanned[i].mac[5]);
		}else{
			//Pokud prijde normalni response
			sscanf((char *)devices[i+1], "%*[%]%02X%02X%02X%02X%02X%02X,%d,%[^,],%[^,],%02X", &btScanned[i].mac[0], &btScanned[i].mac[1], &btScanned[i].mac[2], &btScanned[i].mac[3], &btScanned[i].mac[4], &btScanned[i].mac[5], &btScanned[i].mactype, btScanned[i].name, btScanned[i].uuid, &btScanned[i].rssi);
		}

		btScanedDevices[i].font = &Font_11x18;
		btScanedDevices[i].name = btScanned[i].name;
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

	if(!bluetoothCMD_Until("X\r", BT_AOK, &buff)){
		if(!bluetoothLeaveCMD()) return 0;
		return 0;
	}

	if(!bluetoothLeaveCMD()) return 0;

	return 1;

}

uint8_t bluetoothGetBondedDevices(){

	char buff[300];
	memset(buff, 0, 300);
	//CDC_Transmit_FS("A", 1);

	if(!bluetoothEnterCMD()) return 0;

	if(!bluetoothCMD_Until("LB\r", "END", &buff)){
		if(!bluetoothLeaveCMD()) return 0;
		return 0;
	}

	/*CDC_Transmit_FS(buff, btRxIndex);
	CDC_Transmit_FS("\n", 1);*/

	char *devices[20];
	/*for(int i=0; i < 20; i++){
		devices[i] = (char*)malloc(100);
		memset(devices[i], 0, 100);
	}*/

	btBondedCount = countOccurances(buff, "\n");
	replacechar(buff, '\r', ' ');
	splitString(buff, "\n", devices);

	sprintf(oledHeader, "Count: %d", btBondedCount);

	uint8_t i;

	for(i = 0; i < btBondedCount; i++){
		//Nactou se informace do seznamu zarizeni
		sscanf((char *)devices[i], "%*d,%02X%02X%02X%02X%02X%02X,%d", &btBonded[i].mac[0], &btBonded[i].mac[1], &btBonded[i].mac[2], &btBonded[i].mac[3], &btBonded[i].mac[4], &btBonded[i].mac[5], &btBonded[i].mactype);
		sprintf(btBonded[i].name, "%02X-%02X-%02X-%02X-%02X-%02X", btBonded[i].mac[0], btBonded[i].mac[1], btBonded[i].mac[2], btBonded[i].mac[3], btBonded[i].mac[4], btBonded[i].mac[5]);

		/*CDC_Transmit_FS(btBonded[i].name, strlen(btBonded[i].name));
		CDC_Transmit_FS("\n", 1);*/

		btBondedDevicesMenu[i].font = &Font_11x18;
		btBondedDevicesMenu[i].name = btBonded[i].name;
		//sprintf(btScanedDevices[i].name, "%s", btBonded[i].name);
		btBondedDevicesMenu[i].selected = 0;
		btBondedDevicesMenu[i].hasSpecialSelector = 0;
		btBondedDevicesMenu[i].specharNotSelected = 0;
		btBondedDevicesMenu[i].specharSelected = 0;
		btBondedDevicesMenu[i].submenuLevel = 3;
		btBondedDevicesMenu[i].parentItem = &bluetoothmenu[0].name;


	}


	btBondedDevicesMenu[btBondedCount].font = &Font_11x18;
	btBondedDevicesMenu[btBondedCount].name = "Zpet";
	btBondedDevicesMenu[btBondedCount].selected = 0;
	btBondedDevicesMenu[btBondedCount].hasSpecialSelector = 1;
	btBondedDevicesMenu[btBondedCount].specharNotSelected = 36;
	btBondedDevicesMenu[btBondedCount].specharSelected = 37;
	btBondedDevicesMenu[btBondedCount].submenuLevel = 3;
	btBondedDevicesMenu[btBondedCount].parentItem = 0;

	if(!bluetoothLeaveCMD()) return 0;

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
