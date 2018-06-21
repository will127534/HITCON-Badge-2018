#include "wallet.hpp"
#include "Arduino.h"
#include "BLE.hpp"
#include "Transaction.h"
#include "CryptoHelper.h"
#include "LFlash.h"
#include "util.hpp"
 
const uint8_t PRIVATE_KEY[] = {0x93, 0x51, 0xdd, 0x97, 0x79, 0x42, 0x8c, 0x6f, 0x8f, 0x17, 0xa4, 0x22, 0x16, 0x76, 0x4e, 0x75, 0xfa, 0x26, 0xc1, 0xe3, 0xad, 0x08, 0xf0, 0x20, 0x64, 0xae, 0xcb, 0xf1, 0xef, 0x27, 0x5e, 0x60};
String publicAddress_string;
void init_se(){
    Serial.print("Private Key:");
    for(int i=0;i<sizeof(PRIVATE_KEY);i++){
      Serial.print(PRIVATE_KEY[i],HEX);
    }
    Serial.println();
    // First, we need to create public address from the private one
    vector<uint8_t> publicAddress = CryptoHelper::generateAddress(PRIVATE_KEY);
    publicAddress_string = vector2string(publicAddress);

    Serial.print("Public address:");
    print_vector(publicAddress);
    Serial.println();
    Serial.print(publicAddress_string);
    Serial.println();

    // Now we can start building up the transaction itself
    Transaction transaction;
    transaction.setPrivateKey((uint8_t*)PRIVATE_KEY);
    //Balance_Gatt.setValue(read_balance());
   // WalletAddress_Gatt.setValueBuffer(&publicAddress[0],20);
}
uint8_t start_transaction(uint8_t* outputdata, String from,String to, String value, String data, String gasPrice, String gasLimit, String nonce){
  
    Transaction transaction;
    transaction.setPrivateKey((uint8_t*)PRIVATE_KEY);

    vector<uint8_t> rawTransaction = transaction.getRaw((uint8_t*)nonce.c_str(), (uint8_t*)gasPrice.c_str(), (uint8_t*)gasLimit.c_str(), (uint8_t*)to.c_str(), (uint8_t*)value.c_str(), (uint8_t*)data.c_str(), CHAIN_ID);
    //String raw_tx = vector2string(rawTransaction);

    if (rawTransaction.size()>256)
    {
        return 0;
    }

    std::copy(rawTransaction.begin(), rawTransaction.end(), outputdata);
    Serial.print("Txn out:");
    print_vector(rawTransaction);
    Serial.println();
    return rawTransaction.size();
}

uint64_t read_balance(){
  uint64_t balance = 0;
  uint32_t size;
  LFlash.read("Wallet", "BLANCE", (uint8_t *)&balance, &size);
  return balance;
}