#include "BLE.hpp"
#include "epddisplay.hpp"
#include "util.hpp"
#include "wallet.hpp"
#include "LFlash.h"



LBLEService* ExchangeService;
//deal with 20byte mtu
LBLECharacteristicBuffer* Transaction_GATT; 
LBLECharacteristicBuffer* Txn_GATT; 
LBLECharacteristicBuffer* AddERC20_GATT; 
LBLECharacteristicBuffer* Balance_GATT; 
LBLECharacteristicBuffer* General_CMD_GATT; 
LBLECharacteristicBuffer* General_Data_GATT; 
bool NewBalanceFlag = 0;


void init_BLE(){
  // Initialize BLE subsystem
  LBLE.begin();
  while (!LBLE.ready()) {
    delay(100);
  }


  char ServiceUUID[37] = {0};
  char Transaction_UUID[37] = {0};
  char Txn_UUID[37] = {0};
  char AddERC20_UUID[37] = {0};
  char Balance_UUID[37] = {0};
  char General_CMD_UUID[37] = {0};
  char General_Data_UUID[37] = {0};
  uint32_t size = 37;

  LFlash.read("BLE","Service_UUID",(uint8_t *)&ServiceUUID,&size);

  memcpy((uint8_t *)Transaction_UUID,(uint8_t *)ServiceUUID,37);
  memcpy((uint8_t *)Txn_UUID,(uint8_t *)ServiceUUID,37);
  memcpy((uint8_t *)AddERC20_UUID,(uint8_t *)ServiceUUID,37);
  memcpy((uint8_t *)Balance_UUID,(uint8_t *)ServiceUUID,37);
  memcpy((uint8_t *)General_CMD_UUID,(uint8_t *)ServiceUUID,37);
  memcpy((uint8_t *)General_Data_UUID,(uint8_t *)ServiceUUID,37);

   size = 8;
  LFlash.read("BLE","Transaction",(uint8_t *)&Transaction_UUID,&size);
  LFlash.read("BLE","Txn",(uint8_t *)&Txn_UUID,&size);
  LFlash.read("BLE","AddERC20",(uint8_t *)&AddERC20_UUID,&size);
  LFlash.read("BLE","Balance",(uint8_t *)&Balance_UUID,&size);
  LFlash.read("BLE","General_CMD",(uint8_t *)&General_CMD_UUID,&size);
  LFlash.read("BLE","General_Data",(uint8_t *)&General_Data_UUID,&size);

  uint8_t AES_key[16] = {0};
  size = 16;
  LFlash.read("BLE","BLE_AES_key",(uint8_t *)&AES_key,&size);

  Serial.println("LFLASH_Saved_UUID:");
  Serial.println(ServiceUUID);
  Serial.println(Transaction_UUID);
  Serial.println(Txn_UUID);
  Serial.println(AddERC20_UUID);
  Serial.println(Balance_UUID);
  Serial.println(General_CMD_UUID);
  Serial.println(General_Data_UUID);

  Serial.println("LFLASH_Saved_AESKey:");
  for (int i = 0; i < 16; ++i)
  {
    Serial.print(AES_key[i],HEX);Serial.print(" ");
  }
  Serial.println();


  Transaction_GATT = new LBLECharacteristicBuffer(Transaction_UUID, LBLE_WRITE); 
  Txn_GATT = new LBLECharacteristicBuffer(Txn_UUID, LBLE_WRITE); 
  AddERC20_GATT = new LBLECharacteristicBuffer(AddERC20_UUID, LBLE_WRITE); 
  Balance_GATT = new LBLECharacteristicBuffer(Balance_UUID, LBLE_WRITE); 
  General_CMD_GATT = new LBLECharacteristicBuffer(General_CMD_UUID, LBLE_WRITE); 
  General_Data_GATT = new LBLECharacteristicBuffer(General_Data_UUID, LBLE_WRITE); 

  ExchangeService = new LBLEService(ServiceUUID);
  //LBLEService ExchangeService("48495443-4f4e-4241-4447-453230313810");

  Serial.println("BLE ready");

  Serial.print("Device Address = [");
  Serial.print(LBLE.getDeviceAddress());
  Serial.println("]");

  // configure our advertisement data.
  // In this case, we simply create an advertisement that represents an
  // connectable device with a device name
  LBLEAdvertisementData advertisement;
  advertisement.configAsConnectableDevice("Wallet");

  // Configure our device's Generic Access Profile's device name
  // Ususally this is the same as the name in the advertisement data.
  LBLEPeripheral.setName("Wallet");

  uint8_t buffer[128] = {0};
  Transaction_GATT->setValueBuffer(buffer,128);
  Txn_GATT->setValueBuffer(buffer,128);
  AddERC20_GATT->setValueBuffer(buffer,128);
  Balance_GATT->setValueBuffer(buffer,128);
  General_CMD_GATT->setValueBuffer(buffer,128);
  General_Data_GATT->setValueBuffer(buffer,128);


  ExchangeService->addAttribute(*Transaction_GATT);
  ExchangeService->addAttribute(*Txn_GATT);
  ExchangeService->addAttribute(*AddERC20_GATT);
  ExchangeService->addAttribute(*Balance_GATT);
  ExchangeService->addAttribute(*General_CMD_GATT);
  ExchangeService->addAttribute(*General_Data_GATT);
  

  LBLEPeripheral.addService(*ExchangeService);

  // start the GATT server - it is now 
  // available to connect
  LBLEPeripheral.begin();

  // start advertisment
  LBLEPeripheral.advertise(advertisement);

}


