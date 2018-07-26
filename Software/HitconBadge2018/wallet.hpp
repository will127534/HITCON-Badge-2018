#ifndef _wallet_hpp_
#define _wallet_hpp_

#include "Arduino.h"
#define SE_SS 10
// Wallet related
extern String publicAddress_string;
#define CHAIN_ID  3 // 0x1-mainnet, 0x3-Ropsten, 0x4-Rinkeby
#include <vector>
#include <Transaction.h>
#include <wallet.h>
extern wallet hardware_wallet;

typedef struct erc20_struct
{
  uint8_t Contract_Address[20];
  char Name[4];
};
extern std::vector<erc20_struct> erc20s;
extern char* SE_Mode[];
void init_se();
int BindReg();
int BindLogin(uint8_t* new_pin);
int BindLogout(void);
//int BackToNoHost();
int BackToNoHost(uint8_t *PIN);
int BackToInit(void);
int InitWalletFromMnemonic(String Memonic);
int InitWalletFromMnemonic_withpassphrase(String mnemonic,String passphrase);
int InitDevInfo(void);
int PIN_AuthTest(uint8_t *inputPIN);
int hdw_query_wallet_info(void);
int hdw_create_account(uint8_t id);
int hdw_create_account(uint32_t cointype,uint32_t account,char* account_name);
int hdw_query_all_account_info(void);
int hdw_query_account_info(uint8_t id);
int hdw_query_account_info_cmdline(uint32_t account);
int hdw_query_account_key_info(uint8_t id);
int hdw_query_account_key_info(uint32_t account,uint32_t key);
int hdw_generate_next_trx_addr(uint8_t id);
int hdw_generate_next_trx_addr_account(uint32_t account);
void reinit_secureelement(String Memonic);
uint8_t start_transaction(uint8_t* outputdata,transaction_struct trx);
uint8_t start_transaction_cmdline(uint8_t* outputdata,uint32_t account,uint32_t key,transaction_struct trx_data);
uint64_t read_balance();
void quary_erc20s();
void addERC20(erc20_struct erc20);
void printERC20(erc20_struct erc20);
#endif // _wallet_hpp_
