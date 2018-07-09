#include "wallet.hpp"
#include "Arduino.h"
#include "BLE.hpp"
#include "Transaction.h"
#include "CryptoHelper.h"
#include "LFlash.h"
#include "util.hpp"
#include <nvdm.h>
//const uint8_t PRIVATE_KEY[] = {0x93, 0x51, 0xdd, 0x97, 0x79, 0x42, 0x8c, 0x6f, 0x8f, 0x17, 0xa4, 0x22, 0x16, 0x76, 0x4e, 0x75, 0xfa, 0x26, 0xc1, 0xe3, 0xad, 0x08, 0xf0, 0x20, 0x64, 0xae, 0xcb, 0xf1, 0xef, 0x27, 0x5e, 0x60};
//const uint8_t PRIVATE_KEY[] = {0x95, 0x51, 0xdd, 0x97, 0x79, 0x42, 0x8c, 0x6f, 0x8f, 0x17, 0xa4, 0x22, 0x16, 0x76, 0x4e, 0x75, 0xfa, 0x26, 0xc1, 0xe3, 0xad, 0x08, 0xf0, 0x20, 0x64, 0xae, 0xcb, 0xf1, 0xef, 0x27, 0x5e, 0x60};
//const uint8_t PRIVATE_KEY[] = {0x94, 0x51, 0xdd, 0x97, 0x79, 0x42, 0x8c, 0x6f, 0x8f, 0x17, 0xa4, 0x22, 0x16, 0x76, 0x4e, 0x75, 0xfa, 0x26, 0xc1, 0xe3, 0xad, 0x08, 0xf0, 0x20, 0x64, 0xae, 0xcb, 0xf1, 0xef, 0x27, 0x5e, 0x60};
const uint8_t PRIVATE_KEY[] = {0xFF, 0x51, 0xdd, 0x97, 0x79, 0x42, 0x8c, 0x6f, 0x8f, 0x17, 0xa4, 0x22, 0x16, 0x76, 0x4e, 0x75, 0xfa, 0x26, 0xc1, 0xe3, 0xad, 0x08, 0xf0, 0x20, 0x64, 0xae, 0xcb, 0xf1, 0xef, 0x27, 0x5e, 0x60};
//352c493594c20d61db380c43901532f865c73726d121f49801884bacc31dd21b
std::vector<erc20_struct> erc20s;
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
    quary_erc20s();
    
}
void quary_erc20s(){
    char group_name[64];
    char data_item_name[64];
    nvdm_status_t status;

    status = nvdm_query_begin();
    if (status != NVDM_STATUS_OK) {
        Serial.println("NVDM_STATUS_ERROR");
    }
    while(1){
        status = nvdm_query_next_group_name(group_name);
        if (status != NVDM_STATUS_OK) {
            Serial.println("nvdm_query_next_group_name ERROR");
            status = nvdm_query_end();
            return;
        }
        Serial.println(group_name);
        if(strcmp ("ERC20",group_name)==0){
            Serial.println("ERC20 Found");
            break;
        }
        
    }
    while(1){
        status = nvdm_query_next_data_item_name(data_item_name);
        if (status != NVDM_STATUS_OK) {
            Serial.println("nvdm_query_next_data_item_name ERROR");
            status = nvdm_query_end();
            break;
        }
        Serial.print("New ERC20 Get:");Serial.println(data_item_name);
        /*
        erc20_struct new_erc20;
        status = nvdm_write_data_item(group_name,data_item_name, NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&new_erc20, sizeof(erc20_struct));
        if (status != NVDM_STATUS_OK) {
            Serial.print("nvdm_write_data_item Error:");Serial.println(status);
        }
        else{
            //LFlash.read(group_name, data_item_name, (uint8_t *)&new_erc20, &size);
            Serial.print("New ERC20 Get:");
            erc20s.push_back(new_erc20);
            printERC20(new_erc20);  
        }
        */

        
    }

    status = nvdm_query_end();

}

void printERC20(erc20_struct erc20){
    Serial.print("Contract Address:");Serial.println(ArraytoString(erc20.Contract_Address,20));
    Serial.print("TotalSupply Method:");Serial.println(ArraytoString(erc20.TotalSupply,4));
    Serial.print("BalanceOf Method:");Serial.println(ArraytoString(erc20.BalanceOf,4));
    Serial.print("Transfer Method:");Serial.println(ArraytoString(erc20.Transfer,4));
    Serial.print("TransferFrom Method:");Serial.println(ArraytoString(erc20.TransferFrom,4));
    Serial.print("Approve Method:");Serial.println(ArraytoString(erc20.Approve,4));
    Serial.print("Name:");Serial.println((char*)erc20.Name);
}
uint8_t start_transaction(uint8_t* outputdata,transaction_struct trx){
  
    Transaction transaction;
    transaction.setPrivateKey((uint8_t*)PRIVATE_KEY);

    vector<uint8_t> rawTransaction = transaction.getRaw(trx.nonce,trx.gasPrice, trx.gasLimit, trx.to, trx.value, trx.data, CHAIN_ID);
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


