#include "oled.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stm32g4xx_hal_rtc.h>
#include <rtc.h>
#include "bluetooth.h"
#include "midiControl.h"

struct menuitem mainmenu[] = {
		{"Prehraj", 0, &Font_11x18, 0, 0, 0, 0, 0/*, 0, 0*/},
		{"Nahraj", 0, &Font_11x18, 0, 0, 0, 0, 0/*, 0, 0*/},
		{"Varhany", 0, &Font_11x18, 0, 0, 0, 0, 0/*, 0, 0*/},
		{"Ovladace", 0, &Font_11x18, 0, 0, 0, 0, 0/*, 0, 0*/},
		{"Nastaveni", 0, &Font_11x18, 0, 0, 0, 0, 0/*, &settingsmenu, "settingsmenu"*/}
};

struct menuitem settingsmenu[] = {
		{"Bluetooth", 0, &Font_11x18, 0, 0, 0, 1, &mainmenu[3].name/*, 0, 0*/},
		{"USB", 0, &Font_11x18, 0, 0, 0, 1, &mainmenu[3].name/*, 0, 0*/},
		{"MIDI", 0, &Font_11x18, 0, 0, 0, 1, &mainmenu[3].name/*, 0, 0*/},
		{"Zpet", 0, &Font_11x18, 1, 36, 37, 1, 0/*, 0, 0*/}
};

struct menuitem bluetoothmenu[] = {
		{"Pripojeno", 0, &Font_11x18, 0, 0, 0, 2, &settingsmenu[0].name/*, 0, 0*/},
		{"Sparovat", 0, &Font_11x18, 0, 0, 0, 2, &settingsmenu[0].name/*, 0, 0*/},
		{"Zpet", 0, &Font_11x18, 1, 36, 37, 2, 0/*, 0, 0*/}
};

struct menuitem organmenu[] = {
		{"Zapnout", 0, &Font_11x18, 0, 0, 0, 2, &mainmenu[2].name/*, 0, 0*/},
		{"Vypnout", 0, &Font_11x18, 0, 0, 0, 2, &mainmenu[2].name/*, 0, 0*/},
		{"Zpet", 0, &Font_11x18, 1, 36, 37, 2, 0/*, 0, 0*/}
};




void oled_menuOnclick(int menupos){

	char menunameold[255];
	memcpy(&menunameold, dispmenuname, strlen(dispmenuname)+1);

	if(strcmp(dispmenuname, "mainmenu") == 0){

		switch(menupos){
			case 0:

			break;

			case 1:
			break;

			case 2:
				oled_setDisplayedMenu("organmenu",&organmenu, sizeof(organmenu), 1);
			break;

			case 3:
				//bluetoothGetBondedDevices();
				oled_setDisplayedMenu("btBondedDevicesMenu", &btBondedDevicesMenu, sizeof(btBondedDevicesMenu)-(10-btBondedCount-1)*sizeof(btBondedDevicesMenu[9]), 0);

			break;

			case 4:
				oled_setDisplayedMenu("settingsmenu",&settingsmenu, sizeof(settingsmenu), 1);
			break;

			default:
			break;
		}

	}else if(strcmp(dispmenuname, "settingsmenu") == 0){
		switch(menupos){
			case 0:
				oled_setDisplayedMenu("bluetoothmenu",&bluetoothmenu, sizeof(bluetoothmenu), 1);
			break;

			case 1:
			break;

			case 2:
			break;

			case 3:
				oled_setDisplayedMenu("mainmenu",&mainmenu, sizeof(mainmenu), 0);
			break;

			default:
			break;
		}
	}else if(strcmp(dispmenuname, "bluetoothmenu") == 0){
		switch(menupos){
			case 0:
				oled_setDisplayedSplash(oled_LoadingSplash, "Skenuji");
				scanDev = 1;
			break;

			case 1:

			break;

			case 2:
				oled_setDisplayedMenu("settingsmenu",&settingsmenu, sizeof(settingsmenu), 0);
			break;

			default:
			break;
		}
	}else if(strcmp(dispmenuname, "btScanedDevices") == 0){
		if(menupos == btScannedCount){
				oled_setDisplayedMenu("bluetoothmenu",&bluetoothmenu, sizeof(bluetoothmenu), 0);
		}
	}else if(strcmp(dispmenuname, "btBondedDevicesMenu") == 0){
		if(menupos == btBondedCount){
			oled_setDisplayedMenu("mainmenu",&mainmenu, sizeof(mainmenu), 0);
		}
	}else if(strcmp(dispmenuname, "organmenu") == 0){
		switch(menupos){
					case 0:
						midiControl_currentOn();
					break;

					case 1:
						midiControl_currentOff();
					break;

					case 2:
						oled_setDisplayedMenu("mainmenu",&mainmenu, sizeof(mainmenu), 0);
					break;

					default:
					break;
				}
	}

	encoderclick = 0;

	if(strcmp(dispmenuname, menunameold) != 0) encoderpos = 0;
}

