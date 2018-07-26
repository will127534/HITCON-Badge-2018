#include "BLE.hpp"
#include "epddisplay.hpp"
#include "util.hpp"
#include "wallet.hpp"
#include "LFlash.h"

#include "hal_aes.h"
#include "control.hpp"
LBLEService* ExchangeService;

LBLECharacteristicBuffer* Transaction_GATT; 
LBLECharacteristicBuffer* Txn_GATT; 
LBLECharacteristicBuffer* AddERC20_GATT; 
LBLECharacteristicBuffer* Balance_GATT; 
LBLECharacteristicBuffer* General_CMD_GATT; 
LBLECharacteristicBuffer* General_Data_GATT; 

LBLEService* BatteryService;
LBLECharacteristicBuffer* Battery_Level_GATT; 

//0000xxxx-0000-1000-8000-00805F9B34FB 16bit base UUID
LBLECharacteristicBuffer FirmwareRevisionGATT("00002A26-0000-1000-8000-00805F9B34FB", LBLE_READ); 
LBLEService DeviceInformationService("0000180A-0000-1000-8000-00805F9B34FB");

bool NewBalanceFlag = 0;

#define BLE_MTU 144

#define DEBUG_BLE
#ifdef DEBUG_BLE
#define DEBUG_PRINT Serial.print
#define DEBUG_PRINTLN Serial.println
#else
#define DEBUG_PRINT 
#define DEBUG_PRINTLN 
#endif


void init_BLE(){
  // Initialize BLE subsystem
  //
  while (!LBLE.ready()) {
    delay(20);
  }


  bool changed = 0;
  uint32_t size = 1;
  LFlashStatus status = LFlash.read("BLE","NewSetting",(uint8_t *)&changed,&size);
  DEBUG_PRINT("[BLE init] BLE Changed:");DEBUG_PRINTLN(changed);
  if (changed==1 || status == LFLASH_ITEM_NOT_FOUND)
  {
    regenerate_BLE_parameter();
  }

  char ServiceUUID[37] = {0};
  char Transaction_UUID[37] = {0};
  char Txn_UUID[37] = {0};
  char AddERC20_UUID[37] = {0};
  char Balance_UUID[37] = {0};
  char General_CMD_UUID[37] = {0};
  char General_Data_UUID[37] = {0};
  size = 37;

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

  DEBUG_PRINTLN("[init_BLE] LFLASH_Saved_UUID:");
  DEBUG_PRINT("\t");DEBUG_PRINT("ServiceUUID:");DEBUG_PRINT("\t");DEBUG_PRINTLN(ServiceUUID);
  DEBUG_PRINT("\t");DEBUG_PRINT("Transaction_UUID:");DEBUG_PRINT("\t");DEBUG_PRINTLN(Transaction_UUID);
  DEBUG_PRINT("\t");DEBUG_PRINT("Txn_UUID:");DEBUG_PRINT("\t");DEBUG_PRINTLN(Txn_UUID);
  DEBUG_PRINT("\t");DEBUG_PRINT("AddERC20_UUID:");DEBUG_PRINT("\t");DEBUG_PRINTLN(AddERC20_UUID);
  DEBUG_PRINT("\t");DEBUG_PRINT("Balance_UUID:");DEBUG_PRINT("\t");DEBUG_PRINTLN(Balance_UUID);
  DEBUG_PRINT("\t");DEBUG_PRINT("General_CMD_UUID:");DEBUG_PRINT("\t");DEBUG_PRINTLN(General_CMD_UUID);
  DEBUG_PRINT("\t");DEBUG_PRINT("General_Data_UUID:");DEBUG_PRINT("\t");DEBUG_PRINTLN(General_Data_UUID);

  DEBUG_PRINT("[BLE init] LFLASH_Saved_AESKey: ");
  for (int i = 0; i < 16; ++i)
  {
    DEBUG_PRINT(AES_key[i],HEX);DEBUG_PRINT(" ");
  }
  DEBUG_PRINTLN("");

  Transaction_GATT = new LBLECharacteristicBuffer(Transaction_UUID, LBLE_WRITE); 
  Txn_GATT = new LBLECharacteristicBuffer(Txn_UUID, LBLE_READ); 
  AddERC20_GATT = new LBLECharacteristicBuffer(AddERC20_UUID, LBLE_WRITE); 
  Balance_GATT = new LBLECharacteristicBuffer(Balance_UUID, LBLE_WRITE); 
  General_CMD_GATT = new LBLECharacteristicBuffer(General_CMD_UUID, LBLE_WRITE|LBLE_READ); 
  General_Data_GATT = new LBLECharacteristicBuffer(General_Data_UUID, LBLE_WRITE|LBLE_READ); 

  ExchangeService = new LBLEService(ServiceUUID);
  //LBLEService ExchangeService("48495443-4f4e-4241-4447-453230313810");

  DEBUG_PRINT("[BLE init] Device Address = [");
  DEBUG_PRINT(LBLE.getDeviceAddress());
  DEBUG_PRINTLN("]");

  // configure our advertisement data.
  // In this case, we simply create an advertisement that represents an
  // connectable device with a device name
  LBLEAdvertisementData advertisement;


  // Configure our device's Generic Access Profile's device name
  // Ususally this is the same as the name in the advertisement data.
  //LBLEPeripheral.setName("");

  uint8_t buffer[BLE_MTU] = {0};
  Transaction_GATT->setValueBuffer(buffer,BLE_MTU);
  Txn_GATT->setValueBuffer(buffer,BLE_MTU);
  AddERC20_GATT->setValueBuffer(buffer,BLE_MTU);
  Balance_GATT->setValueBuffer(buffer,BLE_MTU);
  General_CMD_GATT->setValueBuffer(buffer,BLE_MTU);
  General_Data_GATT->setValueBuffer(buffer,BLE_MTU);


  ExchangeService->addAttribute(*Transaction_GATT);
  ExchangeService->addAttribute(*Txn_GATT);
  ExchangeService->addAttribute(*AddERC20_GATT);
  ExchangeService->addAttribute(*Balance_GATT);
  ExchangeService->addAttribute(*General_CMD_GATT);
  ExchangeService->addAttribute(*General_Data_GATT);
  

  LBLEPeripheral.addService(*ExchangeService);


  //0000xxxx-0000-1000-8000-00805F9B34FB 16bit base UUID
  Battery_Level_GATT = new LBLECharacteristicBuffer("00002A19-0000-1000-8000-00805F9B34FB", LBLE_READ); 
  BatteryService = new LBLEService("0000180F-0000-1000-8000-00805F9B34FB");

  char version[] = "1.0.0";
  FirmwareRevisionGATT.setValueBuffer((uint8_t*)version,sizeof(version));
  DeviceInformationService.addAttribute(FirmwareRevisionGATT);
  LBLEPeripheral.addService(DeviceInformationService);


  uint8_t battery_percentage = readBatteryPercentage();
  uint32_t size_battery = 1;
  Battery_Level_GATT->setValueBuffer(&battery_percentage,size_battery);
  BatteryService->addAttribute(*Battery_Level_GATT);

  LBLEPeripheral.addService(*BatteryService);


  advertisement.configAsConnectableDevice("",ServiceUUID);
  //advertisement.configAsConnectableDevice("","0000180F-0000-1000-8000-00805F9B34FB");
  // start the GATT server - it is now 
  // available to connect

    // start advertisment
  LBLEPeripheral.advertise(advertisement);
  LBLEPeripheral.begin();

  if (changed)
  {
    display_paringQR();
    delay(100);
  }

}

