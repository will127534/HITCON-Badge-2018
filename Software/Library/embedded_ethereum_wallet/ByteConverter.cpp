//
// AnyLedger - Created by Bogdan Djukic on 11-04-2018. Inspired by Okada Takahiro, https://github.com/kopanitsa/web3-arduino.
// Licensed under GNU GENERAL PUBLIC LICENSE.
//

#include "ByteConverter.h"
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include "Arduino.h"

vector<uint8_t> ByteConverter::numberToBytes(uint64_t val) {
    vector<uint8_t> tmp;
    vector<uint8_t> ret;
    if ((uint64_t)(val / 256) >= 0) {
        while ((uint64_t)(val / 256) > 0) {
            tmp.push_back((uint8_t)(val % 256));
            val = (uint64_t)(val / 256);
        }
        tmp.push_back((uint8_t)(val % 256));
        uint8_t len = tmp.size();
        for (int i=0; i<len; i++) {
            ret.push_back(tmp[len-i-1]);
        }
    } else {
        ret.push_back((uint8_t)val);
    }
    return ret;
}

vector<uint8_t> ByteConverter::charStrToBytes(const uint8_t *in) {
    uint32_t ret = 0;
    uint8_t tmp[256];
    strcpy((char *)tmp, (char *)in);
    vector<uint8_t> out;

    // Remove "0x"
    char * ptr = strtok((char*)tmp, "x");
    if (strlen(ptr)!=1) {
        ptr = (char *)tmp;
    } else {
        ptr = strtok(NULL, "x");
    }

    size_t lenstr = strlen(ptr);

    for (int i=0; i<lenstr; i+=2) {
        char c[3];
        c[0] = *(ptr+i);
        c[1] = *(ptr+i+1);
        c[2] = 0x00;
        uint8_t val = strtol(c, nullptr, 16);
        out.push_back(val);
        ret++;
    }
    return out;
}
/*
vector<uint8_t> ByteConverter::StringToBytes(const String* str) {
    return charStrToBytes((uint8_t*)(str->c_str()));
}

String ByteConverter::bytesToString(const vector<uint8_t> buf) {
    String ret = "0x";

    for (int i = 0; i < buf.size(); i++) {
      char v[3];
      memset(v, 0, sizeof(v));
      sprintf(v, "%02x", buf[i]);

      ret = ret + String(v);
    }

    return ret;
}
*/