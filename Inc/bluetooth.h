#ifndef bluetooth_h
#define bluetooth_h

#include <stdio.h>
#include <stdint.h>

#define BT_RX_BUFF_SIZE 255
#define BT_AOK "AOK\r\nCMD> "

uint8_t btRxBuff[BT_RX_BUFF_SIZE], btRxIndex, btRxComplete, btRxByte;
uint8_t *btTxBuff;

uint8_t bluetoothInit();
uint8_t bluetoothCMD_ACK(char *cmd, char *ack);

#endif
