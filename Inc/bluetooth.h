#ifndef bluetooth_h
#define bluetooth_h


#include <stdint.h>
#include "oled.h"
#include "menus.h"

extern struct menuitem bluetoothmenu[];

#define BT_AOK "AOK"
#define BT_CMD "CMD>"
#define BT_END "END\r\n"
#define BT_TIMEOUT 2000

#define BT_STATUS_ERR 	0x00
#define BT_STATUS_OK	0x01

uint16_t btFifoIndex, btMsgFifoIndex, btComMessageStartIndex, btMessageLen, btStatusMsgWD;
uint8_t btFifo[500], btFifoByte, btMsgFifo[500];
uint8_t bluetoothStatus, btScannedCount, btBondedCount, btBondableCount, btCmdMode, btStatusMsg, btStreamOpen, btStreamSecured, btStreamBonded, btNullCounter, btComMessageFlag, btComMessageSizeFlag, btMsgReceivedFlag, btSelectedController, btSelectedBondDevice, btPairing;
uint8_t *btTxBuff;

uint8_t bluetoothInit();
uint8_t bluetoothCMD_ACK(char *cmd, char *ack);
uint8_t bluetoothCMD_Until(char *cmd, char *terminator, char (*recvBuffer)[]);
uint8_t bluetoothCMD_Time(char *cmd, uint8_t s, char (*recvBuffer)[]);
uint8_t bluetoothGetScannedDevices();
uint8_t bluetoothGetBondedDevices();
uint8_t bluetooth_refreshSelfInfo();
uint32_t countOccurances(char * buff, char * what);
uint32_t replacechar(char *str, char orig, char rep);
uint32_t splitString(char * string, char * delim, char ** array);
uint8_t bluetoothConnectKnown();
uint8_t bluetoothConnect(char * mac);
uint8_t bluetoothBond();
void bluetoothFifoFlush();
void bluetoothMsgFifoFlush();
uint8_t bluetoothEnterCMD();
uint8_t bluetoothLeaveCMD();
uint8_t bluetoothDecodeMsg();
uint8_t bluetoothGetBondableDevices();

struct menuitem btScanedDevices[20];
struct menuitem btBondedDevicesMenu[10];
struct menuitem btBondableDevices[20];

struct btDevice{
	char name[50];
	unsigned int mac[6];
	unsigned int mactype;
	char* uuid;
	signed char rssi;
	long int pin;
};

struct btDevice btModule;
struct btDevice btPairReq;
struct btDevice btScanned[20];
struct btDevice btBondable[20];
struct btDevice btBonded[10];


#endif
