#ifndef _BLE_hpp_
#define _BLE_hpp_

#include <LBLE.h>
#include <LBLEPeriphral.h>


//Base: 48495443-4f4e-4241-4447-4532303138
//48495443-4f4e-4241-4447-45323031381X
extern LBLEService ExchangeService;
//deal with 20byte mtu
extern LBLECharacteristicBuffer To_Address_Gatt;
extern LBLECharacteristicBuffer Value_Gatt;
extern LBLECharacteristicInt GasPrice_Gatt;
extern LBLECharacteristicInt GasLimit_Gatt;
extern LBLECharacteristicString ETHData_Gatt;
extern LBLECharacteristicInt Noice_Gatt;
extern LBLECharacteristicString Txn_out_Gatt;
//48495443-4f4e-4241-4447-45323031382x
extern LBLEService RequestService;
extern LBLECharacteristicBuffer WalletAddress_Gatt;
extern LBLECharacteristicInt Balance_Gatt;
extern LBLECharacteristicInt NotifyRequest_Gatt;
//48495443-4f4e-4241-4447-45323031383x
extern LBLEService SettingService;
extern LBLECharacteristicInt PinCode_Gatt;
extern LBLECharacteristicString WiFiSSID_Gatt;
extern LBLECharacteristicString WiFiPass_Gatt;

void init_BLE();

void Process_BLE();

#endif // _BLE_hpp_