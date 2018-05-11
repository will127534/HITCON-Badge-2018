#include "BLE.hpp"
#include "epddisplay.hpp"
#include "util.hpp"
#include "wallet.hpp"

LBLEService ExchangeService("48495443-4f4e-4241-4447-453230313810");
//deal with 20byte mtu
LBLECharacteristicBuffer To_Address_Gatt("48495443-4f4e-4241-4447-453230313811", LBLE_WRITE); //20byte-RawAddress
LBLECharacteristicBuffer Value_Gatt("48495443-4f4e-4241-4447-453230313813", LBLE_WRITE );    //1Wei
LBLECharacteristicInt GasPrice_Gatt("48495443-4f4e-4241-4447-453230313814", LBLE_WRITE );    //1Wei
LBLECharacteristicInt GasLimit_Gatt("48495443-4f4e-4241-4447-453230313815", LBLE_WRITE );    
LBLECharacteristicInt Noice_Gatt("48495443-4f4e-4241-4447-453230313817", LBLE_WRITE );
LBLECharacteristicString Txn_out_Gatt("48495443-4f4e-4241-4447-453230313818", LBLE_READ );


LBLEService TokenService("48495443-4f4e-4241-4447-453230313840");
//Token related
LBLECharacteristicBuffer Token_to_address_Gatt("48495443-4f4e-4241-4447-453230313841", LBLE_WRITE );
LBLECharacteristicBuffer Token_method_Gatt("48495443-4f4e-4241-4447-453230313842", LBLE_WRITE );
LBLECharacteristicBuffer Token_Value_Gatt("48495443-4f4e-4241-4447-453230313843", LBLE_WRITE );

//48495443-4f4e-4241-4447-45323031382x
LBLEService RequestService("48495443-4f4e-4241-4447-453230313820");
LBLECharacteristicBuffer WalletAddress_Gatt("48495443-4f4e-4241-4447-453230313821", LBLE_READ);
LBLECharacteristicInt Balance_Gatt("48495443-4f4e-4241-4447-453230313823", LBLE_WRITE|LBLE_READ); //1Wei
LBLECharacteristicInt NotifyRequest_Gatt("48495443-4f4e-4241-4447-453230313824", LBLE_READ);
//48495443-4f4e-4241-4447-45323031383x
LBLEService SettingService("48495443-4f4e-4241-4447-453230313830");
LBLECharacteristicInt PinCode_Gatt("48495443-4f4e-4241-4447-453230313831", LBLE_READ);
LBLECharacteristicString WiFiSSID_Gatt("48495443-4f4e-4241-4447-453230313832", LBLE_WRITE);
LBLECharacteristicString WiFiPass_Gatt("48495443-4f4e-4241-4447-453230313833", LBLE_WRITE);

void init_BLE(){
  // Initialize BLE subsystem
  LBLE.begin();
  while (!LBLE.ready()) {
    delay(100);
  }
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

  uint8_t buffer[20] = {0};
  To_Address_Gatt.setValueBuffer(buffer,20);
  Token_to_address_Gatt.setValueBuffer(buffer,20);

  uint8_t buffer2[32] = {0};
  Value_Gatt.setValueBuffer(buffer2,32);
  Token_method_Gatt.setValueBuffer(buffer2,32);
  Token_Value_Gatt.setValueBuffer(buffer2,32);

  // Add characteristics into ledService
  ExchangeService.addAttribute(To_Address_Gatt);
  ExchangeService.addAttribute(Value_Gatt);
  ExchangeService.addAttribute(GasPrice_Gatt);
  ExchangeService.addAttribute(GasLimit_Gatt);
  ExchangeService.addAttribute(Txn_out_Gatt);
  ExchangeService.addAttribute(Noice_Gatt);

  TokenService.addAttribute(Token_to_address_Gatt);
  TokenService.addAttribute(Token_method_Gatt);
  TokenService.addAttribute(Token_Value_Gatt);

  SettingService.addAttribute(PinCode_Gatt);
  SettingService.addAttribute(WiFiSSID_Gatt);
  SettingService.addAttribute(WiFiPass_Gatt);

  RequestService.addAttribute(Balance_Gatt);
  RequestService.addAttribute(WalletAddress_Gatt);
  RequestService.addAttribute(NotifyRequest_Gatt);

  // Add service to GATT server (peripheral)
  LBLEPeripheral.addService(ExchangeService);
  LBLEPeripheral.addService(TokenService);
  LBLEPeripheral.addService(SettingService);
  LBLEPeripheral.addService(RequestService);
  // start the GATT server - it is now 
  // available to connect
  LBLEPeripheral.begin();

  // start advertisment
  LBLEPeripheral.advertise(advertisement);

}

