#ifndef _control_hpp_
#define _control_hpp_
#include "Arduino.h"

#define R1 2
#define R2 4.7
#define R3 1
#define V_full 3.3
#define LED A0
#define VBAT_PIN A2
#define ABUTTON_PIN A1

byte readBatteryPercentage();
/// Check if Battery Voltage
///
/// \returns Battery Voltage (float)
float readBatteryVoltage();
/// Check if any button is pressed
///
/// \returns key code such as A,B,C,or 0-9 ,D for DEL and E for Enter
/// \returns -1 (NO_BUTTON) if no button pressed.
int readButton();


#endif // _control_hpp_