//
// AnyLedger - Created by Bogdan Djukic on 11-04-2018. Inspired by Okada Takahiro, https://github.com/kopanitsa/web3-arduino.
// Licensed under GNU GENERAL PUBLIC LICENSE.
//

#include <vector>
#include <stdint.h>
using namespace std;
class Transaction {

public:
    Transaction();

    void setPrivateKey(const uint8_t *key);

vector<uint8_t> getRaw(vector<uint8_t> nonce, vector<uint8_t> gasPrice, vector<uint8_t>  gasLimit,
                                    vector<uint8_t> to, vector<uint8_t> value, vector<uint8_t> data, uint32_t chainId);

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

void GenerateSignature(uint8_t* signature,int* recoveryId,
                       vector<uint8_t> nonce, vector<uint8_t> gasPrice, vector<uint8_t>  gasLimit,
                       vector<uint8_t> to, vector<uint8_t> value, vector<uint8_t> data, uint32_t chainId);

vector<uint8_t> RlpEncode(vector<uint8_t> nonce, vector<uint8_t> gasPrice, vector<uint8_t>  gasLimit,
                          vector<uint8_t> to, vector<uint8_t> value, vector<uint8_t> data, uint32_t chainIdStr) ;

vector<uint8_t> RlpEncodeForRawTransaction( vector<uint8_t> nonce, vector<uint8_t> gasPrice, vector<uint8_t>  gasLimit,
                             vector<uint8_t> to, vector<uint8_t> value, vector<uint8_t> data, uint32_t chainIdStr, uint8_t* sig, uint8_t recoveryId);
};