void decript(uint8_t* buffer,uint8_t* decrypted_buffer,uint32_t buf_len){

    uint8_t aes_cbc_iv[HAL_AES_CBC_IV_LENGTH] = {0};
    memcpy((uint8_t *)aes_cbc_iv,(uint8_t *)buffer,HAL_AES_CBC_IV_LENGTH);

    uint8_t encrypted_text_buffer[BLE_MTU-HAL_AES_CBC_IV_LENGTH] = {0};
    memcpy((uint8_t *)encrypted_text_buffer,(uint8_t *)buffer+HAL_AES_CBC_IV_LENGTH,BLE_MTU-HAL_AES_CBC_IV_LENGTH);

    DEBUG_PRINT("[BLE decript] encripted:");DEBUG_PRINTLN(sizeof(encrypted_text_buffer));
    for (int i = 0; i < BLE_MTU-HAL_AES_CBC_IV_LENGTH; ++i)
    {
      DEBUG_PRINT(encrypted_text_buffer[i],HEX);
    }
    DEBUG_PRINTLN("");

    uint8_t AES_key[16] = {0};
    uint32_t size = 16;
    LFlash.read("BLE","BLE_AES_key",(uint8_t *)&AES_key,&size);

    hal_aes_buffer_t key = {
        .buffer = AES_key,
        .length = sizeof(AES_key)
    };

    hal_aes_buffer_t encrypted_text = {
        .buffer = encrypted_text_buffer,
        .length = sizeof(encrypted_text_buffer)
    };

    //uint8_t decrypted_buffer[BLE_MTU-HAL_AES_CBC_IV_LENGTH] = {0};

    hal_aes_buffer_t decrypted_text = {
        .buffer = decrypted_buffer,
        .length = buf_len
    };

    hal_aes_cbc_decrypt(&decrypted_text, &encrypted_text, &key, aes_cbc_iv);

    DEBUG_PRINT("[BLE decript] dencripted:");DEBUG_PRINTLN(sizeof(decrypted_buffer));
    for (int i = 0; i < BLE_MTU-HAL_AES_CBC_IV_LENGTH; ++i)
    {
      DEBUG_PRINT(decrypted_buffer[i],HEX);
    }
    DEBUG_PRINTLN("");
}

