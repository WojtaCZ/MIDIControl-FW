#ifndef devstatus_h
#define devstatus_h

#include <stdio.h>
#include <stdint.h>

#define LED_COUNT	6
#define PERIF_COUNT	(4 | 0x80)

#define CLR_OK		0x004000
#define CLR_ERROR	0x400000
#define CLR_DATA	0x000040
#define CLR_LOAD	0x404000

#define DEV_ERR 		0
#define DEV_OK 			1
#define DEV_DATA 		2
#define DEV_LOAD 		3
#define DEV_CLR			4

#define DEV_USB				0
#define DEV_DISP			1
#define DEV_CURRENT			2
#define DEV_MIDIA			3
#define DEV_MIDIB			4
#define DEV_BLUETOOTH		5

#define FRONT1				(0 | 0x80)
#define FRONT2				(1 | 0x80)
#define FRONT3				(2 | 0x80)
#define FRONT4				(3 | 0x80)




void setStatus(uint8_t perif, uint8_t status);
void proccessPendingStatus();
void setStatusAll(uint8_t strip, uint8_t status);

#endif
