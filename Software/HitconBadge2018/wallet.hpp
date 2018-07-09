#ifndef _wallet_hpp_
#define _wallet_hpp_

#include "Arduino.h"

// Wallet related
extern String publicAddress_string;
#define CHAIN_ID  3 // 0x1-mainnet, 0x3-Ropsten, 0x4-Rinkeby
#include <vector>
struct transaction_struct
{
  std::vector<uint8_t> to;
  std::vector<uint8_t> value;
  std::vector<uint8_t> gasPrice;
  std::vector<uint8_t> gasLimit;
  std::vector<uint8_t> nonce;
  std::vector<uint8_t> data;
};

struct erc20_struct
{
  uint8_t Contract_Address[20];
  uint8_t TotalSupply[4];
  uint8_t BalanceOf[4];
  uint8_t Transfer[4];
  uint8_t TransferFrom[4];
  uint8_t Approve[4];
  uint8_t Name[5];
};

void init_se();

uint8_t start_transaction(uint8_t* outputdata,transaction_struct trx);
uint64_t read_balance();
void quary_erc20s();
void printERC20(erc20_struct erc20);
#endif // _wallet_hpp_
