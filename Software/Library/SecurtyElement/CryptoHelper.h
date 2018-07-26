//
// AnyLedger - Created by Bogdan Djukic 22-03-2018. Inspired by Firefly: https://github.com/firefly/wallet.
// Licensed under GNU GENERAL PUBLIC LICENSE.
//

#include <String>
#include <cString>
#include <stdint.h>
#include <vector>

// "0x" + (40 bytes address) + "\0"
#define CRYPTO_HELPER_CHECKSUM_ADDRESS_LENGTH (2 + 40 + 1)

using namespace std;

class CryptoHelper {
  public:
    static vector<uint8_t> generatePrivateKey();
    static vector<uint8_t> generateAddress(const uint8_t *privateKey);
    static vector<char> generateChecksumAddress(vector<uint8_t> publicKey);
    static vector<uint8_t> keccak256(const uint8_t *data, uint16_t length);
};