int parsing(uint8_t* data,uint8_t header, uint8_t buffersize, uint8_t expected_length, uint8_t* buffer,uint8_t data_pointer){

    if (data[0] != header)
    {
      DEBUG_PRINT("[BLE parsing] Header ERROR:");
      DEBUG_PRINTLN(data[0]);
      return -1;
    }
    if (expected_length > 0)
    {
      if (data[1] != expected_length)
      {
        DEBUG_PRINT("[BLE parsing] Length ERROR");
        DEBUG_PRINTLN(data[1]);
        return -1;
      }
    }

    if (data[1] > buffersize || data[1] >= BLE_MTU-data_pointer)
    {
      DEBUG_PRINTLN("[BLE parsing] Length ERROR");
      return -1;
    }
    

    uint8_t data_size = data[1];
    memcpy(buffer,data+2,data_size);
    return data_size;
}

bool Process_BLE(){

  if (Transaction_GATT->isWritten())
  {
    DEBUG_PRINTLN("[BLE Process] New Transaction Get!");
    uint8_t Transaction_Buffer[BLE_MTU] = {0};
    Transaction_GATT->getValue(Transaction_Buffer,BLE_MTU,0);
    uint8_t buffer[BLE_MTU] = {0};
    Transaction_GATT->setValueBuffer(buffer,BLE_MTU);
    for (int i = 0; i < 144; ++i)
    {
      DEBUG_PRINT(Transaction_Buffer[i],HEX);
    }
    DEBUG_PRINTLN("");

    transaction_struct trx_test;

    uint8_t decrypted_buffer[BLE_MTU-HAL_AES_CBC_IV_LENGTH] = {0};
    decript(Transaction_Buffer,decrypted_buffer,BLE_MTU-HAL_AES_CBC_IV_LENGTH);

    uint8_t TO_Address[20] = {0};
    uint8_t header_counter = 0;
    //int parsing(uint8_t* data,uint8_t header, uint8_t buffersize, uint8_t expected_length, uint8_t* buffer){
    int len = parsing(decrypted_buffer+header_counter,0x01,20,20,TO_Address,header_counter);
    if (len == -1)
    {
      DEBUG_PRINTLN("[BLE Process] Address Parse Error");
      return 0;
    }
    trx_test.to.assign(TO_Address, TO_Address + 20);

    header_counter += len + 2;

    uint8_t Value[32] = {0};
    len = parsing(decrypted_buffer+header_counter,0x02,32,0,Value,header_counter);
    if (len == -1)
    {
      DEBUG_PRINTLN("[BLE Process] Value Parse Error");
      return 0;
    }
    trx_test.value.assign(Value, Value + len);


    header_counter += len + 2;

    uint8_t GasPrice[32] = {0};
    len = parsing(decrypted_buffer+header_counter,0x03,32,0,GasPrice,header_counter);
    if (len == -1)
    {
      DEBUG_PRINTLN("[BLE Process] GasPrice Parse Error");
      return 0;
    }
    trx_test.gasPrice.assign(GasPrice, GasPrice + len);

    header_counter += len + 2;
    
    uint8_t GasLimit[32] = {0};
    len = parsing(decrypted_buffer+header_counter,0x04,32,0,GasLimit,header_counter);
    if (len == -1)
    {
      DEBUG_PRINTLN("[BLE Process] GasLimit Parse Error");
      return 0;
    }
    trx_test.gasLimit.assign(GasLimit, GasLimit + len);


    header_counter += len + 2;

    uint8_t Nonce[32] = {0};
    len = parsing(decrypted_buffer+header_counter,0x05,32,0,Nonce,header_counter);
    if (len == -1)
    {
      DEBUG_PRINTLN("[BLE Process] Nonce Parse Error");
      return 0;
    }
    trx_test.nonce.assign(Nonce, Nonce + len);
    header_counter += len + 2;

    uint8_t Data[100] = {0};
    len = parsing(decrypted_buffer+header_counter,0x06,100,0,Data,header_counter);
    if (len == -1)
    {
      DEBUG_PRINTLN("[BLE Process] Data Parse Error");
      return 0;
    }
    trx_test.data.assign(Data, Data + len);

    String token_names;
    bool if_token = 0;
    for (int i = 0; i < erc20s.size(); ++i)
    {
      if (memcmp(erc20s[i].Contract_Address,&trx_test.to[0],20) == 0)
      {
        Serial.println("[BLE Process] Token Transfer Get");
        Serial.print("[BLE Process] Token:");Serial.println(erc20s[i].Name);
        //check if data contain correct format, compare the data hash
        uint8_t transfermethod[4] = {0xa9,0x05,0x9c,0xbb};
        if (trx_test.data.size() == 68 && memcmp(transfermethod,&trx_test.data[0],4)==0)
        {
          Serial.println("[BLE Process] Token Transfer data Confirm");
          if_token = 1;
          token_names = String(erc20s[i].Name);
        }
      }
    }

    bool ret = 0;
    if (if_token)
    {
      ret = Token_transaction_page(trx_test,token_names);
    }
    else{
      ret = transaction_page(&trx_test);
    }

    if (ret != 1)
    {
      uint8_t Txn_buffer[10] = {0};
      Txn_buffer[0] = 0xF1; //F1 = transaction cancel by user
      Txn_GATT->setValueBuffer(Txn_buffer,1);
      LBLEPeripheral.notifyAll(*Txn_GATT);
      return 1;
    }

    uint8_t Txn_buffer[256] = {0};
    uint8_t Txn_len = start_transaction(Txn_buffer,trx_test);
    DEBUG_PRINT("[BLE Process] Txn Length:");DEBUG_PRINTLN(Txn_len);
    Txn_GATT->setValueBuffer(Txn_buffer,Txn_len);
    LBLEPeripheral.notifyAll(*Txn_GATT);
    return 1;
    
  }

  if (Balance_GATT->isWritten())
  {
    DEBUG_PRINTLN("[BLE Process] New Balance Get!");
    uint8_t Balance_buffer[BLE_MTU] = {0};
    Balance_GATT->getValue(Balance_buffer,BLE_MTU,0);
    uint8_t buffer[BLE_MTU] = {0};
    Balance_GATT->setValueBuffer(buffer,BLE_MTU);
    for (int i = 0; i < BLE_MTU; ++i)
    {
      DEBUG_PRINT(Balance_buffer[i],HEX);
    }
    DEBUG_PRINTLN("");

    uint8_t decrypted_buffer[BLE_MTU-HAL_AES_CBC_IV_LENGTH] = {0};
    decript(Balance_buffer,decrypted_buffer,BLE_MTU-HAL_AES_CBC_IV_LENGTH);

    uint8_t balance_counter = 0;
    while(1){
        uint8_t header_counter = 0x00;
        uint32_t size = BLE_MTU-HAL_AES_CBC_IV_LENGTH;
        if(balance_counter>128){
          break;
        }
        //uint8_t size = BLE_MTU;
        if (decrypted_buffer[balance_counter+0] != 0x01)
        {
          DEBUG_PRINTLN("[BLE Process] Address header ERROR, Return");
          return 0;
        }
        if (decrypted_buffer[balance_counter+1] != 0x14)
        {
          DEBUG_PRINTLN("[BLE Process] Address Length ERROR, Return");
          return 0;
        }
        uint8_t Address[20] = {0};
        memcpy(Address,decrypted_buffer+balance_counter+2,20);
        if (decrypted_buffer[balance_counter+22] != 0x02)
        {
          DEBUG_PRINTLN("[BLE Process] Value header ERROR, Return");
          return 0;
        }
        if (decrypted_buffer[balance_counter+23] !=8)
        {
          DEBUG_PRINTLN("[BLE Process] Value Length ERROR, Return");
          return 0;
        }
        uint8_t Value_size = decrypted_buffer[balance_counter+23];
        uint8_t Value[8] = {0};
        memcpy(Value,decrypted_buffer+balance_counter+24,Value_size);
        balance_counter += 32;
        DEBUG_PRINTLN("[BLE Process] Update Balance:");
        DEBUG_PRINT("[BLE Process] Address: ");    
        String Address_string = ArraytoString(Address,20);
        DEBUG_PRINTLN(Address_string);

        DEBUG_PRINT("[BLE Process] Value: ");    
        for (int i = 0; i < Value_size; ++i)
        {
          DEBUG_PRINT(Value[i],HEX);DEBUG_PRINT("");
        }
        DEBUG_PRINTLN("");
        
        if (Address_string.equals(publicAddress_string)){
          DEBUG_PRINTLN("[BLE Process] Wallet Balance Get! ");    
          NewBalanceFlag = 1;
          LFlash.write("Wallet","Balance",LFLASH_RAW_DATA,(const uint8_t *)&Value,sizeof(Value));
        }

        uint8_t hitconToken_address[] = {0x50,0x3b,0x0C,0x13,0x96,0x65,0xE7,0xe9,0xF8,0x63,0xba,0x1B,0xCf,0x0e,0x63,0x5a,0x2E,0x87,0xaA,0x5b};
        String HitconContract_Address = ArraytoString(hitconToken_address,20);
        if (Address_string.equals(HitconContract_Address)){
          DEBUG_PRINTLN("[BLE Process] Hitcon Balance Get! ");    
          NewBalanceFlag = 1;
          LFlash.write("Wallet","HitconBalance",LFLASH_RAW_DATA,(const uint8_t *)&Value,sizeof(Value));


          double HITCON_Balance,HITCON_Diplay_Balance;
          uint32_t size = 8;
          LFlash.read("Wallet","HitconBalance",(uint8_t *)&HITCON_Balance,&size);
          LFlash.read("Wallet","HitconDisplayBalance",(uint8_t *)&HITCON_Diplay_Balance,&size);
          if (HITCON_Diplay_Balance<HITCON_Balance)
          {
            LFlash.write("Wallet","HitconDisplayBalance",LFLASH_RAW_DATA,(const uint8_t *)&Value,sizeof(Value));
          }
        }
    
    }
    return 0;
  }

}

