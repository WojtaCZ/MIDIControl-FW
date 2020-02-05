#include "midiControl.h"
#include <main.h>
#include "devStatus.h"

void midiControl_checkDisplay(){
	if(dispStatus != !HAL_GPIO_ReadPin(DISP_SENSE_GPIO_Port, DISP_SENSE_Pin)){
		dispStatus = !HAL_GPIO_ReadPin(DISP_SENSE_GPIO_Port, DISP_SENSE_Pin);
		if(dispStatus){
			setStatus(DEV_DISP, DEV_OK);
		}else{
			setStatus(DEV_DISP, DEV_ERR);
		}
	}

}

void midiControl_currentOn(){
	if(!HAL_GPIO_ReadPin(CURRENT_SOURCE_GPIO_Port, CURRENT_SOURCE_Pin)){
			HAL_GPIO_WritePin(CURRENT_SOURCE_GPIO_Port, CURRENT_SOURCE_Pin, GPIO_PIN_SET);
			setStatus(DEV_CURRENT, DEV_OK);
	}
}

void midiControl_currentOff(){
	if(HAL_GPIO_ReadPin(CURRENT_SOURCE_GPIO_Port, CURRENT_SOURCE_Pin)){
			HAL_GPIO_WritePin(CURRENT_SOURCE_GPIO_Port, CURRENT_SOURCE_Pin, GPIO_PIN_RESET);
			setStatus(DEV_CURRENT, DEV_ERR);
	}
}


uint8_t midiControl_midiIO_getState(){
	if(HAL_GPIO_ReadPin(MIDI_SEARCHING_GPIO_Port, MIDI_SEARCHING_Pin)){
		return MIDI_SEARCHING;
	}else if((!HAL_GPIO_ReadPin(MIDI_SEARCHING_GPIO_Port, MIDI_SEARCHING_Pin)) & (!HAL_GPIO_ReadPin(MIDI_IO_SELECTED_GPIO_Port, MIDI_IO_SELECTED_Pin))){
		return MIDI_A;
	}else if((!HAL_GPIO_ReadPin(MIDI_SEARCHING_GPIO_Port, MIDI_SEARCHING_Pin)) & (HAL_GPIO_ReadPin(MIDI_IO_SELECTED_GPIO_Port, MIDI_IO_SELECTED_Pin))){
		return MIDI_B;
	}

	return MIDI_SEARCHING;
}

void midiControl_midiIO_init(){

}
