#include "oled.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stm32g4xx_hal_rtc.h>
#include <rtc.h>
#include "bluetooth.h"
#include "midiControl.h"
#include "devStatus.h"
#include "msgDecoder.h"
#include <math.h>


//Jednotliva MENU

struct menuitem mainmenu[] = {
		{"Prehraj", 0, &Font_11x18, 0, 0, 0, 0, 0/*, 0, 0*/},
		{"Nahraj", 0, &Font_11x18, 0, 0, 0, 0, 0/*, 0, 0*/},
		{"Varhany", 0, &Font_11x18, 0, 0, 0, 0, 0/*, 0, 0*/},
		{"Ukazatel", 0, &Font_11x18, 0, 0, 0, 0, 0/*, 0, 0*/},
		{"Nastaveni", 0, &Font_11x18, 0, 0, 0, 0, 0/*, &settingsmenu, "settingsmenu"*/}
};

struct menuitem settingsmenu[] = {
		{"Bluetooth", 0, &Font_11x18, 0, 0, 0, 1, &mainmenu[3].name/*, 0, 0*/},
		{"USB", 0, &Font_11x18, 0, 0, 0, 1, &mainmenu[3].name/*, 0, 0*/},
		{"MIDI", 0, &Font_11x18, 0, 0, 0, 1, &mainmenu[3].name/*, 0, 0*/},
		{"Zpet", 0, &Font_11x18, 1, 36, 37, 1, 0/*, 0, 0*/}
};

struct menuitem bluetoothmenu[] = {
		{"Skenovat", 0, &Font_11x18, 0, 0, 0, 2, &settingsmenu[0].name/*, 0, 0*/},
		{"Sparovat", 0, &Font_11x18, 0, 0, 0, 2, &settingsmenu[0].name/*, 0, 0*/},
		{"Sparovana zarizeni", 0, &Font_11x18, 0, 0, 0, 2, &settingsmenu[0].name/*, 0, 0*/},
		{"Informace", 0, &Font_11x18, 0, 0, 0, 2, &settingsmenu[0].name/*, 0, 0*/},
		{"Zpet", 0, &Font_11x18, 1, 36, 37, 2, 0/*, 0, 0*/}
};

struct menuitem organmenu[] = {
		{"Zapnout", 0, &Font_11x18, 0, 0, 0, 2, &mainmenu[2].name/*, 0, 0*/},
		{"Vypnout", 0, &Font_11x18, 0, 0, 0, 2, &mainmenu[2].name/*, 0, 0*/},
		{"Zpet", 0, &Font_11x18, 1, 36, 37, 2, 0/*, 0, 0*/}
};

struct menuitem controllermenu[] = {
		{"Pripojit", 0, &Font_11x18, 0, 0, 0, 2, &btBondedDevicesMenu[0].name/*, 0, 0*/},
		{"Odstranit", 0, &Font_11x18, 0, 0, 0, 2, &btBondedDevicesMenu[0].name/*, 0, 0*/},
		{"Zpet", 0, &Font_11x18, 1, 36, 37, 2, 0/*, 0, 0*/}
};


struct menuitem displaymenu[] = {
		{"Stav", 0, &Font_11x18, 0, 0, 0, 2, &mainmenu[2].name/*, 0, 0*/},
		{"Nastavit", 0, &Font_11x18, 0, 0, 0, 2, &mainmenu[2].name/*, 0, 0*/},
		{"Zpet", 0, &Font_11x18, 1, 36, 37, 2, 0/*, 0, 0*/}
};

struct menuitem displaysettingsmenu[] = {
		{"Pisen", 0, &Font_11x18, 0, 0, 0, 2, &displaysettingsmenu[0].name/*, 0, 0*/},
		{"Sloku", 0, &Font_11x18, 0, 0, 0, 2, &displaysettingsmenu[0].name/*, 0, 0*/},
		{"Pismeno", 0, &Font_11x18, 0, 0, 0, 2, &displaysettingsmenu[0].name/*, 0, 0*/},
		{"LED", 0, &Font_11x18, 0, 0, 0, 2, &displaysettingsmenu[0].name/*, 0, 0*/},
		{"Zpet", 0, &Font_11x18, 1, 36, 37, 2, 0/*, 0, 0*/}
};

struct menuitem displaysettingsledmenu[] = {
		{"Cervena", 0, &Font_11x18, 0, 0, 0, 2, &displaysettingsledmenu[0].name/*, 0, 0*/},
		{"Zluta", 0, &Font_11x18, 0, 0, 0, 2, &displaysettingsledmenu[0].name/*, 0, 0*/},
		{"Zelena", 0, &Font_11x18, 0, 0, 0, 2, &displaysettingsledmenu[0].name/*, 0, 0*/},
		{"Modra", 0, &Font_11x18, 0, 0, 0, 2, &displaysettingsledmenu[0].name/*, 0, 0*/},
		{"Zhasnuto", 0, &Font_11x18, 0, 0, 0, 2, &displaysettingsledmenu[0].name/*, 0, 0*/},
		{"Zpet", 0, &Font_11x18, 1, 36, 37, 2, 0/*, 0, 0*/}
};


