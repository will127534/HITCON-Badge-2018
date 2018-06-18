#ifndef _epddisplay_hpp_
#define _epddisplay_hpp_
//e-Paper

#include <GxEPD.h>
#include <GxGDE0213B1/GxGDE0213B1.h>
//e-Paper

extern GxGDE0213B1 display;

void init_display();
//Battery_voltage ... etc
void display_static();
void main_menu();
void setting_menu();
void BLE_pairing();
void display_paringQR();
#endif // _epddisplay_hpp_