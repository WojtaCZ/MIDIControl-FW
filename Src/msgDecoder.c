#include "msgDecoder.h"
#include "midiControl.h"
#include "oled.h"
#include "usart.h"
#include <stdlib.h>
#include "devStatus.h"
#include "midiControl.h"
#include <stm32g4xx_hal_rtc.h>
#include <rtc.h>

extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);


//Rutina pro dekodovani zprav komunikacniho protokolu zarizeni
void decodeMessage(char * msg, uint8_t broadcast){
	//Internal
	char msgType = msg[6];

	uint8_t src = ((msg[6] & 0x18) >> 3);
	uint8_t type = ((msgType & 0xE0) >> 5);

	uint16_t len = (msg[4]<<8 | msg[5]);

	if(src == ADDRESS_CONTROLLER) btData = 1;

	if(type == INTERNAL){
		if(msg[7] == INTERNAL_COM){
			if(msg[8] == INTERNAL_COM_PLAY){
				char * buffPlay = (char*) malloc(50);
				memset(buffPlay, 0, 50);
				memcpy(buffPlay, msg+9, len-3);
				midiControl_play(src, buffPlay);
			}else if(msg[8] == INTERNAL_COM_STOP){
				midiControl_stop(src);
			}else if(msg[8] == INTERNAL_COM_REC){
				char * buffRec = (char*) malloc(50);
				memset(buffRec, 0, 50);
				memcpy(buffRec, msg+9, len-3);
				midiControl_record(src, buffRec);
			}else if(msg[8] == INTERNAL_COM_KEEPALIVE){
				if(src == ADDRESS_CONTROLLER){
					aliveRemote = 1;
					btStreamOpen = 1;
					btStreamSecured = 1;
					btStreamBonded = 1;
					aliveRemoteCounter = 0;
				}else if(src == ADDRESS_PC){
					alivePC = 1;
					alivePCCounter = 0;
				}
			}else if(msg[8] == INTERNAL_COM_SET_TIME){
				RTC_TimeTypeDef time;
				RTC_DateTypeDef date;

				time.Seconds = msg[9];
				time.Minutes = msg[10];
				time.Hours = msg[11];
				time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
				time.SecondFraction = 0;
				time.SubSeconds = 0;
				time.TimeFormat = RTC_HOURFORMAT_24;
				time.StoreOperation = RTC_STOREOPERATION_RESET;

				date.WeekDay = RTC_WEEKDAY_MONDAY;
				date.Date = msg[12];
				date.Month = msg[13];
				date.Year = msg[14];

				HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
				HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);

			}else msgERR(0, msgType, len);
		}else if(msg[7] == INTERNAL_CURR){
			if(msg[8] == INTERNAL_CURR_SET_STATUS){
				if(msg[9]){
					midiControl_current_On();
				}else{
					midiControl_current_Off();
				}

				msgAOK(0, msgType, len, 0, NULL);
			}else if(msg[8] == INTERNAL_CURR_GET_STATUS){
				uint8_t currentStatus = HAL_GPIO_ReadPin(CURRENT_SOURCE_GPIO_Port, CURRENT_SOURCE_Pin);
				msgAOK(0, msgType, len, 1, (char*)&currentStatus);
			}else msgERR(0, msgType, len);

		}else if(msg[7] == INTERNAL_MIDI){
			if(msg[8] == INTERNAL_MIDI_GET_STATUS){
				msgAOK(0, msgType, len, 1, (char*)&midiStatus);
			}else if(msg[8] == INTERNAL_CURR_GET_STATUS){
				uint8_t currentStatus = HAL_GPIO_ReadPin(CURRENT_SOURCE_GPIO_Port, CURRENT_SOURCE_Pin);
				msgAOK(0, msgType, len, 1, (char*)&currentStatus);
			}else msgERR(0, msgType, len);

		}else if(msg[7] == INTERNAL_BT){
			if(msg[8] == INTERNAL_BT_GET_STATUS){
				uint8_t stat = (btStreamOpen << 1) | bluetoothStatus;
				msgAOK(0, msgType, len, 1, (char*)&stat);
			}else msgERR(0, msgType, len);

		}else if(msg[7] == INTERNAL_DISP){
			if(msg[8] == INTERNAL_DISP_SET_SONG){
				numDispSong.enteredValue[0] = msg[9];
				numDispSong.enteredValue[1] = msg[10];
				numDispSong.enteredValue[2] = msg[11];
				numDispSong.enteredValue[3] = msg[12];
				workerAssert(&workerDispRefresh);
				msgAOK(0, msgType, len, 0, NULL);
			}else if(msg[8] == INTERNAL_DISP_SET_VERSE){
				numDispVerse.enteredValue[0] = msg[9];
				numDispVerse.enteredValue[1] = msg[10];
				workerAssert(&workerDispRefresh);
				msgAOK(0, msgType, len, 0, NULL);
			}else if(msg[8] == INTERNAL_DISP_SET_LETTER){
				numDispLetter.enteredValue[0] = msg[9];
				workerAssert(&workerDispRefresh);
				msgAOK(0, msgType, len, 0, NULL);
			}else if(msg[8] == INTERNAL_DISP_SET_LED){
				dispLED = msg[9];
				workerAssert(&workerDispRefresh);
				msgAOK(0, msgType, len, 0, NULL);
			}else if(msg[8] == INTERNAL_DISP_GET_STATUS){
				char msg[] = {dispConnected, dispSong[3], dispSong[2], dispSong[1], dispSong[0], dispVerse[1], dispVerse[0], dispLetter, (dispLED > 3) ? 0xe0 : dispLED};
				msgAOK(0, msgType, len, 9, msg);
			}else msgERR(0, msgType, len);
		}else if(msg[7] == INTERNAL_USB){
			if(msg[8] == INTERNAL_USB_GET_CONNECTED){
				msgAOK(0, msgType, len, 1, (char*)&usbStatus);
			}else msgERR(0, msgType, len);

		}else msgERR(0, msgType, len);
	}else if(type == EXTERNAL_DISP){
		midiControl_setDisplayRaw((uint8_t*)&msg[7], len-1);
	}else if(type == EXTERNAL_BT){
		HAL_UART_Transmit_IT(&huart2, (uint8_t*)&msg[7], len-1);
	}else if(type == EXTERNAL_MIDI){
		HAL_UART_Transmit_IT(&huart1, (uint8_t*)&msg[7], len-1);
	}else if(type == EXTERNAL_USB){
		CDC_Transmit_FS((uint8_t*)&msg[7], len-1);
	}else if(type == AOKERR){
		if((msg[7] & 0x80) == AOK){

			//Pokud se jedna o odpoved na zpravu z PC do hl. jednotky
			if(msg[8] == 0x30){
				if(workerGetSongs.assert && workerGetSongs.status == WORKER_WAITING){
					workerGetSongs.status = WORKER_OK;
					strToSongMenu(&msg[11], &songMenuSize);
				}

				if(workerRecord.assert && workerRecord.status == WORKER_WAITING){
					if(msg[11] == 1){
						workerRecord.status = WORKER_ERR;
					}else{
						workerRecord.status = WORKER_OK;
					}
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
	sendMsg(ADDRESS_MAIN, ((recType & 0x18) >> 3), 0, 0x07, buffer, dataSize+4);
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

	sendMsg(ADDRESS_MAIN, ((recType & 0x18) >> 3), 0, 0x07, buffer, 4);
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
		if(btStreamOpen && !btCmdMode){
			HAL_UART_Transmit_IT(&huart2, buffer, len+7);
			btData = 1;
		}
	}else if(dest == ADDRESS_PC){
		CDC_Transmit_FS(buffer, len+7);
	}else if(dest == ADDRESS_CONTROLLER && btStreamOpen && !btCmdMode){
		HAL_UART_Transmit_IT(&huart2, buffer, len+7);
		btData = 1;
	}

}

