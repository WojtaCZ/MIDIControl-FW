#ifndef oled_h
#define oled_h

#include "tim.h"
#include "encoder.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "menus.h"
#include "bluetooth.h"

#define OLED_MENU_TOP_PADDING 3
#define OLED_MENU_LEFT_PADDING 2

#define OLED_MENU_TEXT_WIDTH 11
#define OLED_MENU_TEXT_HEIGHT 18

#define OLED_MENU_SCROLL_PAUSE 2

#define OLED_MENU	2
#define OLED_SPLASH	3

extern struct menuitem mainmenu[];



//Inicializuje se menu o 50 prvcích
struct menuitem dispmenu[50];
int dispmenusize;
int dispmenusubmenu;
char* dispmenuname;

void (*splashFunction)(void*);
void * splashParams;

#define APP_DISPLAY		0
#define APP_RECORD		1

struct reqNumber{
	uint8_t application;
	uint8_t digits;
	uint8_t selectedDigit;
	long int enteredNumber;
	char * message;
};

struct reqValue{
	uint8_t application;
	uint8_t digits;
	uint8_t selectedDigit;
	char enteredValue[30];
	char * message;
	char * characters;
	uint8_t charactersLen;
};


int scrollIndex, scrollMax, scrollPauseDone, scrollPause, oledType, loadingStat, refreshHalt, loadingToggle;

char *oledHeader;

void oled_begin();
void oled_refresh();
void oled_drawMenu();
void oled_setDisplayedMenu(char *menuname ,struct menuitem (*menu)[], int menusize, int issubmenu);
void oled_menuOnclick(int menupos);
void oled_StartSplash();
void oled_UsbWaitingSplash();
void oled_LoadingSplash(char * msg);
void oled_BtDevPairCompleteSplash(char * msg);
void oled_BtDevInfoSplash(struct btDevice * dev);
void oled_BtDevPairRequestSplash(struct btDevice * dev);
void oled_BtDevPairAckSplash(struct btDevice * dev);
void oled_NothingFound();
void oled_DisplayStatusSplash();
void oled_setDisplayedSplash(void (*funct)(), void * params);
void oled_playingSplash(char * songname);
void oled_recordingSplash(char * songname);
void oled_refreshPause();
void oled_refreshResume();
void oled_ErrorSplash(char * msg);
void oled_MIDIstatusSplash();
void oled_NameExistsSplash();

void oled_NumberEnterSplash(struct reqNumber * num);
void oled_ValueEnterSplash(struct reqValue * num);

#endif