void oled_begin(){
	//inicializuje se driver oled
	ssd1306_Init();
	//Zapne se obnova OLED
	oled_setDisplayedMenu("mainmenu", &mainmenu, sizeof(mainmenu), 0);
	//oled_setDisplayedSplash(oled_StartSplash, "");
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim4);
	oledHeader = (char*)malloc(50);
	encoderpos = 0;
	encoderposOld = -1;
	scrollPause = 0;
	scrollPauseDone = 0;
}

void oled_refresh(){
	ssd1306_Fill(0);
	if(oledType == OLED_MENU){
		oled_drawMenu();
	}else if(oledType == OLED_SPLASH){
		(*splashFunction)(splashTxt);
		ssd1306_UpdateScreen(0);
	}

}

void oled_setDisplayedMenu(char *menuname ,struct menuitem (*menu)[], int menusize, int issubmenu){
	//HAL_TIM_Base_Stop_IT(&htim2);
	dispmenuname = malloc(strlen(menuname)+1);
	dispmenusize = menusize/sizeof(struct menuitem);
	memcpy(dispmenuname, menuname, strlen(menuname)+1);
	memcpy(&dispmenu, menu, menusize);
	dispmenusubmenu = issubmenu;
	encoderposOld = -1;
	oledType = OLED_MENU;
	//HAL_TIM_Base_Start_IT(&htim2);
}

void oled_setDisplayedSplash(void (*funct)(), char * txt){
	//HAL_TIM_Base_Stop_IT(&htim2);
	loadingStat = 1;
	splashFunction = funct;
	splashTxt = txt;
	oledType = OLED_SPLASH;
	//HAL_TIM_Base_Start_IT(&htim2);

}

void oled_drawMenu(){

	if(encoderclick){
		oled_menuOnclick(encoderpos);
		/*if(dispmenu[encoderpos].submenu != 0){
			//oled_setDisplayedMenu(dispmenu[encoderpos].submenuname, &dispmenu[encoderpos].submenu, sizeof(dispmenu[encoderpos].submenu));
		}*/
	}

	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

	//sprintf(oledHeader, "%d.%d %d %ld:%ld:%02d",date.Date, date.Month, date.Year, time.SecondFraction, time.SubSeconds, time.Seconds);
	//sprintf(oledHeader, "E: %d N: %s", encoderpos, dispmenu[encoderpos].name);
	//sprintf(oledHeader, "Disp: %d", HAL_GPIO_ReadPin(DISP_SENSE_GPIO_Port, DISP_SENSE_Pin));
	//oledHeader = "MIDIControll 0.1";
	//sprintf(oledHeader, "%d %d %d", HAL_GPIO_ReadPin(MIDI_ACTIVE_GPIO_Port, MIDI_ACTIVE_Pin), HAL_GPIO_ReadPin(MIDI_SEARCHING_GPIO_Port, MIDI_SEARCHING_Pin), HAL_GPIO_ReadPin(MIDI_IO_SELECTED_GPIO_Port, MIDI_IO_SELECTED_Pin));
	sprintf(oledHeader, "E: %d", loadingStat);
	ssd1306_SetCursor(2,0);
	ssd1306_WriteString(oledHeader, Font_7x10, White);

	for(uint8_t i = 0; i <= 128; i++) ssd1306_DrawPixel(i, 13, White);


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
			/*ssd1306_SetCursor(OLED_MENU_LEFT_PADDING, (i-encoderpos+1)*OLED_MENU_TEXT_HEIGHT + OLED_MENU_TOP_PADDING);
			ssd1306_WriteChar(33-(dispmenu[i].selected), Icon_11x18, White);*/

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


	ssd1306_UpdateScreen(0);

}

void oled_StartSplash(){
	ssd1306_SetCursor(3, 10);
	ssd1306_WriteString("MIDIControl", Font_11x18, White);

	ssd1306_SetCursor(42,30);
	ssd1306_WriteString("BASE", Font_11x18, White);

	char * version = "Verze 1.1";
	ssd1306_SetCursor((128-(strlen(version)-1)*7)/2,50);
	ssd1306_WriteString(version, Font_7x10, White);

}

void oled_LoadingSplash(char * msg){
	ssd1306_SetCursor((128-(strlen(msg)-1)*11)/2, 15);
	ssd1306_WriteString(msg, Font_11x18, White);

	ssd1306_SetCursor(42,35);
	ssd1306_WriteChar(33 - (loadingStat & 0x01), Icon_11x18, White);
	ssd1306_SetCursor(64,35);
	ssd1306_WriteChar(33 - ((loadingStat>>1) & 0x01), Icon_11x18, White);
	ssd1306_SetCursor(86,35);
	ssd1306_WriteChar(33 - ((loadingStat>>2) & 0x01), Icon_11x18, White);


}

