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



/// Check if Battery Voltage
///
/// \returns Battery Voltage (float)
byte readBatteryPercentage(){
  float voltage = readBatteryVoltage();
  Serial.print("Raw Voltage:");Serial.print(voltage);Serial.println("V");
  long voltage_int = voltage*1000-2400;
  if(voltage_int<0){
    voltage_int = 0;
  }
  Serial.print("voltage int.");Serial.println(voltage_int);
  byte Percentage = map(voltage_int, 0, 3200-2400, 0, 100);
  Serial.print("VBAT Percentage:");Serial.println(Percentage);
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
    //Serial.print(temp);Serial.print(" ");
  }
  //Serial.println();
  std::sort (voltage_array.begin(), voltage_array.end());
  voltage_array.erase (voltage_array.begin(),voltage_array.begin()+5);
  voltage_array.erase (voltage_array.end()-20,voltage_array.end());


  uint32_t voltage = 0;
  for (int i=0; i<voltage_array.size(); ++i){
    voltage+=voltage_array[i];
    //Serial.print(voltage_array[i]);Serial.print(" ");
  }
  //Serial.println();

  voltage = voltage/(average-25);
  Serial.print("VBAT INT:");Serial.println(voltage);
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
    //Serial.print(temp);Serial.print(" ");
  }
  //Serial.println();
  std::sort (voltage_array.begin(), voltage_array.end());
  voltage_array.erase (voltage_array.begin(),voltage_array.begin()+5);
  voltage_array.erase (voltage_array.end()-20,voltage_array.end());


  uint32_t voltage = 0;
  for (int i=0; i<voltage_array.size(); ++i){
    voltage+=voltage_array[i];
    //Serial.print(voltage_array[i]);Serial.print(" ");
  }
  //Serial.println();

  voltage = voltage/(average-25);

  for (int i=0; i<voltage_array.size(); ++i){
    if(abs((int)voltage_array[i] - (int)voltage) > 10){
      //Serial.print("Error too large! ");
     // Serial.print(voltage_array[i]);Serial.print(" ");Serial.println(voltage);
      voltage = voltage-voltage_array[i]/(average-25);
      //return -1;
    }
    //Serial.print(voltage_array[i]);Serial.print(" ");
  }



  
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
        Serial.println("Button Get");
        Serial.println(abs(V_matrix-voltage)/float(V_matrix)*100.0);
        Serial.println(buttonArray[j][i],HEX);
        //Serial.print("Array:");
        //for (int i=0; i<voltage_array.size(); ++i){
        //  Serial.print(voltage_array[i]);Serial.print(" ");
       // }
        //Serial.println("");
        return buttonArray[j][i];
      }

  }
}
  return -1;
}
