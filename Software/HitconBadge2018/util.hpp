#ifndef _util_hpp_
#define _util_hpp_

#include <vector>
#include "Arduino.h"
String ArraytoString(uint8_t* buffer,uint32_t len,uint32_t String_len);
String ArraytoString(uint8_t* buffer,uint32_t len);
void print_vector(std::vector<uint8_t> VEC);
String vector2string(std::vector<uint8_t> VEC);
void randomNumbergenerator_init();
uint32_t randomNumbergenerator(uint8_t digit);
uint32_t randomUint32_t_generator();
void random_UUID_generator(char* array);
void random_UUID_generator_head(char* array);
void WDT_Reset();
#endif // _util_hpp_