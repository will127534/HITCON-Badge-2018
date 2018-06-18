from bluepy.btle import Scanner, DefaultDelegate
from bluepy.btle import Peripheral
import bluepy.btle as btle
import time
import binascii
import sys
import struct

input_str = "hitcon://pair?v=18&a=808c2257d778e5f1340d9325116f5a7273b33f5d&k=09626aa096254e8a8ce871bfd7b8895c&s=1cbfbb33-ffc7-c966-77f9-311c6ba9e425&c=26ccce12e2c66a0b72c50cca509dbfc1275074f57e7c5668"

class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)

    def handleDiscovery(self, dev, isNewDev, isNewData):
        if isNewDev:
            pass
            #print ("Discovered device", dev.addr )
            #print(dev)
        elif isNewData:
            pass
            print ("Received new data from", dev.addr)
    def handleNotification(self, cHandle, data):
        # ... perhaps check cHandle
        # ... process 'data'
        pass


print("Scanning for Wallet")
scanner = Scanner().withDelegate(ScanDelegate())
devices = scanner.scan(5.0)
wallet_address = 0
for dev in devices:
    print ("Device %s (%s), RSSI=%d dB" % (dev.addr, dev.addrType, dev.rssi) )
    for (adtype, desc, value) in dev.getScanData():
        print ("  %s = %s" % (desc, value))
        if dev.rssi >  -65 :
                print("Get Wallet address:%s!" % dev.addr)
                wallet_address = dev.addr

if wallet_address == 0:
        print ("No wallet found")
        exit()

p = Peripheral(wallet_address,btle.ADDR_TYPE_PUBLIC)
print(p.setMTU(1000)) #not actually changing MTU



Address = input_str.split("&")[1].split("=")[1]
AES_Key = input_str.split("&")[2].split("=")[1]
Service_UUID = input_str.split("&")[3].split("=")[1]
TotalChar_UUID = [input_str.split("&")[4].split("=")[1][i:i+8] for i in range(0, len(input_str.split("&")[4].split("=")[1]), 8)]


print ("Wallet Address:",Address)

Characteristics = ["Transaction_UUID","Txn_UUID","AddERC20_UUID","Balance_UUID","General_CMD_UUID","General_Data_UUID"]
MainCharacteristics_head = {"Transaction_UUID":TotalChar_UUID[0],"Txn_UUID":TotalChar_UUID[1],"AddERC20_UUID":TotalChar_UUID[2],"Balance_UUID":TotalChar_UUID[3],"General_CMD_UUID":TotalChar_UUID[4],"General_Data_UUID":TotalChar_UUID[5]}
MainCharacteristics = {"Transaction_UUID":"","Txn_UUID":"","AddERC20_UUID":"","Balance_UUID":"","General_CMD_UUID":"","General_Data_UUID":""}



service = p.getServiceByUUID(Service_UUID)


for Character in service.getCharacteristics():
  print(Character.uuid)
  for x in range(len(MainCharacteristics_head)):
    if Character.uuid.getCommonName().startswith(MainCharacteristics_head[Characteristics[x]]):
      print(Characteristics[x]," Get!")
      MainCharacteristics[Characteristics[x]] = Character.uuid
    pass
  


Balance_GATT = p.getCharacteristics(uuid=MainCharacteristics['Balance_UUID'])[0]


print("Writing Balance msg...")


addr = bytearray.fromhex(Address) 
Value = bytearray(struct.pack("d",float(sys.argv[1])))
print("To:",addr.hex())
print("Value:",Value.hex()," Eth")

Balance_array = bytes([0x01]) + bytes([len(addr)])+ addr + \
          bytes([0x02]) + bytes([len(Value)])+ Value

print("Update Balance Package:",Balance_array.hex())
Balance_GATT.write(Balance_array)


