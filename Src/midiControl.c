#include "midiControl.h"
#include <main.h>
#include "devStatus.h"
#include <usart.h>
#include "oled.h"
#include "msgDecoder.h"
#include "midiControl.h"

//Funkce pro jednodussi ovladani workeru - aktivace
void workerAssert(struct worker * wrk){
	wrk->assert = 1;
	wrk->status = WORKER_WAITING;
}

//Funkce pro jednodussi ovladani workeru - deaktivace
void workerDesert(struct worker * wrk){
	wrk->assert = 0;
	wrk->status = WORKER_WAITING;
}

//Funkce pro vytvoreni menu pisni ze stringu vraceneho z PC
void strToSongMenu(char * str, uint8_t * size){

	//Spocita pocet pisni
	uint8_t items = countOccurances(str, "\n");
	*size = items;
	splitString(str, "\n", songs);

	//Projde je a pro kazdou vytvori zaznam v menu
	for(int i = 0; i < items; i++){
			songMenu[i].name = songs[i];
			songMenu[i].font = &Font_11x18;
			songMenu[i].selected = 0;
			songMenu[i].hasSpecialSelector = 0;
			songMenu[i].specharNotSelected = 0;
			songMenu[i].specharSelected = 0;
			songMenu[i].submenuLevel = 3;
			songMenu[i].parentItem = &bluetoothmenu[0].name;
		}

	//Vytvori tlacitko zpet
	songMenu[items].font = &Font_11x18;
	songMenu[items].name = "Zpet";
	songMenu[items].selected = 0;
	songMenu[items].hasSpecialSelector = 1;
	songMenu[items].specharNotSelected = 36;
	songMenu[items].specharSelected = 37;
	songMenu[items].submenuLevel = 3;
	songMenu[items].parentItem = 0;

}



void midiControl_init(){
	//Inicializace promennych pro chod zarizeni
	dispStatus = -1;
	currentStatus = -1;
	alivePC = 0;
	aliveRemote = 0;
	alivePCCounter = 0;
	aliveRemoteCounter = 0;
	workerDesert(&workerBtConnect);
	workerDesert(&workerBtScanBondable);
	workerDesert(&workerBtRemoveController);
}

//Rutina pro kontrolu stavu displeje a zobrazeni na LED
void midiControl_display_getState(){
	//Zkontroluje se stav stavoveho pinu
	if(dispStatus != !HAL_GPIO_ReadPin(DISP_SENSE_GPIO_Port, DISP_SENSE_Pin)){
		dispStatus = !HAL_GPIO_ReadPin(DISP_SENSE_GPIO_Port, DISP_SENSE_Pin);
		//Nastavi se LED
		if(dispStatus){
			setStatus(DEV_DISP, DEV_OK);
		}else{
			setStatus(DEV_DISP, DEV_ERR);
		}
	}

}

//Rutina pro zapnuti proudoveho zdroje
void midiControl_current_On(){
	if(!HAL_GPIO_ReadPin(CURRENT_SOURCE_GPIO_Port, CURRENT_SOURCE_Pin)){
			HAL_GPIO_WritePin(CURRENT_SOURCE_GPIO_Port, CURRENT_SOURCE_Pin, GPIO_PIN_SET);
			setStatus(DEV_CURRENT, DEV_OK);
	}
}

//Rutina pro vypnuti proudoveho zdroje
void midiControl_current_Off(){
	if(HAL_GPIO_ReadPin(CURRENT_SOURCE_GPIO_Port, CURRENT_SOURCE_Pin)){
			HAL_GPIO_WritePin(CURRENT_SOURCE_GPIO_Port, CURRENT_SOURCE_Pin, GPIO_PIN_RESET);
			setStatus(DEV_CURRENT, DEV_ERR);
	}
}

//Rutina pro zjisteni stavu MIDI rozhrani
uint8_t midiControl_midiIO_getState(){
	uint8_t retval;

	//Stavy pinu se prevedou na stav periferie
	if(HAL_GPIO_ReadPin(MIDI_SEARCHING_GPIO_Port, MIDI_SEARCHING_Pin)){
		retval = MIDI_SEARCHING;
	}else if((!HAL_GPIO_ReadPin(MIDI_SEARCHING_GPIO_Port, MIDI_SEARCHING_Pin)) & (!HAL_GPIO_ReadPin(MIDI_IO_SELECTED_GPIO_Port, MIDI_IO_SELECTED_Pin))){
		retval = MIDI_A;
	}else if((!HAL_GPIO_ReadPin(MIDI_SEARCHING_GPIO_Port, MIDI_SEARCHING_Pin)) & (HAL_GPIO_ReadPin(MIDI_IO_SELECTED_GPIO_Port, MIDI_IO_SELECTED_Pin))){
		retval = MIDI_B;
	}else{
		retval = MIDI_SEARCHING;
	}

	//Pokud se stav zmenil, nastavi se barvy LED
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

	//Neimplementovano

}

