#ifndef midicontrol_h
#define midicontrol_h

#include <stdint.h>

#define MIDI_SEARCHING 0
#define MIDI_A	1
#define MIDI_B	2

uint8_t dispStatus, currentStatus, midiStatus, usbStatus;

void midiControl_checkDisplay();
void midiControl_currentOn();
void midiControl_currentOff();
void midiControl_midiIO_init();
uint8_t midiControl_midiIO_getState();

#endif
