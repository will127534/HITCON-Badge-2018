#include "wallet.hpp"
#include "Arduino.h"
#include "BLE.hpp"
#include "Transaction.h"
#include "CryptoHelper.h"
#include "LFlash.h"
#include "util.hpp"
 
const uint8_t PRIVATE_KEY[] = {0x1e,0x4a,0xf8,0x55,0x62,0x0a,0x81,0x0c,0x22,0x77,0xaf,0x71,0x1d,0x47,0x7e,0x7c,0x82,0x6c,0x70,0x4c,0xfb,0x1b,0x72,0x51,0xca,0xd4,0xa3,0xfd,0x1f,0x6d,0x0b,0xfa};
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
    Balance_Gatt.setValue(read_balance());
    WalletAddress_Gatt.setValueBuffer(&publicAddress[0],20);
}
String start_transaction(String from,String to, String value, String data, uint64_t gasPrice, uint32_t gasLimit, uint32_t nonce){
  
    Transaction transaction;
    transaction.setPrivateKey((uint8_t*)PRIVATE_KEY);

    vector<uint8_t> rawTransaction = transaction.getRaw(nonce, gasPrice, gasLimit, (uint8_t*)to.c_str(), (uint8_t*)value.c_str(), (uint8_t*)data.c_str(), CHAIN_ID);
    String raw_tx = vector2string(rawTransaction);
    Serial.print("Raw Tx:");
    Serial.println(raw_tx);
    return raw_tx;
}

uint64_t read_balance(){
  uint64_t balance = 0;
  uint32_t size;
  LFlash.read("Wallet", "BLANCE", (uint8_t *)&balance, &size);
  return balance;
}