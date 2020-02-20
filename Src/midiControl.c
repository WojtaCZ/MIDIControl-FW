#include "midiControl.h"
#include <main.h>
#include "devStatus.h"


void midiControl_init(){
	dispStatus = -1;
	currentStatus = -1;
	alivePC = 0;
	aliveRemote = 0;
	alivePCCounter = 0;
	aliveRemoteCounter = 0;
}

void midiControl_display_getState(){
	if(dispStatus != !HAL_GPIO_ReadPin(DISP_SENSE_GPIO_Port, DISP_SENSE_Pin)){
		dispStatus = !HAL_GPIO_ReadPin(DISP_SENSE_GPIO_Port, DISP_SENSE_Pin);
		if(dispStatus){
			setStatus(DEV_DISP, DEV_OK);
		}else{
			setStatus(DEV_DISP, DEV_ERR);
		}
	}

}

void midiControl_current_On(){
	if(!HAL_GPIO_ReadPin(CURRENT_SOURCE_GPIO_Port, CURRENT_SOURCE_Pin)){
			HAL_GPIO_WritePin(CURRENT_SOURCE_GPIO_Port, CURRENT_SOURCE_Pin, GPIO_PIN_SET);
			setStatus(DEV_CURRENT, DEV_OK);
	}
}

void midiControl_current_Off(){
	if(HAL_GPIO_ReadPin(CURRENT_SOURCE_GPIO_Port, CURRENT_SOURCE_Pin)){
			HAL_GPIO_WritePin(CURRENT_SOURCE_GPIO_Port, CURRENT_SOURCE_Pin, GPIO_PIN_RESET);
			setStatus(DEV_CURRENT, DEV_ERR);
	}
}


uint8_t midiControl_midiIO_getState(){
	uint8_t retval;

	if(HAL_GPIO_ReadPin(MIDI_SEARCHING_GPIO_Port, MIDI_SEARCHING_Pin)){
		retval = MIDI_SEARCHING;
	}else if((!HAL_GPIO_ReadPin(MIDI_SEARCHING_GPIO_Port, MIDI_SEARCHING_Pin)) & (!HAL_GPIO_ReadPin(MIDI_IO_SELECTED_GPIO_Port, MIDI_IO_SELECTED_Pin))){
		retval = MIDI_A;
	}else if((!HAL_GPIO_ReadPin(MIDI_SEARCHING_GPIO_Port, MIDI_SEARCHING_Pin)) & (HAL_GPIO_ReadPin(MIDI_IO_SELECTED_GPIO_Port, MIDI_IO_SELECTED_Pin))){
		retval = MIDI_B;
	}else{
		retval = MIDI_SEARCHING;
	}

	if(retval != midiStatusOld){
		if(retval != MIDI_SEARCHING){
			setStatus(DEV_MIDIA, DEV_OK);
			setStatus(DEV_MIDIB, DEV_OK);
		}else{
			setStatus(DEV_MIDIA, DEV_ERR);
			setStatus(DEV_MIDIB, DEV_ERR);
		}

		midiStatusOld = retval;
		midiStatus = retval;
	}

	return retval;
}

void midiControl_midiIO_init(){

}

void midiControl_record(uint8_t initiator){
	//Spusteno z PC
	if(initiator == 0x00){
		//sendMessage();
	}else if(initiator == 0x01){
	//Spusteno ovladacem

	}else if(initiator == 0x02){
	//Spusteno ze zakladnove stanice

	}
}

midiControl_keepalive_process(){
	alivePCCounter++;
	aliveRemoteCounter++;

	if(alivePCCounter > 4){
		alivePC = 0;
		alivePCCounter = 0;
	}

	if(aliveRemoteCounter > 4){
		aliveRemote = 0;
		aliveRemoteCounter = 0;
	}
}
