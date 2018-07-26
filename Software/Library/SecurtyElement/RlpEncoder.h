//
// AnyLedger - Created by Bogdan Djukic on 11-04-2018. Inspired by Okada Takahiro, https://github.com/kopanitsa/web3-arduino.
// Licensed under GNU GENERAL PUBLIC LICENSE.
//

#include <String>
#include <cString>
#include <stdint.h>
#include <vector>

using namespace std;

class RlpEncoder {
  public:
    static uint32_t encodeWholeHeader(uint8_t *header_output, uint32_t total_len);
    static vector<uint8_t> encodeWholeHeaderWithVector(uint32_t total_len);
    static uint32_t encodeItem(uint8_t* output, const uint8_t* input, uint32_t input_len);
    static vector<uint8_t> encodeItemWithVector(const vector<uint8_t> input);
};