//Rutina pro spusteni nahravani
void midiControl_record(uint8_t initiator, char * songname){
	//Spusteno z PC
	if(initiator == ADDRESS_PC){
		//Jen se zobrazi obrazovka nahravani
		oled_setDisplayedSplash(oled_recordingSplash, songname);
	}else if(initiator == ADDRESS_CONTROLLER){
	//Spusteno ovladacem
		//Jen se zobrazi obrazovka nahravani
		oled_setDisplayedSplash(oled_recordingSplash, songname);
	}else if(initiator == ADDRESS_MAIN){
	//Spusteno ze zakladnove stanice
		//Posle se zprava do PC aby zacalo nahravat
		char msg[100];
		msg[0] = INTERNAL_COM;
		msg[1] = INTERNAL_COM_REC;
		memcpy(&msg[2], songname, strlen(songname));
		sendMsg(ADDRESS_MAIN, ADDRESS_PC, 0, INTERNAL, msg, strlen(songname)+2);
	}


}

void midiControl_play(uint8_t initiator, char * songname){
	//Spusteno z PC
	if(initiator == 0x00){
		memset(selectedSong, 0, 40);
		sprintf(selectedSong, "%s", songname);
		//Jen se zobrazi obrazovka prehravani
		oled_setDisplayedSplash(oled_playingSplash, songname);
	}else if(initiator == 0x01){
	//Spusteno ovladacem
		//Jen se zobrazi obrazovka prehravani
		oled_setDisplayedSplash(oled_playingSplash, songname);
	}else if(initiator == 0x02){
	//Spusteno ze zakladnove stanice
		//Posle se zprava do PC aby zacalo prehravat
		char msg[100];
		msg[0] = INTERNAL_COM;
		msg[1] = INTERNAL_COM_PLAY;
		memcpy(&msg[2], songname, strlen(songname));
		sendMsg(ADDRESS_MAIN, ADDRESS_PC, 0, INTERNAL, msg, strlen(songname)+2);
	}


}

void midiControl_stop(uint8_t initiator){
	//Spusteno z hlavni jednotky
	if(initiator == ADDRESS_MAIN){
		//Posle se zprava do PC o zastaveni
		char msg[2] = {INTERNAL_COM, INTERNAL_COM_STOP};
		sendMsg(ADDRESS_MAIN, ADDRESS_PC, 0, INTERNAL, msg, 2);
	}else{
		//Vrati se do menu, zapne OLED refresh a vypne LED
		oledType = OLED_MENU;
		oled_refreshResume();
		setStatusAll(1, DEV_CLR);
	}

}


//Rutina pro kontrolu pripojeni PC a ovladace a odesilani info o "zijici" hlavni jednotce
void midiControl_keepalive_process(){
	//Pricita citace - jak dlouho nedostal odpoved
	alivePCCounter++;
	aliveRemoteCounter++;

	//Pokud nedostal odpoved za 3s, neni PC pripojeno
	if(alivePCCounter > 6){
		alivePC = 0;
		alivePCCounter = 0;
	}

	//Pokud nedostal odpoved za 3s, neni ovladac pripojen
	if(aliveRemoteCounter > 6){
		aliveRemote = 0;
		aliveRemoteCounter = 0;
	}

}



//Rutina pro nastaveni dat zobrazenych na displeji
uint8_t midiControl_setDisplay(uint16_t cislo_pisne, uint8_t cislo_sloky, uint8_t barva, uint8_t napev){
	uint8_t data[9];
	//Asi nejaky kontrolni znak, vzdy stejny
	data[0] = 176;
	//Jednotky sloky
	data[1] = cislo_sloky-(uint8_t)(cislo_sloky/10)*10;
	//Desitky sloky
	data[2] = (uint8_t)(cislo_sloky/10);
	//Jednotky pisne
	data[3] = cislo_pisne - (uint8_t)(cislo_pisne/10)*10;
	//Desitky pisne
	data[4] = (uint8_t)(cislo_pisne/10) - (uint8_t)(cislo_pisne/100);
	//Stovky pisne
	data[5] = (uint8_t)(cislo_pisne/100) - (uint8_t)(cislo_pisne/1000);
	//Tisice pisne
	data[6] = (uint8_t)(cislo_pisne/1000);
	//Pismeno
	data[7] = napev-55;
	//Barva
	data[8] = barva;

	midiControl_setDisplayRaw(data, 9);
}

uint8_t midiControl_setDisplayRaw(uint8_t * data, uint16_t len){
	HAL_UART_Transmit_IT(&huart1, data, len);
}

void midiControl_get_time(){
	//Odesle zadost o nastaveni casu
	char msg[] = {INTERNAL_COM, INTERNAL_COM_GET_TIME};
	sendMsg(ADDRESS_MAIN, ADDRESS_PC, 0, INTERNAL, msg, 2);
}
