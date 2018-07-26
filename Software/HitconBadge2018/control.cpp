#include "control.hpp"
#include "Arduino.h"
#include <vector>
#include <algorithm>  
#include <hal_adc.h>
//Analog Button
int buttonArray[5][3] = {
  {0xA,0xB,0xC},
  {7,4,1},
  {8,5,2},
  {9,6,3},
  {0,0xD,0xE},
};
float V_in = V_full;
//#define DEBUG_CONTROL

#ifdef DEBUG_CONTROL
#define DEBUG_PRINT Serial.print
#define DEBUG_PRINTLN Serial.println
#else
#define DEBUG_PRINT 
#define DEBUG_PRINTLN 
#endif

/// Check if Battery Percentage
/// Just using simple mapping method
/// \returns Battery Percentage 0-100 byte
byte readBatteryPercentage(){
  float voltage = readBatteryVoltage();
  DEBUG_PRINT("Raw Voltage:");DEBUG_PRINT(voltage);DEBUG_PRINTLN("V");
  long voltage_int = voltage*1000-2400;
  if(voltage_int<0){
    voltage_int = 0;
  }
  DEBUG_PRINT("voltage int.");DEBUG_PRINTLN(voltage_int);
  byte Percentage = map(voltage_int, 0, 3200-2400, 0, 100);
  DEBUG_PRINT("VBAT Percentage:");DEBUG_PRINTLN(Percentage);

  return Percentage;
}


/// Check if Battery Voltage
///
/// \returns Battery Voltage (float)
float readBatteryVoltage(){
  std::vector<uint16_t> voltage_array;
  int average = 35;
  for (int i = 0; i < average; ++i)
  {
    uint32_t temp = analogRead(VBAT_PIN);
    voltage_array.push_back(temp);
  }
  std::sort (voltage_array.begin(), voltage_array.end());
  voltage_array.erase (voltage_array.begin(),voltage_array.begin()+5);
  voltage_array.erase (voltage_array.end()-20,voltage_array.end());


  uint32_t voltage = 0;
  for (int i=0; i<voltage_array.size(); ++i){
    voltage+=voltage_array[i];
    DEBUG_PRINT(voltage_array[i]);DEBUG_PRINT(" ");
  }
  DEBUG_PRINTLN("");

  voltage = voltage/(average-25);
  DEBUG_PRINT("VBAT INT:");DEBUG_PRINTLN(voltage);
  return (float)voltage/4096.0*5.0;
}


/// Check if any button is pressed
///
/// \returns key code such as A,B,C,or 0-9 ,D for DEL and E for Enter
/// \returns -1 (NO_BUTTON) if no button pressed.
int readButton(){
  std::vector<uint16_t> voltage_array;
  int average = 35;
  for (int i = 0; i < average; ++i)
  {
    uint32_t temp = analogRead(ABUTTON_PIN);
    voltage_array.push_back(temp);
  }
  std::sort (voltage_array.begin(), voltage_array.end());
  voltage_array.erase (voltage_array.begin(),voltage_array.begin()+5);
  voltage_array.erase (voltage_array.end()-20,voltage_array.end());


  uint32_t voltage = 0;
  for (int i=0; i<voltage_array.size(); ++i){
    voltage+=voltage_array[i];
    DEBUG_PRINT(voltage_array[i]);DEBUG_PRINT(" ");
  }
  DEBUG_PRINTLN("");

  voltage = voltage/(average-25);

  for (int i=0; i<voltage_array.size(); ++i){
    if(abs((int)voltage_array[i] - (int)voltage) > 10){
      DEBUG_PRINT("Error too large! ");
      DEBUG_PRINT(voltage_array[i]);DEBUG_PRINT(" ");DEBUG_PRINTLN(voltage);
      voltage = voltage-voltage_array[i]/(average-25);
      //return -1;
    }
    DEBUG_PRINT(voltage_array[i]);DEBUG_PRINT(" ");
  }

  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 5; ++j)
    {
      float resistor_array = R3/(i*R1+j*R2+R3);
      int V_matrix = V_in*resistor_array/2.5*4095;
      if (V_matrix>4095)
      {
        V_matrix = 4095;
      }
      if ( float(abs(V_matrix-(int)voltage))/float(V_matrix)*100.0 <2.5 )
      {
        DEBUG_PRINTLN("Button Get");
        DEBUG_PRINTLN(float(abs(V_matrix-(int)voltage))/float(V_matrix)*100.0);
        DEBUG_PRINTLN(buttonArray[j][i],HEX);
        DEBUG_PRINT("Array:");
        for (int i=0; i<voltage_array.size(); ++i){
          DEBUG_PRINT(voltage_array[i]);DEBUG_PRINT(" ");
        }
        DEBUG_PRINTLN("");
        // auto cal: when user push ENTER, it will auto cal 3.3V
        if (buttonArray[j][i] == 0x06 && float(abs(V_matrix-(int)voltage))/float(V_matrix)*100.0 < 1 )
        {
          V_in = voltage/resistor_array*2.5/4095;
        }
        if (buttonArray[j][i] == 0x0E && float(abs(V_matrix-(int)voltage))/float(V_matrix)*100.0 < 1 )
        {
          V_in = voltage/resistor_array*2.5/4095; 
        }
        return buttonArray[j][i];
      }

  }
}
  return -1;
}
