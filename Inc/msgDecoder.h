#ifndef msgdecoder_h
#define msgdecoder_h

#include <stdio.h>
#include <stdint.h>

//Typy zprav (7bit = Set / Get)
#define MSG_TYPE_INTERNAL	0xA0
#define MSG_TYPE_BLUETOOTH	0xB0
#define MSG_TYPE_DISPLAY	0xC0

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

void decodeMessage(char *msg, uint32_t len);
void internalMessageDecoder(char *msg, uint32_t len);

#endif