int parsing(uint8_t* data,uint8_t header, uint8_t buffersize, uint8_t expected_length, uint8_t* buffer,uint8_t data_pointer){

    if (data[0] != header)
    {
      Serial.print("Header ERROR:");
      Serial.println(data[0]);
      return -1;
    }
    if (expected_length > 0)
    {
      if (data[1] != expected_length)
      {
        Serial.print("Length ERROR");
        Serial.println(data[1]);
        return -1;
      }
    }

    if (data[1] > buffersize || data[1] >= 128-data_pointer)
    {
      Serial.println("Length ERROR");
      return -1;
    }
    

    uint8_t data_size = data[1];
    memcpy(buffer,data+2,data_size);
    return data_size;
}

void Process_BLE(){

  if (Transaction_GATT->isWritten())
  {
    
    Serial.println("New Transaction Get!");
    uint8_t Transaction_Buffer[128] = {0};
    Transaction_GATT->getValue(Transaction_Buffer,128,0);

    for (int i = 0; i < 100; ++i)
    {
      Serial.print(Transaction_Buffer[i],HEX);
    }
    Serial.println();

    uint8_t TO_Address[20] = {0};
    uint8_t header_counter = 0;
    //int parsing(uint8_t* data,uint8_t header, uint8_t buffersize, uint8_t expected_length, uint8_t* buffer){
    int len = parsing(Transaction_Buffer+header_counter,0x01,20,20,TO_Address,header_counter);
    if (len == -1)
    {
      Serial.println("Address Parse Error");
      return;
    }
    String To = ArraytoString(TO_Address,20);
    Serial.print("To Address:");Serial.println(To);

    header_counter += len + 2;

    uint8_t Value[32] = {0};
    len = parsing(Transaction_Buffer+header_counter,0x02,32,0,Value,header_counter);
    if (len == -1)
    {
      Serial.println("Value Parse Error");
      return;
    }
    String Value_str = ArraytoString(Value,len);
    Serial.print("Value:");Serial.println(Value_str);

    header_counter += len + 2;

    uint8_t GasPrice[32] = {0};
    len = parsing(Transaction_Buffer+header_counter,0x03,32,0,GasPrice,header_counter);
    if (len == -1)
    {
      Serial.println("GasPrice Parse Error");
      return;
    }
    String GasPrice_str = ArraytoString(GasPrice,len);
    Serial.print("GasPrice:");Serial.println(GasPrice_str);
    header_counter += len + 2;
    
    uint8_t GasLimit[32] = {0};
    len = parsing(Transaction_Buffer+header_counter,0x04,32,0,GasLimit,header_counter);
    if (len == -1)
    {
      Serial.println("GasLimit Parse Error");
      return;
    }
    String GasLimit_str = ArraytoString(GasLimit,len);
    Serial.print("GasLimit:");Serial.println(GasLimit_str);
    header_counter += len + 2;

    uint8_t Noice[32] = {0};
    len = parsing(Transaction_Buffer+header_counter,0x05,32,0,Noice,header_counter);
    if (len == -1)
    {
      Serial.println("Noice Parse Error");
      return;
    }
    String Noice_str = ArraytoString(Noice,len);
    Serial.print("Noice:");Serial.println(Noice_str);
    header_counter += len + 2;

    uint8_t Data[100] = {0};
    len = parsing(Transaction_Buffer+header_counter,0x06,100,0,Data,header_counter);
    if (len == -1)
    {
      Serial.println("Data Parse Error");
      return;
    }
    String Data_str = ArraytoString(Data,len);
    Serial.print("Data:");Serial.println(Data_str);

    uint8_t Txn_buffer[256] = {0};
    uint8_t Txn_len = start_transaction(Txn_buffer,publicAddress_string,To,Value_str,Data_str,GasPrice_str,GasLimit_str,Noice_str);
    Serial.print("Txn Length:");Serial.println(Txn_len);
    Txn_GATT->setValueBuffer(Txn_buffer,Txn_len);
    LBLEPeripheral.notifyAll(*Txn_GATT);
    
  }
  if (AddERC20_GATT->isWritten())
  {
    Serial.println("New AddERC20 Get!");
    uint8_t AddERC20_buffer[128] = {0};
    AddERC20_GATT->getValue(AddERC20_buffer,128,0);
    Serial.println((char*)AddERC20_buffer);
    for (int i = 0; i < 100; ++i)
    {
      Serial.print(AddERC20_buffer[i],HEX);
    }
    Serial.println();

    uint8_t header_counter = 0x10;
    uint8_t size = 128;
    if (AddERC20_buffer[0] != 0x10)
    {
      Serial.println("ERC20 Contract Address header ERROR");
      return;
    }
    if (AddERC20_buffer[1] != 0x14)
    {
      Serial.println("ERC20 Contract Address Length ERROR");
      return;
    }

    uint8_t ERC20_Address[20] = {0};
    memcpy(ERC20_Address,AddERC20_buffer+2,20);

    uint8_t methods[6][4] = {0};
    for (int i = 0; i < 6; ++i)
    {
      Serial.println(i);
      if (AddERC20_buffer[22+i*6] != (0x11+i))
      {
        Serial.println("ERC20 Contract method header ERROR");
        return;
      }
      if (AddERC20_buffer[22+i*6+1] != 0x4)
      {
        Serial.println("ERC20 Contract method Length ERROR");
        return;
      }
      memcpy(methods[i],AddERC20_buffer+22+i*6+2,4);
    }

    Serial.print("Contract Address:");Serial.println(ArraytoString(ERC20_Address,20));
    Serial.print("TotalSupply Method:");Serial.println(ArraytoString(methods[0],4));
    Serial.print("BalanceOf Method:");Serial.println(ArraytoString(methods[1],4));
    Serial.print("Transfer Method:");Serial.println(ArraytoString(methods[2],4));
    Serial.print("TransferFrom Method:");Serial.println(ArraytoString(methods[3],4));
    Serial.print("Approve Method:");Serial.println(ArraytoString(methods[4],4));
    Serial.print("Name:");Serial.println((char*)methods[5]);
    
  }
  if (Balance_GATT->isWritten())
  {
    Serial.println("New Balance Get!");
    uint8_t Balance_buffer[128] = {0};
    Balance_GATT->getValue(Balance_buffer,128,0);
    for (int i = 0; i < 100; ++i)
    {
      Serial.print(Balance_buffer[i],HEX);
    }
    Serial.println();

    uint8_t header_counter = 0x00;
    uint8_t size = 128;
    if (Balance_buffer[0] != 0x01)
    {
      Serial.println("Address header ERROR");
      return;
    }
    if (Balance_buffer[1] != 0x14)
    {
      Serial.println("Address Length ERROR");
      return;
    }
    uint8_t Address[20] = {0};
    memcpy(Address,Balance_buffer+2,20);
    if (Balance_buffer[22] != 0x02)
    {
      Serial.println("Value header ERROR");
      return;
    }
    if (Balance_buffer[23] !=4)
    {
      Serial.println("Value Length ERROR");
      return;
    }
    uint8_t Value_size = Balance_buffer[23];
    uint8_t Value[4] = {0};
    memcpy(Value,Balance_buffer+24,Value_size);

    Serial.println("Update Balance:");
    Serial.print("Address: ");    
    for (int i = 0; i < 20; ++i)
    {
      Serial.print(Address[i],HEX);
    }
    Serial.println();

    Serial.print("Value: ");    
    for (int i = 0; i < Value_size; ++i)
    {
      Serial.print(Value[i]);
    }
    Serial.println();
    NewBalanceFlag = 1;
    LFlash.write("Wallet","Balance",LFLASH_RAW_DATA,(const uint8_t *)&Value,sizeof(Value));
  }
}