//Rutina ktera se vykona pri kliknuti v menu
void oled_menuOnclick(int menupos){

	//Zaznamena se nazev menu
	char menunameold[255];
	char msg[50];
	memcpy(&menunameold, dispmenuname, strlen(dispmenuname)+1);

	//Zjisti se v jakem menu uzivatel kliknul
	if(strcmp(dispmenuname, "mainmenu") == 0){

		switch(menupos){
			case 0:
				//Odesle zpravu pro ziskani pisni
				msg[0] = 0x00;
				msg[1] = 0x04;
				workerAssert(&workerGetSongs);
				sendMsg(ADDRESS_MAIN, ADDRESS_PC, 0, INTERNAL, msg, 2);
			break;

			case 1:
				//Odesle zpravu pro nahravani
				oled_refreshPause();
			break;

			case 2:
				//Zobrazi menu varhan
				oled_setDisplayedMenu("organmenu",&organmenu, sizeof(organmenu), 1);
			break;

			case 3:
				//Zobrazi menu ukazatele
				oled_setDisplayedMenu("displaymenu",&displaymenu, sizeof(displaymenu), 1);
			break;

			default:
				//Nacte menu nastaveni
				oled_setDisplayedMenu("settingsmenu",&settingsmenu, sizeof(settingsmenu), 1);
			break;
		}

	}else if(strcmp(dispmenuname, "settingsmenu") == 0){
		switch(menupos){
			case 0:
				//Nacte bluetooth menu
				oled_setDisplayedMenu("bluetoothmenu",&bluetoothmenu, sizeof(bluetoothmenu), 1);
			break;

			case 1:
			break;

			case 2:
			break;

			default:
				//Vrati se do hlavniho menu
				oled_setDisplayedMenu("mainmenu",&mainmenu, sizeof(mainmenu), 0);
			break;
		}
	}else if(strcmp(dispmenuname, "bluetoothmenu") == 0){
		switch(menupos){
			case 0:
				//Zacne skenovani
				oled_setDisplayedSplash(oled_LoadingSplash, "Skenuji");
				workerAssert(&workerBtScanDev);
			break;

			case 1:
				//Najde sparovatelna zarizeni
				oled_setDisplayedSplash(oled_LoadingSplash, "Skenuji");
				workerAssert(&workerBtScanBondable);

			break;

			case 2:
				//Nacte sparovane ovladace
				oled_setDisplayedSplash(oled_LoadingSplash, "Nacitam");
				workerAssert(&workerBtBondDev);
			break;

			case 3:
				//Nacte info o sobe
				bluetooth_refreshSelfInfo();
			break;

			default:
				//Vrati se do nastaveni
				oled_setDisplayedMenu("settingsmenu",&settingsmenu, sizeof(settingsmenu), 0);
			break;
		}
	}else if(strcmp(dispmenuname, "displaymenu") == 0){
		switch(menupos){
			case 0:
				//
				oled_setDisplayedSplash(oled_DisplayStatusSplash, "");
			break;

			case 1:
				//
				oled_setDisplayedMenu("displaysettingsmenu",&displaysettingsmenu, sizeof(displaysettingsmenu), 0);

			break;

			default:
				//Vrati se do hlavniho menu
				oled_setDisplayedMenu("mainmenu",&mainmenu, sizeof(mainmenu), 0);
			break;
		}
	}else if(strcmp(dispmenuname, "displaysettingsmenu") == 0){
		switch(menupos){
			case 0:
				//
				numDispSong.digits = 4;
				sprintf(numDispSong.enteredValue, "----");
				numDispSong.message = "Cislo pisne";
				numDispSong.selectedDigit = 0;
				numDispSong.characters = "-0123456789";
				numDispSong.charactersLen = 11;
				numDispSong.application = APP_DISPLAY;
				oled_setDisplayedSplash(oled_ValueEnterSplash, &numDispSong);
			break;

			case 1:
				//
				numDispVerse.digits = 2;
				sprintf(numDispVerse.enteredValue, "--");
				numDispVerse.message = "Cislo sloky";
				numDispVerse.selectedDigit = 0;
				numDispVerse.characters = "-0123456789";
				numDispVerse.charactersLen = 11;
				numDispVerse.application = APP_DISPLAY;
				oled_setDisplayedSplash(oled_ValueEnterSplash, &numDispVerse);

			break;

			case 2:
				//
				numDispLetter.digits = 1;
				sprintf(numDispLetter.enteredValue, "-");
				numDispLetter.message = "Pismeno";
				numDispLetter.selectedDigit = 0;
				numDispLetter.characters = "-ABCD";
				numDispLetter.charactersLen = 5;
				numDispLetter.application = APP_DISPLAY;
				oled_setDisplayedSplash(oled_ValueEnterSplash, &numDispLetter);
			break;

			case 3:
				//
				oled_setDisplayedMenu("displaysettingsledmenu",&displaysettingsledmenu, sizeof(displaysettingsledmenu), 0);
			break;

			default:
				//Vrati se do hlavniho menu
				oled_setDisplayedMenu("displaymenu",&displaymenu, sizeof(displaymenu), 0);
			break;
		}
	}else if(strcmp(dispmenuname, "displaysettingsledmenu") == 0){
		switch(menupos){
			case 0:
				//
				dispLED = DISP_LED_RED;
				workerAssert(&workerDispRefresh);
			break;

			case 1:
				//
				dispLED = DISP_LED_YELLOW;
				workerAssert(&workerDispRefresh);
			break;

			case 2:
				//
				dispLED = DISP_LED_GREEN;
				workerAssert(&workerDispRefresh);
			break;

			case 3:
				//
				dispLED = DISP_LED_BLUE;
				workerAssert(&workerDispRefresh);
			break;

			case 4:
				//
				dispLED = DISP_LED_CLEAR;
				workerAssert(&workerDispRefresh);
			break;

			default:
				//Vrati se do hlavniho menu
				oled_setDisplayedMenu("displaysettingsmenu",&displaysettingsmenu, sizeof(displaysettingsmenu), 0);
			break;
		}
	}else if(strcmp(dispmenuname, "songmenu") == 0){
		if(menupos == songMenuSize){
			//Vrati se do hlavniho menu
			oled_setDisplayedMenu("mainmenu",&mainmenu, sizeof(mainmenu), 0);
		}else{
			memset(selectedSong, 0, 40);
			sprintf(selectedSong, "%s", songMenu[encoderpos].name);
			//memcpy(&selectedSong[0], songMenu[encoderpos].name, strlen(songMenu[encoderpos].name));
			midiControl_play(ADDRESS_MAIN, selectedSong);
		}
	}else if(strcmp(dispmenuname, "btScanedDevices") == 0){
		if(menupos == btScannedCount){
			//Vrati se do bluetooth menu
			oled_setDisplayedMenu("bluetoothmenu",&bluetoothmenu, sizeof(bluetoothmenu), 0);
		}else{
			//Ukaze info o zarizeni
			oled_setDisplayedSplash(oled_BtDevInfoSplash, &btScanned[menupos]);
		}
	}else if(strcmp(dispmenuname, "btBondedDevicesMenu") == 0){
		if(menupos == btBondedCount){
			//Vrati se do hlavniho menu
			oled_setDisplayedMenu("mainmenu",&mainmenu, sizeof(mainmenu), 0);
		}else{
			//Ukaze menu ovladacu
			btSelectedController = menupos;
			encoderpos = 1;
			oled_setDisplayedMenu("controllermenu",&controllermenu, sizeof(controllermenu), 0);
		}
	}else if(strcmp(dispmenuname, "btBondableDevices") == 0){
		if(menupos == btBondableCount){
			//Vrati se do BT menu
			oled_setDisplayedMenu("bluetoothmenu",&bluetoothmenu, sizeof(bluetoothmenu), 0);
		}else{
			//Pokusi se sparovat se zarizenim
			btSelectedBondDevice = menupos;
			oled_setDisplayedSplash(oled_BtDevPairAckSplash, &btBondable[btSelectedBondDevice]);
		}
	}else if(strcmp(dispmenuname, "controllermenu") == 0){
		switch(menupos){
			case 0:
				sprintf(workerBtConnectMAC, "%02X%02X%02X%02X%02X%02X", btBonded[btSelectedController].mac[0], btBonded[btSelectedController].mac[1], btBonded[btSelectedController].mac[2], btBonded[btSelectedController].mac[3], btBonded[btSelectedController].mac[4], btBonded[btSelectedController].mac[5]);

				//Pokusi se pripojit
				workerAssert(&workerBtConnect);
			break;

			case 1:
				//Odstrani ovladac
				workerAssert(&workerBtRemoveController);
			break;

			default:
				//Vrati se do hlavniho menu
				oled_setDisplayedMenu("btBondedDevicesMenu", &btBondedDevicesMenu, sizeof(btBondedDevicesMenu)-(10-btBondedCount-1)*sizeof(btBondedDevicesMenu[9]), 0);
			break;
		}
	}else if(strcmp(dispmenuname, "organmenu") == 0){
		switch(menupos){
					case 0:
						//Zapne proudový zdroj
						midiControl_current_On();
					break;

					case 1:
						//Vypne proudový zdroj
						midiControl_current_Off();
					break;

					default:
						//Vrati se zpet do hlavniho menu
						oled_setDisplayedMenu("mainmenu",&mainmenu, sizeof(mainmenu), 0);
					break;
				}
	}


	encoderclick = 0;

	if(strcmp(dispmenuname, menunameold) != 0) encoderpos = 0;
}