void Process_BLE(){

  if (To_Address_Gatt.isWritten() && Value_Gatt.isWritten() && Noice_Gatt.isWritten())
  {
    
    Serial.println("New Transaction Get!");
    uint8_t To_Address_Buffer[20] = {0};
    To_Address_Gatt.getValue(To_Address_Buffer,20,0);

    uint8_t * Value_Buffer;
    uint8_t size = Value_Gatt.getLastWrittenInfo().size;
    Value_Buffer = (uint8_t*)malloc(size);
    Value_Gatt.getValue(Value_Buffer,size,0);

    String To = ArraytoString(To_Address_Buffer,20);
    String Value_str = ArraytoString(Value_Buffer,size);    
    //String To = String(To_Address_Buffer,HEX);
    //String Value_str = String(Value_Buffer,HEX);

    uint32_t Noice_int = Noice_Gatt.getValue();
    uint64_t GasPrice_int = (uint64_t)GAS_PRICE;
    uint32_t GasLimit_int = GAS_LIMIT;
    String Data = "";
 
    Serial.print("TO:");Serial.println(To);
    Serial.print("Value:");Serial.println(Value_str);
    Serial.print("Noice:");Serial.println(Noice_int);

    if(GasPrice_Gatt.isWritten()){
      GasPrice_int =(uint64_t)GasPrice_Gatt.getValue()*(uint64_t)1000000000;
      Serial.print("GasPrice(gwei):");Serial.println((uint32_t)(GasPrice_int/(uint64_t)1000000000));
    }
    if(GasLimit_Gatt.isWritten()){
      GasLimit_int = GasLimit_Gatt.getValue();
      Serial.print("GasLimit:");Serial.println(GasLimit_int);      
    }
    if(Token_method_Gatt.isWritten() && Token_to_address_Gatt.isWritten() && Token_Value_Gatt.isWritten()){
      Serial.println("Token Transfer Get!");
      uint8_t Token_To_Address_Buffer[20] = {0};
      Token_to_address_Gatt.getValue(Token_To_Address_Buffer,20,0);

      uint8_t * Token_Value_Buffer;
      uint8_t size_Token_Value_Buffer = Token_Value_Gatt.getLastWrittenInfo().size;
      Token_Value_Buffer = (uint8_t*)malloc(size_Token_Value_Buffer);
      Token_Value_Gatt.getValue(Token_Value_Buffer,size_Token_Value_Buffer,0);

      uint8_t * Token_method_Buffer;
      uint8_t size_Token_method_Buffer = Token_method_Gatt.getLastWrittenInfo().size;
      Token_method_Buffer = (uint8_t*)malloc(size_Token_method_Buffer);
      Token_method_Gatt.getValue(Token_method_Buffer,size_Token_method_Buffer,0);

      Serial.print("Token TO:");Serial.println(ArraytoString(Token_To_Address_Buffer,20));
      Serial.print("Token Method:");Serial.println(ArraytoString(Token_method_Buffer,size_Token_method_Buffer,4));
      Serial.print("Token Value:");Serial.println(ArraytoString(Token_Value_Buffer,size_Token_Value_Buffer));

      Data = ArraytoString(Token_method_Buffer,size_Token_method_Buffer,4)+ArraytoString(Token_To_Address_Buffer,20,32)+ArraytoString(Token_Value_Buffer,size_Token_Value_Buffer,32);

      Serial.print("Data:");Serial.println(Data);
    }

    String Txn = start_transaction(publicAddress_string,To,Value_str,Data,GasPrice_int,GasLimit_int,Noice_int);
    Serial.print("Txn_out:");Serial.println(Txn);
    Txn_out_Gatt.setValue(Txn);
    LBLEPeripheral.notifyAll(Txn_out_Gatt);
  }
}
