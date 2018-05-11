#include "util.hpp"
#include "Arduino.h"
#include "hal_trng.h"

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
uint32_t randomNumbergenerator(uint8_t digit){
  uint32_t number = 0;
  uint32_t max = pow(10,digit);
  hal_trng_status_t ret = hal_trng_init();
  hal_trng_get_generated_random_number(&number); 
  return number%max;
}