void oled_begin(){
	//Inicializuje se driver oled
	ssd1306_Init();
	//Nastavi se defaultni menu a splash screen
	oled_setDisplayedMenu("mainmenu", &mainmenu, sizeof(mainmenu), 0);
	oled_setDisplayedSplash(oled_StartSplash, "");
	refreshHalt = 0;
	encoderpos = 0;
	encoderposOld = -1;
	scrollPause = 0;
	scrollPauseDone = 0;
	loadingToggle = 0;
	encoderDirSwap = 0;
	//Zapnou se casovace pro obnovu OLED a scrollovani
	HAL_TIM_Base_Start_IT(&htim2);
	//Zapne se timer crollovani
	HAL_TIM_Base_Start_IT(&htim4);
	oledHeader = (char*)malloc(50);
}


//Rutina pro obnovu OLED
void oled_refresh(){
	//Vyplni se cernou
	ssd1306_Fill(0);

	if(oledType == OLED_MENU){
		//Pokud se ma zobrazovat menu, vykresli se
		oled_drawMenu();
	}else if(oledType == OLED_SPLASH){
		//Pokud se ma zobrazit splash screen, zobrazi se
		(*splashFunction)(splashParams);
		ssd1306_UpdateScreen(0);
	}
}

void oled_setDisplayedMenu(char *menuname ,struct menuitem (*menu)[], int menusize, int issubmenu){
	//Data z menu se okopiruji do zobrazovaneho dispmenu
	dispmenuname = malloc(strlen(menuname)+1);
	dispmenusize = menusize/sizeof(struct menuitem);
	memcpy(dispmenuname, menuname, strlen(menuname)+1);
	memcpy(&dispmenu, menu, menusize);
	dispmenusubmenu = issubmenu;
	encoderposOld = -1;
	encoderpos = 0;
	oledType = OLED_MENU;
}

