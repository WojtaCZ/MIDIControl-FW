#include "msgDecoder.h"
#include "midiControl.h"
#include "oled.h"
#include "usart.h"
#include <stdlib.h>
#include "devStatus.h"
#include "midiControl.h"

extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);


//Rutina pro dekodovani zprav komunikacniho protokolu zarizeni
void decodeMessage(char * msg, uint16_t len, uint8_t broadcast){
	//Internal
	char msgType = msg[6];

	uint8_t src = ((msg[6] & 0x18) >> 3);
	uint8_t type = ((msgType & 0xE0) >> 5);


	if(type == INTERNAL){
		if(msg[7] == INTERNAL_COM){
			if(msg[8] == INTERNAL_COM_PLAY){
				msg[len] = 0;
				midiControl_play(src, &msg[9]);
			}else if(msg[8] == INTERNAL_COM_STOP){
				midiControl_stop(src);
			}else if(msg[8] == INTERNAL_COM_REC){
				midiControl_record(src, &msg[9]);
			}else if(msg[8] == INTERNAL_COM_KEEPALIVE){
				if(src == ADDRESS_CONTROLLER){
					aliveRemote = 1;
					aliveRemoteCounter = 0;
				}else if(src == ADDRESS_PC){
					alivePC = 1;
					alivePCCounter = 0;
				}
			}else msgERR(0, msgType, len);
		}else if(msg[7] == INTERNAL_CURR){
			if(msg[8] == INTERNAL_CURR_ON){
				midiControl_current_On();
				msgAOK(0, msgType, len, 0, NULL);
			}else if(msg[8] == INTERNAL_CURR_OFF){
				midiControl_current_Off();
				msgAOK(0, msgType, len, 0, NULL);
			}else msgERR(0, msgType, len);

		}else if(msg[7] == INTERNAL_DISP){

		}else msgERR(0, msgType, len);
	}else if(type == EXTERNAL_DISP){
		midiControl_setDisplayRaw((uint8_t*)&msg[7], len-7);
	}else if(type == AOKERR){
		if((msg[7] & 0x80) == AOK){

			//Pokud se jedna o odpoved na zpravu z hl. jednotky do PC
			if(msg[8] == 0x30){
				if(workerGetSongs.assert && workerGetSongs.status == WORKER_WAITING){
					workerGetSongs.status = WORKER_OK;
					strToSongMenu(&msg[11], &songMenuSize);
				}
			}
		}else if((msg[7] & 0x80) == ERR){
			if(workerGetSongs.assert && workerGetSongs.status == WORKER_WAITING){
				workerGetSongs.status = WORKER_ERR;
			}
		}
	}else{
		msgERR(0, msgType, len);
	}
}


//Odesle zpravu typu AOK
void msgAOK(uint8_t aokType, uint8_t recType, uint16_t recSize, uint16_t dataSize, char * msg){
	char * buffer = (char*)malloc(dataSize);
	//Utvori se AOK znak s typem
	buffer[0] = 0x80 | (aokType & 0x7f);
	buffer[1] = recType;
	buffer[2] = ((recSize-6) & 0xff00) >> 8;
	buffer[3] = (recSize-6) & 0xff;
	memcpy(&buffer[4], msg, dataSize);
	sendMsg(ADDRESS_MAIN, ((recType & 0x18) >> 3), 0, 0x07, buffer, dataSize+5);
	free(buffer);
}

//Odesle zpravu typu ERR
void msgERR(uint8_t errType, uint8_t recType, uint16_t recSize){
	char * buffer = (char*)malloc(5);
	//Utvori se ERR znak s typem
	buffer[0] = 0x7f & (errType & 0x7f);
	buffer[1] = recType;
	buffer[2] = ((recSize-6) & 0xff00) >> 8;
	buffer[3] = (recSize-6) & 0xff;

	sendMsg(ADDRESS_MAIN, ((recType & 0x18) >> 3), 0, 0x07, buffer, 5);
	free(buffer);
}

//Odesle libovolnou zpravu
void sendMsg(uint8_t src, uint8_t dest, uint8_t broadcast, uint8_t type, char * msg, uint16_t len){
	uint8_t * buffer = (uint8_t*)malloc(len+7);
	buffer[0] = 0;
	buffer[1] = 0;
	buffer[2] = 0;
	buffer[3] = 0;
	buffer[4] = ((len+1) >> 4) & 0xff;
	buffer[5] = (len+1) & 0xff;
	buffer[6] = ((type & 0x07) << 5) | ((src & 0x3) << 3) | ((broadcast & 0x01) << 2) | (dest & 0x03);
	memcpy(&buffer[7], msg, len);

	//Podle cile ji odesle na ruzne rozhrani
	if(broadcast){
		CDC_Transmit_FS(buffer, len+7);
		if(btStreamOpen && !btCmdMode) HAL_UART_Transmit_IT(&huart2, buffer, len+7);
	}else if(dest == ADDRESS_PC){
		CDC_Transmit_FS(buffer, len+7);
	}else if(dest == ADDRESS_CONTROLLER && btStreamOpen && !btCmdMode){
		HAL_UART_Transmit_IT(&huart2, buffer, len+7);
	}

}

