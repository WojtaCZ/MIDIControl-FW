#ifndef menus_h
#define menus_h

struct menuitem{
	char* name;
	int selected;
	FontDef* font;
	int hasSpecialSelector;
	int specharNotSelected;
	int specharSelected;
	int submenuLevel;
	char **parentItem;
	/*char* submenuname;*/

};

#endif
