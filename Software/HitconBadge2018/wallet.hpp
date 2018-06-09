#ifndef _wallet_hpp_
#define _wallet_hpp_

#include "Arduino.h"

// Wallet related
extern String publicAddress_string;
#define CHAIN_ID  3 // 0x1-mainnet, 0x3-Ropsten, 0x4-Rinkeby
#define GAS_PRICE   50//50 Gwei
#define GAS_LIMIT 21000

void init_se();

uint8_t start_transaction(uint8_t* outputdata, String from,String to, String value, String data, String gasPrice, String gasLimit, String nonce);

uint64_t read_balance();

#endif // _wallet_hpp_