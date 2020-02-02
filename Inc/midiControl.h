#ifndef midicontrol_h
#define midicontrol_h

#include <stdint.h>
uint8_t dispStatus, currentStatus;

void midiControl_checkDisplay();
void midiControl_currentOn();
void midiControl_currentOff();

#endif