void oled_setDisplayedSplash(void (*funct)(), void * params){
	//Nastavi se pointery na vykonavanou funkci pri splash screen
	loadingStat = 1;
	encoderpos = 0;
	splashFunction = funct;
	splashParams = params;
	oledType = OLED_SPLASH;

}

void oled_drawMenu(){

	//Pokud bylo kliknuto, provedou se portrebne fce
	if(encoderclick){
		oled_menuOnclick(encoderpos);
	}

	//Ziska se aktualni cas
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

	//Vypise se hlavicka
	sprintf(oledHeader, "%d %d %d %d", alivePC, aliveRemote, btCmdMode, btStreamOpen);
	//sprintf(oledHeader, "%d.%d %d:%d  %d %d",date.Date, date.Month, time.Hours, time.Minutes, alivePC, aliveRemote);
	//sprintf(oledHeader, "E: %d N: %s", encoderpos, dispmenu[encoderpos].name);
	//sprintf(oledHeader, "Disp: %d", HAL_GPIO_ReadPin(DISP_SENSE_GPIO_Port, DISP_SENSE_Pin));
	//oledHeader = "MIDIControll 0.1";
	//sprintf(oledHeader, "%d %d %d", HAL_GPIO_ReadPin(MIDI_ACTIVE_GPIO_Port, MIDI_ACTIVE_Pin), HAL_GPIO_ReadPin(MIDI_SEARCHING_GPIO_Port, MIDI_SEARCHING_Pin), HAL_GPIO_ReadPin(MIDI_IO_SELECTED_GPIO_Port, MIDI_IO_SELECTED_Pin));
	//sprintf(oledHeader, "E: %d", loadingStat);
	ssd1306_SetCursor(2,0);
	ssd1306_WriteString(oledHeader, Font_7x10, White);

	//for(uint8_t i = 0; i <= 128; i++) ssd1306_DrawPixel(i, 13, White);


	//Zapne se scrollovani
	if(strlen(dispmenu[encoderpos].name) > 9 && encoderpos != encoderposOld){
		scrollIndex = 0;
		scrollMax = strlen(dispmenu[encoderpos].name)-10;
		encoderposOld = encoderpos;
		scrollPause = 0;
		scrollPauseDone = 0;
	}

	//Vykresli se dispmenu
	if(encoderpos != (signed int)(dispmenusize)-1){
		for(int i = encoderpos; i <= (encoderpos+1); i++){
			//Vypise puntik
			if(i == encoderpos){
				dispmenu[i].selected = 1;
			}else dispmenu[i].selected = 0;

			ssd1306_SetCursor(OLED_MENU_LEFT_PADDING + OLED_MENU_TEXT_WIDTH,(i-encoderpos+1)*OLED_MENU_TEXT_HEIGHT + OLED_MENU_TOP_PADDING);

			if(strlen(dispmenu[i].name) > 9){
				if(dispmenu[i].selected){
					char tmp[10];
					memcpy(tmp, dispmenu[i].name+scrollIndex, 9);
					memset(tmp+9, 0, strlen(dispmenu[i].name)-9);
					ssd1306_WriteString(tmp, *dispmenu[i].font, White);
				}else{
					char tmp[10];
					memcpy(tmp, dispmenu[i].name, 9);
					memset(tmp+9, 0, strlen(dispmenu[i].name)-9);
					ssd1306_WriteString(tmp, *dispmenu[i].font, White);
				}

			}else ssd1306_WriteString(dispmenu[i].name, *dispmenu[i].font, White);

			ssd1306_SetCursor(OLED_MENU_LEFT_PADDING, (i-encoderpos+1)*OLED_MENU_TEXT_HEIGHT + OLED_MENU_TOP_PADDING);

			if(dispmenu[i].hasSpecialSelector && dispmenu[i].selected){
				ssd1306_WriteChar(dispmenu[i].specharSelected, Icon_11x18, White);
			}else if(dispmenu[i].hasSpecialSelector && !dispmenu[i].selected){
				ssd1306_WriteChar(dispmenu[i].specharNotSelected, Icon_11x18, White);
			}else{
				ssd1306_WriteChar(33-(dispmenu[i].selected), Icon_11x18, White);
			}
		}
	}else{
		for(int i = encoderpos-1; i <= (encoderpos); i++){
				//Vypise puntik
				if((i) == encoderpos){
					dispmenu[i].selected = 1;
				}else dispmenu[i].selected = 0;

				ssd1306_SetCursor(OLED_MENU_LEFT_PADDING + OLED_MENU_TEXT_WIDTH,(i-encoderpos+2)*OLED_MENU_TEXT_HEIGHT + OLED_MENU_TOP_PADDING);

				if(strlen(dispmenu[i].name) > 9){
					if(dispmenu[i].selected){
						char tmp[10];
						memcpy(tmp, dispmenu[i].name+scrollIndex, 9);
						memset(tmp+9, 0, strlen(dispmenu[i].name)-9);
						ssd1306_WriteString(tmp, *dispmenu[i].font, White);
					}else{
						char tmp[10];
						memcpy(tmp, dispmenu[i].name, 9);
						memset(tmp+9, 0, strlen(dispmenu[i].name)-9);
						ssd1306_WriteString(tmp, *dispmenu[i].font, White);
				}

				}else ssd1306_WriteString(dispmenu[i].name, *dispmenu[i].font, White);

				ssd1306_SetCursor(OLED_MENU_LEFT_PADDING, (i-encoderpos+2)*OLED_MENU_TEXT_HEIGHT + OLED_MENU_TOP_PADDING);

				if(dispmenu[i].hasSpecialSelector && dispmenu[i].selected){
					ssd1306_WriteChar(dispmenu[i].specharSelected, Icon_11x18, White);
				}else if(dispmenu[i].hasSpecialSelector && !dispmenu[i].selected){
					ssd1306_WriteChar(dispmenu[i].specharNotSelected, Icon_11x18, White);
				}else{
					ssd1306_WriteChar(33-(dispmenu[i].selected), Icon_11x18, White);
				}

			}
	}

	if(encoderpos <= 0){
		//Vypise sipku nahoru
		ssd1306_SetCursor(117,41);
		ssd1306_WriteChar(35, Icon_11x18, White);
	}else if(encoderpos == dispmenusize-1){
		//Vypise sipku dolu
		ssd1306_SetCursor(117,19);
		ssd1306_WriteChar(34, Icon_11x18, White);
	}else if(encoderpos > 0 && encoderpos < dispmenusize){
		//Vypise sipku dolu
		ssd1306_SetCursor(117,19);
		ssd1306_WriteChar(34, Icon_11x18, White);
		//Vypise sipku nahoru
		ssd1306_SetCursor(117,41);
		ssd1306_WriteChar(35, Icon_11x18, White);
	};


	if(!refreshHalt) ssd1306_UpdateScreen(0);

}

