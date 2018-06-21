#include "control.hpp"
#include "Arduino.h"

//Analog Button
int buttonArray[5][3] = {
  {0xA,0xB,0xC},
  {7,4,1},
  {8,5,2},
  {9,6,3},
  {0,0xD,0xE},
};




/// Check if Battery Voltage
///
/// \returns Battery Voltage (float)
float readBatteryVoltage(){
  return (float)analogRead(A2)/4096.0*5.0;
}
/// Check if any button is pressed
///
/// \returns key code such as A,B,C,or 0-9 ,D for DEL and E for Enter
/// \returns -1 (NO_BUTTON) if no button pressed.
int readButton(){
  long voltage = 0;
  int average = 10;
  for (int i = 0; i < average; ++i)
  {
    voltage += analogRead(A1);
  }
  voltage = voltage/average;
  
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 5; ++j)
    {
      float resistor_array = R3/(i*R1+j*R2+R3);
      int V_matrix = V_full*resistor_array/2.5*4095;
      if (V_matrix>4095)
      {
        V_matrix = 4095;
      }

      if (abs(V_matrix-voltage)/float(V_matrix)*100.0 <2.5)
      {
        //Serial.println(abs(V_matrix-voltage)/float(V_matrix)*100.0);
        //Serial.println(buttonArray[j][i],HEX);
        return buttonArray[j][i];
      }

  }
}
return -1;
}
