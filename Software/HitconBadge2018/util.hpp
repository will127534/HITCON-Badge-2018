#ifndef _util_hpp_
#define _util_hpp_

#include <vector>
#include "Arduino.h"
String ArraytoString(uint8_t* buffer,uint32_t len,uint32_t String_len);
String ArraytoString(uint8_t* buffer,uint32_t len);
void print_vector(std::vector<uint8_t> VEC);
String vector2string(std::vector<uint8_t> VEC);
uint32_t randomNumbergenerator(uint8_t digit);

#endif // _util_hpp_