//Funkce pro zastaveni/zpomaleni refreshe na nutne minimum
void oled_refreshPause(){
	refreshHalt = 1;
}

//Funkce pro zapnuti refreshe
void oled_refreshResume(){
	refreshHalt = 0;
}

//Funkce vykreslujici zapinaci obrazovku
void oled_StartSplash(){
	//Vypisou se texty
	ssd1306_SetCursor((128-(strlen("MIDIControl"))*11)/2,10);
	ssd1306_WriteString("MIDIControl", Font_11x18, White);

	ssd1306_SetCursor((128-(strlen("BASE"))*11)/2,30);
	ssd1306_WriteString("BASE", Font_11x18, White);

	char * version = "Verze 1.1";
	ssd1306_SetCursor((128-(strlen(version))*7)/2,50);
	ssd1306_WriteString(version, Font_7x10, White);

	//Vyporada se s kliknutim tlacitka
	encoderclick = 0;
}

//Funkce pro vykresleni libovolneho textu s nacitacimi puntiky
void oled_LoadingSplash(char * msg){
	//Vypise se text
	ssd1306_SetCursor((128-(strlen(msg))*11)/2, 15);
	ssd1306_WriteString(msg, Font_11x18, White);

	//Vykresli se puntiky
	ssd1306_SetCursor(36,35);
	ssd1306_WriteChar(33 - (loadingStat & 0x01), Icon_11x18, White);
	ssd1306_SetCursor(58,35);
	ssd1306_WriteChar(33 - ((loadingStat>>1) & 0x01), Icon_11x18, White);
	ssd1306_SetCursor(80,35);
	ssd1306_WriteChar(33 - ((loadingStat>>2) & 0x01), Icon_11x18, White);
	encoderclick = 0;

}

//Funkce pro vypsani chyby
void oled_ErrorSplash(char * msg){
	ssd1306_SetCursor((128-(strlen("Chyba!"))*11)/2, 15);
	ssd1306_WriteString("Chyba!", Font_11x18, White);

	ssd1306_SetCursor((128-(strlen(msg))*7)/2, 15);
	ssd1306_WriteString(msg, Font_7x10, White);

	encoderclick = 0;

}

//Funkce pro vykresleni obrazovky "Cekman na pripojeni USB"
void oled_UsbWaitingSplash(){
	//Vykresli se texty
	char * msg = "Cekam na";
	ssd1306_SetCursor((128-(strlen(msg))*11)/2, 1);
	ssd1306_WriteString(msg, Font_11x18, White);
	msg = "aplikaci";
	ssd1306_SetCursor((128-(strlen(msg))*11)/2, 23);
	ssd1306_WriteString(msg, Font_11x18, White);

	//VYkresli se puntiky
	ssd1306_SetCursor(36,45);
	ssd1306_WriteChar(33 - (loadingStat & 0x01), Icon_11x18, White);
	ssd1306_SetCursor(58,45);
	ssd1306_WriteChar(33 - ((loadingStat>>1) & 0x01), Icon_11x18, White);
	ssd1306_SetCursor(80,45);
	ssd1306_WriteChar(33 - ((loadingStat>>2) & 0x01), Icon_11x18, White);
	encoderclick = 0;
}

//Funkce vykreslujici "Zadne vysledky"
void oled_NothingFound(){
	char * msg = "Zadne";
	ssd1306_SetCursor((128-(strlen(msg))*11)/2, 1);
	ssd1306_WriteString(msg, Font_11x18, White);
	msg = "vysledky";
	ssd1306_SetCursor((128-(strlen(msg))*11)/2, 23);
	ssd1306_WriteString(msg, Font_11x18, White);
	if(encoderclick){
		oledType = OLED_MENU;
		encoderclick = 0;
	}
}


