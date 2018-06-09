#include "util.hpp"
#include "Arduino.h"
#include "hal_trng.h"
#include "hal_wdt.h"


String ArraytoString(uint8_t* buffer,uint32_t len,uint32_t String_len){
  if (String_len<len)
  {
    Serial.println("Error String_len< buffer len");
    return "";
  }
  String data;
  uint32_t append_0 = String_len - len;
  for (int i = 0; i < append_0; ++i)
  {
    data+= "00";
  }
  data +=ArraytoString(buffer,len);
  return data;
}


String ArraytoString(uint8_t* buffer,uint32_t len){
  String data;
  for (int i = 0; i < len; ++i)
  {
    if (buffer[i]<0x0F)
    {
      data += "0";
    }
    data += String(buffer[i],HEX);
  }
  return data;
}

void print_vector(std::vector<uint8_t> VEC){
  for(int i=0;i<VEC.size();i++){
    if(VEC[i]<16){
    Serial.print("0");
    }
    Serial.print(VEC[i],HEX);
  }
}
String vector2string(std::vector<uint8_t> VEC){
    String s;
    for(int i=0;i<VEC.size();i++){
      if(VEC[i]<16){
        s+="0";
      }
      s+=String(VEC[i],16);
    }
  return s;
}
void randomNumbergenerator_init(){
  hal_trng_status_t ret = hal_trng_init();
}
uint32_t randomNumbergenerator(uint8_t digit){
  uint32_t number = 0;
  uint32_t max = pow(10,digit);
  
  hal_trng_get_generated_random_number(&number); 
  return number%max;
}

uint32_t randomUint32_t_generator(){
  uint32_t number = 0;
  hal_trng_get_generated_random_number(&number); 
  return number;
}


void random_UUID_generator(char* array){
  uint32_t number[4] = {0};
  for (int i = 0; i < 4; ++i)
  {
    hal_trng_get_generated_random_number(&number[i]); 
  }

  uint16_t buffer[4] = {0};
  buffer[0] = number[1]>>16;
  buffer[1] = number[1];
  buffer[2] = number[2]>>16;
  buffer[3] = number[2];
  

  sprintf (array, "%08x-%04x-%04x-%04x-%04x%08x",number[0], buffer[0], buffer[1],buffer[2],buffer[3], number[3]);
  //Serial.println((char*)array);
}

void WDT_Reset(){
  Serial.println("Reset");
  pinMode(A0,OUTPUT);
  hal_wdt_software_reset();
  while(1){
    digitalWrite(A0,HIGH);
    delay(100);
    digitalWrite(A0,LOW);
    delay(100);

  }
}