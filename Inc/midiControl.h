#ifndef midicontrol_h
#define midicontrol_h

#include <stdint.h>

#define MIDI_SEARCHING 0
#define MIDI_A	1
#define MIDI_B	2
//midiStatus = rika jaky MIDI je RX
uint8_t dispStatus, currentStatus, midiStatus, usbStatus;

uint8_t alivePC, aliveRemote, alivePCCounter, aliveRemoteCounter;

//Pomocne promenne pro statusy
uint8_t midiStatusOld;
//Flagy pro "workery"
uint8_t workerBtScanDev, workerBtBondDev;

void midiControl_init();
void midiControl_display_getState();
void midiControl_current_On();
void midiControl_current_Off();
void midiControl_midiIO_init();
uint8_t midiControl_midiIO_getState();
void midiControl_record(uint8_t initiator);
void midiControl_keepalive_process();

#endif
