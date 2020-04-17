#ifndef midicontrol_h
#define midicontrol_h

#include <stdint.h>
#include <oled.h>

#define MIDI_SEARCHING 0
#define MIDI_A	1
#define MIDI_B	2


uint16_t midiFifoIndex;
uint8_t midiFifo[500], uartMsgDecodeBuff[300];

//midiStatus = rika jaky MIDI je RX
uint8_t dispStatus, dispConnected, currentStatus, midiStatus, usbStatus;
int8_t btDataIcon;

uint8_t alivePC, aliveRemote, alivePCCounter, aliveRemoteCounter;

//Promenne pro ukazatel
uint8_t dispData[9];
uint8_t dispVerse[2], dispLetter, dispLED, dispLEDOld;
uint16_t dispSong[4];

struct reqValue numDispSong, numDispVerse, numDispLetter, numRecordSong;

#define DISP_LED_RED 	0
#define DISP_LED_GREEN	2
#define DISP_LED_BLUE	3
#define DISP_LED_YELLOW 1
#define DISP_LED_CLEAR	0xe0

//Pomocne promenne pro statusy
uint8_t midiStatusOld;

struct menuitem songMenu[100];

uint8_t songMenuSize;
char *songs[100];
char selectedSong[40];

#define WORKER_WAITING	0x00
#define WORKER_ERR		0x01
#define WORKER_OK		0x02
#define WORKER_REQUEST	0x03

struct worker{
	uint8_t assert;
	uint8_t status;
};


//Flagy pro "workery"
struct worker workerBtScanDev, workerBtBondDev, workerBtConnect, workerBtBond, workerBtScanBondable, workerBtRemoveController, workerGetSongs, workerMiscellaneous, workerDispRefresh, workerRecord;
char * workerBtConnectMAC;


void workerAssert(struct worker * wrk);
void workerDesert(struct worker * wrk);

void midiControl_init();
void midiControl_display_getState();
void midiControl_current_On();
void midiControl_current_Off();
void midiControl_midiIO_init();
uint8_t midiControl_midiIO_getState();
void midiControl_record(uint8_t initiator, char * songname);
void midiControl_stop(uint8_t initiator);
void midiControl_play(uint8_t initiator, char * songname);
void midiControl_keepalive_process();
void midiControl_refreshDisplayData();
uint8_t midiControl_setDisplay(uint16_t cislo_pisne, uint8_t cislo_sloky, uint8_t barva, uint8_t napev);
uint8_t midiControl_setDisplayRaw(uint8_t * data, uint16_t len);
void midiControl_get_time();
void midiControl_setDisplaySong(uint16_t song);

void strToSongMenu(char * str, uint8_t * size);

#endif
