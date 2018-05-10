//
// AnyLedger - Created by Bogdan Djukic on 11-04-2018. Inspired by Okada Takahiro, https://github.com/kopanitsa/web3-arduino.
// Licensed under GNU GENERAL PUBLIC LICENSE.
//

#include <String>
#include <cString>
#include <stdint.h>
#include <vector>

using namespace std;

class ByteConverter {
  public:
    static vector<uint8_t> numberToBytes(uint64_t val);
    static vector<uint8_t> charStrToBytes(const uint8_t *in);
    //static vector<uint8_t> StringToBytes(const String* str);
    //static String bytesToString(const vector<uint8_t> buf);
};
