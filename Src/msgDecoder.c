#include "msgDecoder.h"
#include "midiControl.h"
#include "oled.h"

extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);

void decodeMessage(char *msg, uint32_t len){
	sprintf(oledHeader, "%s", msg);

	switch(msg[0]){
		case MSG_TYPE_INTERNAL:
			internalMessageDecoder(msg, len);
		break;

		case MSG_TYPE_BLUETOOTH:

		break;

		case MSG_TYPE_DISPLAY:

		break;
	}

	uint8_t answer[] = {0, 0, 0, 0, msg[0], 0x01};
	CDC_Transmit_FS(answer, sizeof(answer));
}


void internalMessageDecoder(char *msg, uint32_t len){
	//Current source
	if(msg[1] == 0x03){
		if(msg[2] == 0x00){
			midiControl_currentOff();
		}else if(msg[2] == 0x01){
			midiControl_currentOn();
		}
	}
}


