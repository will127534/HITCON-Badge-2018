#ifndef _Transaction_h_
#define _Transaction_h_

//
// AnyLedger - Created by Bogdan Djukic on 11-04-2018. Inspired by Okada Takahiro, https://github.com/kopanitsa/web3-arduino.
// Licensed under GNU GENERAL PUBLIC LICENSE.
//

#include <vector>
#include <stdint.h>
using namespace std;

struct transaction_struct
{
  std::vector<uint8_t> to;
  std::vector<uint8_t> value;
  std::vector<uint8_t> gasPrice;
  std::vector<uint8_t> gasLimit;
  std::vector<uint8_t> nonce;
  std::vector<uint8_t> data;
};

class Transaction {

public:
    Transaction();
    vector<uint8_t> GenerateSignature(transaction_struct trx);

    vector<uint8_t> RlpEncode(transaction_struct trx) ;

    vector<uint8_t> RlpEncodeForRawTransaction( transaction_struct trx, uint8_t* sig, uint8_t recoveryId);
};
#endif
