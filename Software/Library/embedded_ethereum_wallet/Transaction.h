//
// AnyLedger - Created by Bogdan Djukic on 11-04-2018. Inspired by Okada Takahiro, https://github.com/kopanitsa/web3-arduino.
// Licensed under GNU GENERAL PUBLIC LICENSE.
//

#include <vector>
#include <stdint.h>
using namespace std;
class Transaction {

public:
    typedef struct {
        char from[80];
        char to[80];
        char gasPrice[20];
        long gas;
    } Options;

    Options options;

public:
    Transaction();

    void setPrivateKey(const uint8_t *key);

    vector<uint8_t> getRaw(uint32_t nonceVal,
                  uint64_t gasPriceVal,
                  uint32_t  gasLimitVal,
                  uint8_t *toStr,
                  uint8_t *valueStr,
                  uint8_t *dataStr,
                  uint32_t chainId);

private:
    const uint8_t * privateKey;

private:
  /*
    String GenerateBytesForInt(const int32_t value);
    String GenerateBytesForUint(const uint32_t value);
    String GenerateBytesForAddress(const String *value);
    String GenerateBytesForString(const String *value);
    String GenerateBytesForBytes(const char* value, const int len);
    String String_format(const char* format, ...);*/
    void Sign(uint8_t* hash, uint8_t* sig, int* recid);

    void GenerateSignature(uint8_t* signature,
                           int* recoveryId,
                           uint32_t nonceVal,
                           uint64_t gasPriceVal,
                           uint32_t  gasLimitVal,
                           uint8_t* toStr,
                           uint8_t* valueStr,
                           uint8_t* dataStr,
                           uint32_t chainId);

    vector<uint8_t> RlpEncode(uint32_t nonceVal,
                              uint64_t gasPriceVal,
                              uint32_t  gasLimitVal,
                              uint8_t* toStr,
                              uint8_t* valueStr,
                              uint8_t* dataStr,
                              uint32_t chainId);

    vector<uint8_t> RlpEncodeForRawTransaction(uint32_t nonceVal,
                                               uint64_t gasPriceVal,
                                               uint32_t  gasLimitVal,
                                               uint8_t* toStr,
                                               uint8_t* valueStr,
                                               uint8_t* dataStr,
                                               uint32_t chainId,
                                               uint8_t* sig,
                                               uint8_t recoveryId);
};
