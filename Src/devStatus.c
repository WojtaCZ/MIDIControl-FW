#include <devStatus.h>

extern void setLEDcolor(uint8_t strip, uint32_t LEDnumber, uint8_t RED, uint8_t GREEN, uint8_t BLUE);
extern void setWHOLEcolor(uint8_t strip, uint8_t RED, uint8_t GREEN, uint8_t BLUE);

uint32_t pendingFlags[PERIF_COUNT];
uint32_t statuses[PERIF_COUNT];

//Nastaveni barvy LED podle statusu
void setStatus(uint8_t perif, uint8_t status){
	uint32_t color;

	switch(status){
		case DEV_ERR:
			color = CLR_ERROR;
		break;

		case DEV_OK:
			color = CLR_OK;
		break;

		case DEV_DATA:
			color = CLR_DATA;
		break;

		case DEV_LOAD:
			color = CLR_LOAD;
		break;

		default:
			color = 0;

	}

	//Pokude je typu OK, ERR nebo LOAD - nastavi se na pevne sviceni
	if(status == DEV_OK || status == DEV_ERR || status == DEV_LOAD){
		statuses[perif] = color;
		setLEDcolor((perif >> 7) & 0x01, perif & 0x7f, ((pendingFlags[perif]>>16) & 0xff), ((pendingFlags[perif]>>8) & 0xff), (pendingFlags[perif] & 0xff));
	}else if(status == DEV_DATA){
		//Pokud jde o data, udela se jen probliknuti
		pendingFlags[perif] = color;
	}
}

//Rutina pro nastaveni barvy vsech LED najednou
void setStatusAll(uint8_t strip, uint8_t status){
	uint32_t color;

	switch(status){
		case DEV_ERR:
			color = CLR_ERROR;
		break;

		case DEV_OK:
			color = CLR_OK;
		break;

		case DEV_DATA:
			color = CLR_DATA;
		break;

		case DEV_LOAD:
			color = CLR_LOAD;
		break;

		default:
			color = 0;

	}

	//Rozdeli se na dva "pasky" podle nazvu
	for(uint16_t i = 0; i < PERIF_COUNT; i++){
		if(((i & 0x80)>>7) == strip) statuses[i] = color;
	}

	//Nastavi se barva
	setWHOLEcolor(strip, ((color>>16) & 0xff), ((color>>8) & 0xff), (color & 0xff));

}



//Rutina pro "probliknuti" kdyz jsou na LED nastavena DATA
void proccessPendingStatus(){
	//Projde vsechny LED
	for(uint16_t i = 0; i < PERIF_COUNT; i++){
		if((i & 0x7f) < LED_COUNT){
			if(pendingFlags[i] != 0){
				//Pokud data nebyla nastavena, nastavi barvu a vycisti flag
				setLEDcolor((i >> 7) & 0x01,i & 0x7f, ((pendingFlags[i]>>16) & 0xff), ((pendingFlags[i]>>8) & 0xff), (pendingFlags[i] & 0xff));
				pendingFlags[i] = 0;
			}else{
				//Nastavi zpet puvodni barvu led
				setLEDcolor((i >> 7) & 0x01,i & 0x7f, ((statuses[i]>>16) & 0xff), ((statuses[i]>>8) & 0xff), (statuses[i] & 0xff));
			}
		}
	}

}