void regenerate_BLE_parameter(){
  uint32_t AES_key[4] = {0};
  for (int i = 0; i < 4; ++i)
  {
    AES_key[i] = randomUint32_t_generator();
  }

  char ServiceUUID[37]= {0};
  random_UUID_generator(ServiceUUID);
  char ServiceUUIDHead[9]= {0};
  memcpy(ServiceUUIDHead,ServiceUUID,8);
  DEBUG_PRINT("[BLE Regenerate Paring] Service UUID:");DEBUG_PRINTLN(ServiceUUID);

  uint8_t Value[8] = {0};
  LFlash.write("Wallet","Balance",LFLASH_RAW_DATA,(const uint8_t *)&Value,sizeof(Value));
  LFlash.write("Wallet","HitconBalance",LFLASH_RAW_DATA,(const uint8_t *)&Value,sizeof(Value));
  char UUIDHead[6][9] = {0};

  for (uint8_t i = 0; i < 6; ++i)
  {
    random_UUID_generator_head(UUIDHead[i]);
    DEBUG_PRINT("[BLE Regenerate Paring] UUID Head:");DEBUG_PRINTLN(UUIDHead[i]);
    for (uint8_t j = 0; j < 6; ++j)
    {
      if (String(UUIDHead[i]) == String(UUIDHead[j]) && j!=i)
      {
         DEBUG_PRINTLN(j);
         DEBUG_PRINTLN(i);
         DEBUG_PRINTLN(UUIDHead[j]);
         i = i-1;
      }
      if (String(UUIDHead[i]) == String(ServiceUUIDHead[j]))
      {
        DEBUG_PRINTLN(i);
        DEBUG_PRINTLN("[BLE Regenerate Paring] ServiceUUID");
        i = i-1;
      }
    }
  }

  LFlash.write("BLE","Service_UUID",LFLASH_RAW_DATA,(const uint8_t *)&ServiceUUID,sizeof(ServiceUUID));
  LFlash.write("BLE","Transaction",LFLASH_RAW_DATA,(const uint8_t *)&UUIDHead[0],8);
  LFlash.write("BLE","Txn",LFLASH_RAW_DATA,(const uint8_t *)&UUIDHead[1],8);
  LFlash.write("BLE","AddERC20",LFLASH_RAW_DATA,(const uint8_t *)&UUIDHead[2],8);
  LFlash.write("BLE","Balance",LFLASH_RAW_DATA,(const uint8_t *)&UUIDHead[3],8);
  LFlash.write("BLE","General_CMD",LFLASH_RAW_DATA,(const uint8_t *)&UUIDHead[4],8);
  LFlash.write("BLE","General_Data",LFLASH_RAW_DATA,(const uint8_t *)&UUIDHead[5],8);

  LFlash.write("BLE","BLE_AES_key",LFLASH_RAW_DATA,(const uint8_t *)&AES_key,sizeof(AES_key));
}
