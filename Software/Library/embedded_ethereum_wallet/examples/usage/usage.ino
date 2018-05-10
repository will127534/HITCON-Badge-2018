//
// AnyLedger - Created by Bogdan Djukic on 01-04-2018.
//

#include "Transaction.h"
#include "CryptoHelper.h"

#include <vector>

static const uint8_t PRIVATE_KEY[] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                                      0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                                      0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                                      0x11, 0x11};

#define CONTRACT_ADDRESS "0x351c7fca3d8ff5d1a385353d71af8e90083538a2"
#define CHAIN_ID 3 // 0x1-mainnet, 0x3-Ropsten, 0x4-Rinkeby
#define GAS_PRICE 50000000000
#define GAS_LIMIT 100000
#define SMART_CONTRACT_METHOD "0x48f14470"
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("Start");
  Serial.print("Private Key:");
  for(int i=0;i<sizeof(PRIVATE_KEY);i++){
    Serial.print(PRIVATE_KEY[i],HEX);
  }
  Serial.println();
  // First, we need to create public key from the private one
  vector<uint8_t> publicKey = CryptoHelper::generateAddress(PRIVATE_KEY);

  // Then, we will create public address using public key
  vector<char> checksumAddress = CryptoHelper::generateChecksumAddress(publicKey);
  
  Serial.print("Public Key:");
  for(int i=0;i<publicKey.size();i++){
    Serial.print(publicKey[i],HEX);
  }
  Serial.println();
  
  // Let's convert the address into HEX format
  //String checksumAddressHex(checksumAddress.begin(), checksumAddress.end());
 // Serial.println(checksumAddress.size());
  Serial.print("Public address:");
  for(int i=0;i<checksumAddress.size();i++){
    Serial.print(checksumAddress[i],HEX);
  }
  Serial.println();

  // Now we can start building up the transaction itself
  Transaction transaction;
  transaction.setPrivateKey((uint8_t*)PRIVATE_KEY);

  // Make sure that nonce number is not smaller than the total number of transactions for the address
  // Total transaction is available through JSON-RPC method eth_getTransactionCount
  uint32_t nonce = 42;
  uint32_t chainId = CHAIN_ID;
  uint32_t gasPrice = GAS_PRICE;
  uint32_t gasLimit = GAS_LIMIT;

  String to = CONTRACT_ADDRESS;
  String value = "";
  String data = "0xc604091c0000000000000000000000000000000000000000000000000000000000000011";

  // The last step is passing all the necessary values for transaction signing process
  String rawTransaction = transaction.getRaw(nonce, gasPrice, gasLimit, &to, &value, &data, chainId);

  // Raw transaction is ready to be sent to JSON-RPC method call eth_sendRawTransaction
  Serial.print("Signed raw transaction:");
  Serial.print(rawTransaction);
  Serial.println();
  

}
void loop() {

}