//Funkce vykreslujici ze struktry info obrazovku o zarizeni
void oled_BtDevInfoSplash(struct btDevice * dev){

	//Nastavi se scrollovani pokud je jmeno delsi nez obrazovka
	if(strlen(dev->name) > 9){
		scrollMax = (strlen(dev->name) - 10);
		ssd1306_SetCursor(14, 1);
		char tmp[10];
		memcpy(tmp, (char*)(dev->name)+scrollIndex, 9);
		memset(tmp+9, 0, strlen(dev->name)-9);
		ssd1306_WriteString(tmp, Font_11x18, White);
	}else{
		ssd1306_SetCursor((128-(strlen(dev->name))*9)/2, 1);
		ssd1306_WriteString(dev->name, Font_11x18, White);
	}

	//Vypisou se udaje
	char msg[25];
	sprintf(msg, "%02X-%02X-%02X-%02X-%02X-%02X", dev->mac[0], dev->mac[1], dev->mac[2], dev->mac[3], dev->mac[4], dev->mac[5]);
	ssd1306_SetCursor((128-(strlen(msg))*7)/2, 30);
	ssd1306_WriteString(msg, Font_7x10, White);

	//Vypise se RSSI
	sprintf(msg, "RSSI: %ddB", dev->rssi);
	ssd1306_SetCursor((128-(strlen(msg))*7)/2, 43);
	ssd1306_WriteString(msg, Font_7x10, White);


	//Pokud se klikne, vrati se do menu
	if(encoderclick){
		oledType = OLED_MENU;
		encoderclick = 0;
	}
}

//Funkce pro vykresleni obrazovky pro potvrzeni parovani
void oled_BtDevPairAckSplash(struct btDevice * dev){
	//Obrati se orientace enkoderu
	encoderDirSwap = 1;
	//Omezi se pohyb enkoderu na 2
	dispmenusize = 2;

	//Nastavi se scrollovani pokud je jmeno delsi nez obrazovka
	if(strlen(dev->name) > 9){
		scrollMax = (strlen(dev->name) - 10);
		ssd1306_SetCursor(14, 1);
		char tmp[10];
		memcpy(tmp, (char*)(dev->name)+scrollIndex, 9);
		memset(tmp+9, 0, strlen(dev->name)-9);
		ssd1306_WriteString(tmp, Font_11x18, White);
	}else{
		ssd1306_SetCursor((128-(strlen(dev->name))*9)/2, 1);
		ssd1306_WriteString(dev->name, Font_11x18, White);
	}


	//Vypise se hlaska "Opravdu parovat?"
	char msg[25];

	sprintf(msg, "Opravdu");
	ssd1306_SetCursor(((128-((float)strlen(msg)-0.5)*7)/2), 25);
	ssd1306_WriteString(msg, Font_7x10, White);

	sprintf(msg, "parovat?");
	ssd1306_SetCursor(((128-((float)strlen(msg)-0.5)*7)/2), 37);
	ssd1306_WriteString(msg, Font_7x10, White);

	//Podle pozice enkoderu se meni vybrany text
	if(encoderpos == 0){
		sprintf(msg, "ANO");
		for(int x = 0; x < 64; x++){
			for(int y = 0; y < 12; y++){
				ssd1306_DrawPixel(x, y+51, White);
			}
		}

		ssd1306_SetCursor(((64-((float)strlen(msg)-0.5)*7)/2), 53);
		ssd1306_WriteString(msg, Font_7x10, Black);


		sprintf(msg, "NE");
		ssd1306_SetCursor(((64-((float)strlen(msg)-0.5)*7)/2)+64, 53);
		ssd1306_WriteString(msg, Font_7x10, White);

	}else{
		sprintf(msg, "ANO");
		ssd1306_SetCursor(((64-((float)strlen(msg)-0.5)*7)/2), 53);
		ssd1306_WriteString(msg, Font_7x10, White);


		sprintf(msg, "NE");
		for(int x = 0; x < 64; x++){
			for(int y = 0; y < 12; y++){
				ssd1306_DrawPixel(x+64, y+51, White);
			}
		}
		ssd1306_SetCursor(((64-((float)strlen(msg)-0.5)*7)/2)+64, 53);
		ssd1306_WriteString(msg, Font_7x10, Black);

	}

	//Pokud na danem textu klikne
	if(encoderclick){
		encoderDirSwap = 0;
		if(encoderpos == 0){
			//Klikne na ano - pokusi se parovat
			sprintf(workerBtConnectMAC, "%02X%02X%02X%02X%02X%02X", btBondable[btSelectedBondDevice].mac[0], btBondable[btSelectedBondDevice].mac[1], btBondable[btSelectedBondDevice].mac[2], btBondable[btSelectedBondDevice].mac[3], btBondable[btSelectedBondDevice].mac[4], btBondable[btSelectedBondDevice].mac[5]);
			btPairing = 1;
			workerAssert(&workerBtConnect);
			oled_setDisplayedSplash(oled_LoadingSplash, "Paruji");
		}else{
			//Klikne na ne - vrati se do menu
			oled_setDisplayedMenu("btBondableDevices", &btBondableDevices, sizeof(btBondableDevices)-(20-btBondableCount-1)*sizeof(btBondableDevices[19]), 0);
			oledType = OLED_MENU;
		}

		encoderclick = 0;
	}
}

