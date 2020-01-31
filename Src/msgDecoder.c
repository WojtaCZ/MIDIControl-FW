#include "msgDecoder.h"

void decodeMessage(char *msg, uint32_t len){
	switch(msg[0] & 0xBF){
		case MSG_TYPE_INTERNAL:

		break;

		case MSG_TYPE_BLUETOOTH:

		break;

		case MSG_TYPE_DISPLAY:

		break;
	}
}


void internalMessageDecoder(char *msg, uint32_t len){

}
