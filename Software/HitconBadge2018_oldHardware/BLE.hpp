#ifndef _BLE_hpp_
#define _BLE_hpp_

#include <LBLE.h>
#include <LBLEPeriphral.h>



extern LBLEService* ExchangeService;
extern LBLECharacteristicBuffer* Transaction_GATT; 
extern LBLECharacteristicBuffer* Txn_GATT; 
extern LBLECharacteristicBuffer* AddERC20_GATT; 
extern LBLECharacteristicBuffer* Balance_GATT; 
extern LBLECharacteristicBuffer* General_CMD_GATT; 
extern LBLECharacteristicBuffer* General_Data_GATT; 
extern bool NewBalanceFlag;
void init_BLE();

void Process_BLE();
void regenerate_BLE_parameter();
#endif // _BLE_hpp_