//Funkce vykreslujici obrazovku s zadosti o parovani
void oled_BtDevPairRequestSplash(struct btDevice * dev){

	//Nastavi se scrollovani pokud je jmeno delsi nez obrazovka
	if(strlen(dev->name) > 9){
		scrollMax = (strlen(dev->name) - 10);
		ssd1306_SetCursor(14, 1);
		char tmp[10];
		memcpy(tmp, (char*)(dev->name)+scrollIndex, 9);
		memset(tmp+9, 0, strlen(dev->name)-9);
		ssd1306_WriteString(tmp, Font_11x18, White);
	}else{
		ssd1306_SetCursor((128-(strlen(dev->name))*9)/2, 1);
		ssd1306_WriteString(dev->name, Font_11x18, White);
	}


	//Vypise se hlaska
	char msg[25];

	sprintf(msg, "Zada parovani");
	ssd1306_SetCursor((128-(strlen(msg))*7)/2, 25);
	ssd1306_WriteString(msg, Font_7x10, White);

	//Vypise se PIN kod
	sprintf(msg, "PIN %06ld", dev->pin);
	ssd1306_SetCursor((128-(strlen(msg))*11)/2, 41);
	ssd1306_WriteString(msg, Font_11x18, White);

	//Po kliknuti skoci do menu
	if(encoderclick){
		oledType = OLED_MENU;
		encoderclick = 0;
	}
}

//Obrazovka vypisujici "Parovani kompletni"
void oled_BtDevPairCompleteSplash(char * msg){

	//Vypise se hlaska
	ssd1306_SetCursor((128-(strlen(msg))*11)/2, 25);
	ssd1306_WriteString(msg, Font_11x18, White);

	char * msg2 = "Parovani";
	ssd1306_SetCursor((128-(strlen(msg2))*11)/2, 1);
	ssd1306_WriteString(msg2, Font_11x18, White);

	//Pri kliknuti skoci zpet do menu
	if(encoderclick){
		encoderclick = 0;
		oled_setDisplayedMenu("mainmenu",&mainmenu, sizeof(mainmenu), 0);
		oledType = OLED_MENU;
	}
}

//Vykresli obrazovku prehravani
void oled_playingSplash(char * songname){

	//Vykresli hlasku "Prehravam"
	char msg[25];

	sprintf(msg, "Prehravam");
	ssd1306_SetCursor((128-(strlen(msg))*11)/2, 1);
	ssd1306_WriteString(msg, Font_11x18, White);

	//Vypise nazev pisne
	if(strlen(songname) > 9){
			scrollMax = (strlen(songname) - 10);
			ssd1306_SetCursor(14, 25);
			char tmp[10];
			memcpy(tmp, (char*)(songname)+scrollIndex, 9);
			memset(tmp+9, 0, strlen(songname)-9);
			ssd1306_WriteString(tmp, Font_11x18, White);
		}else{
			ssd1306_SetCursor((128-(strlen(songname))*9)/2, 25);
			ssd1306_WriteString(songname, Font_11x18, White);
		}

	//Vykresli bily blok okolo textu "Zastavit"
	for(int x = 0; x < 128; x++){
		for(int y = 0; y < 12; y++){
			ssd1306_DrawPixel(x, y+51, White);
		}
	}

	//Vypise "Zastavit"
	sprintf(msg, "Zastavit");
	ssd1306_SetCursor((128-(strlen(msg))*7)/2, 53);
	ssd1306_WriteString(msg, Font_7x10, Black);

	//Blika se LED
	if(loadingToggle){
		setStatus(FRONT2, DEV_OK);
		setStatus(FRONT3, DEV_OK);
	}else setStatusAll(1, DEV_CLR);


	//Pri kliknuti
	if(encoderclick){
		//Skoci zpet do menu
		oled_setDisplayedMenu("mainmenu", &mainmenu, sizeof(mainmenu), 0);
		oledType = OLED_MENU;
		//Zapne refresh
		oled_refreshResume();
		//Vypne LED
		setStatusAll(1, DEV_CLR);
		//Zastavi prehravani
		midiControl_stop(ADDRESS_MAIN);
		encoderclick = 0;
	}
}

//Vykresli obrazovku nahravani
void oled_recordingSplash(char * songname){

	//Vypise hlasku "Nahravam"
	char msg[25];

	sprintf(msg, "Nahravam");
	ssd1306_SetCursor((128-(strlen(msg))*11)/2, 1);
	ssd1306_WriteString(msg, Font_11x18, White);

	//Vypise nazev pisne
	if(strlen(songname) > 9){
			scrollMax = (strlen(songname) - 10);
			ssd1306_SetCursor(14, 25);
			char tmp[10];
			memcpy(tmp, (char*)(songname)+scrollIndex, 9);
			memset(tmp+9, 0, strlen(songname)-9);
			ssd1306_WriteString(tmp, Font_11x18, White);
		}else{
			ssd1306_SetCursor((128-(strlen(songname))*9)/2, 25);
			ssd1306_WriteString(songname, Font_11x18, White);
		}

	//Vykresli bily blok okolo textu "Zastavit"
	for(int x = 0; x < 128; x++){
		for(int y = 0; y < 12; y++){
			ssd1306_DrawPixel(x, y+51, White);
		}
	}

	//Vypise "Zastavit"
	sprintf(msg, "Zastavit");
	ssd1306_SetCursor((128-(strlen(msg))*7)/2, 53);
	ssd1306_WriteString(msg, Font_7x10, Black);

	//Blika se LED
	if(loadingToggle){
		setStatus(FRONT2, DEV_ERR);
		setStatus(FRONT3, DEV_ERR);
	}else setStatusAll(1, DEV_CLR);

	//Pri kliknuti
	if(encoderclick){
		//Vrati se zpet do menu
		oled_setDisplayedMenu("mainmenu", &mainmenu, sizeof(mainmenu), 0);
		oledType = OLED_MENU;
		//Obnovi refresh
		oled_refreshResume();
		//Vycisti LED
		setStatusAll(1, DEV_CLR);
		//Zastavi prehravani
		midiControl_stop(ADDRESS_MAIN);
		encoderclick = 0;
	}
}

