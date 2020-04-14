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
uint8_t dispStatus, currentStatus, midiStatus, usbStatus;

uint8_t alivePC, aliveRemote, alivePCCounter, aliveRemoteCounter;

uint8_t dispData[9];

//Pomocne promenne pro statusy
uint8_t midiStatusOld;

struct menuitem songMenu[100];

uint8_t songMenuSize;
char *songs[100];
char selectedSong[40];

#define WORKER_WAITING	0x00
#define WORKER_ERR		0x01
#define WORKER_OK		0x02

struct worker{
	uint8_t assert;
	uint8_t status;
};


//Flagy pro "workery"
struct worker workerBtScanDev, workerBtBondDev, workerBtConnect, workerBtBond, workerBtScanBondable, workerBtRemoveController, workerGetSongs, workerMiscellaneous;
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
uint8_t midiControl_setDisplay(uint16_t cislo_pisne, uint8_t cislo_sloky, uint8_t barva, uint8_t napev);
uint8_t midiControl_setDisplayRaw(uint8_t * data, uint16_t len);
void midiControl_get_time();

void strToSongMenu(char * str, uint8_t * size);

#endif
