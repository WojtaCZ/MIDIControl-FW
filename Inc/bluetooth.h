#ifndef bluetooth_h
#define bluetooth_h


#include <stdint.h>
#include "oled.h"
#include "menus.h"

extern struct menuitem bluetoothmenu[];

#define BT_RX_BUFF_SIZE 255
#define BT_AOK "AOK\r\nCMD> "

uint8_t btRxBuff[BT_RX_BUFF_SIZE], btRxIndex, btRxComplete, btRxByte, btRxStatus, btScannedCount;
uint8_t *btTxBuff;

uint8_t bluetoothInit();
uint8_t bluetoothCMD_ACK(char *cmd, char *ack);
uint8_t bluetoothCMD_Until(char *cmd, char *terminator, char *recvBuffer);
uint8_t bluetoothCMD_Time(char *cmd, uint8_t s, char (*recvBuffer)[]);
uint8_t bluetoothGetScannedDevices();
uint32_t countOccurances(char * buff, char * what);
uint32_t replacechar(char *str, char orig, char rep);
uint32_t splitString(char * string, char * delim, char ** array);

struct menuitem btScanedDevices[20];

#endif
