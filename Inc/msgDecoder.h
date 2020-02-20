#ifndef msgdecoder_h
#define msgdecoder_h

#include <stdio.h>
#include <stdint.h>

//Typy zprav
#define ADDRESS_MAIN 0x02
#define ADDRESS_PC 0x00
#define ADDRESS_CONTROLLER 0x01


#define INTERNAL_COM 0x00
#define INTERNAL_COM_KEEPALIVE 0xAB
#define INTERNAL_COM_STOP 0x00
#define INTERNAL_COM_PLAY 0x01
#define INTERNAL_COM_REC 0x02

#define INTERNAL_DISP 0x01


#define INTERNAL_USB 0x02


#define INTERNAL_CURR 0x03
#define INTERNAL_CURR_ON 0x01
#define INTERNAL_CURR_OFF 0x00

#define INTERNAL_BLUETOOTH 0x04


#define INTERNAL_MIDI 0x05


#define INTERNAL_CHRG 0x05


char decoderBuffer[255];

//Internal messages
//	Periferals
//MIDI
//DISPLAY
//USB
//CURRENT SOURCE
//BLUETOOTH

//	CPU
//VERSION
//
//
//
//
//
//
//

void decodeMessage(char *msg, uint16_t len, uint8_t broadcast);
void msgAOK(uint8_t aokType, uint8_t recType, uint16_t recSize, uint16_t dataSize, char * msg);
void msgERR(uint8_t errType, uint8_t recType, uint16_t recSize);
void sendMsg(uint8_t src, uint8_t dest, uint8_t broadcast, uint8_t type, char * msg, uint16_t len);

#endif
