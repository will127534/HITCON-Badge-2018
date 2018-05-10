//
// AnyLedger - Created by Bogdan Djukic 22-03-2018. Inspired by Firefly: https://github.com/firefly/wallet.
// Licensed under GNU GENERAL PUBLIC LICENSE.
//

#include "CryptoHelper.h"

#include "keccak256.h"
#include "types.h"
#include "uECC.h"
#include "ByteConverter.h"
 #include <stdint.h>



#include <cstdlib>
#include <vector>
#include <stdio.h>


vector<uint8_t> CryptoHelper::generatePrivateKey() {
  vector<uint8_t> privateKey;

  for (size_t index = 0; index < 32; index++) {
    privateKey.push_back(123);
  }

  return privateKey;
}

vector<uint8_t> CryptoHelper::keccak256(const uint8_t *data, uint16_t length) {
    uint8_t result[32];
    SHA3_CTX context;

    keccak_init(&context);
    keccak_update(&context, (const unsigned char*)data, (size_t)length);
    keccak_final(&context, (unsigned char*)result);

    // Clear out the contents of what we hashed (in case it was secret)
    memset((char*)&context, 0, sizeof(SHA3_CTX));

    return vector<uint8_t>(std::begin(result), std::end(result));
}

vector<uint8_t> CryptoHelper::generateAddress(const uint8_t *privateKey) {
  uint8_t publicKey[64];
  uint8_t publicAddress[20];

  bool success = uECC_compute_public_key(privateKey, publicKey, uECC_secp256k1());

  vector<uint8_t> hashed = CryptoHelper::keccak256(publicKey, 64);

  memcpy(publicAddress, &hashed[12], 20);

  return vector<uint8_t>(std::begin(publicAddress), std::end(publicAddress));
}

static char getHexNibble(uint8_t value) {
    value &= 0x0f;

    if (value <= 9) {
      return '0' + value;
     }

    return 'a' + (value - 10);
}

vector<char> CryptoHelper::generateChecksumAddress(vector<uint8_t> publicKey) {
  uint8_t end = CRYPTO_HELPER_CHECKSUM_ADDRESS_LENGTH - 1;

  vector<char> checksumAddress(CRYPTO_HELPER_CHECKSUM_ADDRESS_LENGTH);

  // Null Termination
  checksumAddress[end--] = 0;

  // Expand the address into a lowercase-ascii-nibble representation
  // We go backwads, so don't clobber the address
  for (int8_t i = 20 - 1; i >= 0; i--) {
      checksumAddress[end--] = getHexNibble(publicKey[i]);
      checksumAddress[end--] = getHexNibble(publicKey[i] >> 4);
  }

  // "0x" prefix
  checksumAddress[end--] = 'x';
  checksumAddress[end--] = '0';

  // Compute the hash of the address
  vector<uint8_t> checksumAddressBytes = ByteConverter::charStrToBytes((const uint8_t*)&checksumAddress[0]);
  vector<uint8_t> hashed = CryptoHelper::keccak256(&checksumAddressBytes[2], 40);

  // Do the checksum
  for (uint8_t i = 0; i < 40; i += 2) {
      if (checksumAddress[2 + i] >= 'a' && (hashed[i >> 1] >> 4) >= 8) {
          checksumAddress[2 + i] -= 0x20;
      }
      if (checksumAddress[2 + i + 1] >= 'a' && (hashed[i >> 1] & 0x0f) >= 8) {
          checksumAddress[2 + i + 1] -= 0x20;
      }
  }

  return checksumAddress;
}