void oled_DisplayStatusSplash(){

	char msg[20];
	//
	sprintf(msg, " %c%c%c%c ", dispSong[3], dispSong[2], dispSong[1], dispSong[0]);
	ssd1306_SetCursor((128-(strlen(msg))*16)/2, 3);
	ssd1306_WriteString(msg, Font_16x26, White);

	sprintf(msg, " %c%c %c ", dispVerse[1], dispVerse[0], dispLetter);
	ssd1306_SetCursor((128-(strlen(msg))*16)/2, 33);
	ssd1306_WriteString(msg, Font_16x26, White);

	if(dispLED != dispLEDOld){
		setColorAll(1, CLR_CLEAR);
		dispLEDOld = dispLED;
	}

	switch(dispLED){
		case DISP_LED_BLUE:
			setColor(FRONT4, CLR_BLUE);
		break;

		case DISP_LED_GREEN:
			setColor(FRONT3, CLR_GREEN);
		break;

		case DISP_LED_RED:
			setColor(FRONT1, CLR_RED);
		break;

		case DISP_LED_YELLOW:
			setColor(FRONT2, CLR_YELLOW);
		break;

		default:
			setColorAll(1, CLR_CLEAR);
		break;

	}


	//Pri kliknuti skoci zpet do menu
	if(encoderclick){
		setColorAll(1, CLR_CLEAR);
		encoderclick = 0;
		oled_setDisplayedMenu("displaymenu",&displaymenu, sizeof(displaymenu), 0);
		oledType = OLED_MENU;
	}
}

void oled_NumberEnterSplash(struct reqNumber * num){

	dispmenusize = 10;

	char msg[25];

	ssd1306_SetCursor((128-((float)strlen(num->message)-0.5)*11)/2, 1);
	ssd1306_WriteString(num->message, Font_11x18, White);


	sprintf(msg, "%04ld", num->enteredNumber);
	ssd1306_SetCursor((128-((float)strlen(msg)-0.5)*11)/2, 28);
	ssd1306_WriteString(msg, Font_11x18, White);

	if(loadingToggle){
		for(int x = 0; x < 11; x++){
			for(int y = 0; y < 3; y++){
				ssd1306_DrawPixel(x+(128-((float)strlen(msg)-0.5)*11)/2+num->selectedDigit*11, y+48, White);
			}
		}
	}


	uint8_t digit = (num->enteredNumber / (long int)pow(10, (num->digits - 1 - num->selectedDigit))) - (num->enteredNumber/(long int)pow(10, (num->digits - 1 - num->selectedDigit)+1))*10;

	num->enteredNumber = num->enteredNumber - digit*pow(10, (num->digits - 1 - num->selectedDigit)) + encoderpos*pow(10, (num->digits - 1 - num->selectedDigit));


	if(encoderclick){
		//workerAssert(&workerBtEnterPairingKey);
		oledType = OLED_MENU;
		encoderclick = 0;
	}
}

void oled_ValueEnterSplash(struct reqValue * num){

	encoderDirSwap = 1;
	dispmenusize = num->charactersLen;

	num->enteredValue[num->selectedDigit] = num->characters[encoderpos];

	char msg[40];

	ssd1306_SetCursor((128-((float)strlen(num->message)-0.5)*11)/2, 1);
	ssd1306_WriteString(num->message, Font_11x18, White);

	sprintf(msg, "%*s", num->digits ,num->enteredValue);
	ssd1306_SetCursor((128-((float)strlen(msg)-0.5)*11)/2, 28);
	ssd1306_WriteString(msg, Font_11x18, White);

	if(loadingToggle){
		for(int x = 0; x < 11; x++){
			for(int y = 0; y < 3; y++){
				ssd1306_DrawPixel(x+(128-((float)strlen(msg)-0.5)*11)/2+num->selectedDigit*11, y+48, White);
			}
		}
	}


	/*uint8_t digit = (num->enteredNumber / (long int)pow(10, (num->digits - 1 - num->selectedDigit))) - (num->enteredNumber/(long int)pow(10, (num->digits - 1 - num->selectedDigit)+1))*10;

	num->enteredNumber = num->enteredNumber - digit*pow(10, (num->digits - 1 - num->selectedDigit)) + encoderpos*pow(10, (num->digits - 1 - num->selectedDigit));

*/
	if(encoderclick && num->selectedDigit >= (num->digits-1)){

		if(num->application == APP_DISPLAY){
			workerAssert(&workerDispRefresh);
		}

		encoderDirSwap = 0;
		encoderclick = 0;
		oled_setDisplayedMenu("displaysettingsmenu",&displaysettingsmenu, sizeof(displaysettingsmenu), 0);
		oledType = OLED_MENU;
	}else if(encoderclick){
		num->selectedDigit++;
		encoderclick = 0;
		encoderpos = 0;
	}
}

