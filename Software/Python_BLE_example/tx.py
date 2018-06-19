from bluepy.btle import Scanner, DefaultDelegate
from bluepy.btle import Peripheral
import bluepy.btle as btle
import time
import binascii
import sys
import struct
from Crypto.Cipher import AES
from Crypto import Random
rndfile = Random.new()

input_str = "hitcon://pair?v=18&a=808c2257d778e5f1340d9325116f5a7273b33f5d&k=ce1f843391af38a1a93cae8c6439754c&s=1f7fd22b-bbeb-dd95-98ac-b9b75e971974&c=256f3a074babbb0940dc1c2751eccf05e12df5c12737e1d8"
_IV = rndfile.read(16)



def aes_encrypt(data, key):
    cryptor = AES.new(key, AES.MODE_CBC, _IV)
    return cryptor.encrypt(data)

def aes_decrypt(data, key):
    cryptor = AES.new(key, AES.MODE_CBC, _IV)
    return cryptor.decrypt(data)


class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)

    def handleDiscovery(self, dev, isNewDev, isNewData):
        if isNewDev:
            pass
            #print ("Discovered device", dev.addr )
        elif isNewData:
            pass
            #print ("Received new data from", dev.addr)
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
        if desc == "Complete 128b Services" and value == "7419975eb7b9ac9895ddebbb2bd27f1f":
                print("Get Wallet address:%s!" % dev.addr)
                wallet_address = dev.addr

if wallet_address == 0:
        print ("No wallet found")
        exit()

p = Peripheral(wallet_address,btle.ADDR_TYPE_PUBLIC)
print(p.setMTU(144+3)) #not actually changing MTU



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
  


Transaction_GATT = p.getCharacteristics(uuid=MainCharacteristics['Transaction_UUID'])[0]


print("Writing Tx msg...")


addr = bytearray.fromhex(sys.argv[1]) 
Value_eth = float(sys.argv[2])
GasPrice_int = int(sys.argv[3])
GasLimit_int = int(sys.argv[4])
Noice_int = int(sys.argv[5])
data_byte = bytearray.fromhex(sys.argv[6]) 


Value_int = int(Value_eth*1000000000*1000000000)

Value_byte = Value_int.to_bytes((Value_int.bit_length() + 7) // 8, byteorder='big')

if(Value_int == 0):
  Value_byte = bytes(0x01)

GasPrice_byte = GasPrice_int.to_bytes((GasPrice_int.bit_length() + 7) // 8, byteorder='big')
GasLimit_byte = GasLimit_int.to_bytes((GasLimit_int.bit_length() + 7) // 8, byteorder='big')
Noice_byte = Noice_int.to_bytes((Noice_int.bit_length() + 7) // 8, byteorder='big')


AES_Key  =  bytes(bytearray.fromhex(AES_Key) )
print(AES_Key)
print(len(AES_Key))
print("IV:",_IV.hex())




print("To:",addr.hex())
print("Value:",Value_int/1000000000000000000," Eth",Value_int,"wei ","0x"+Value_byte.hex())
print("Writing GasPrice",GasPrice_int,"gwei ","0x"+GasPrice_byte.hex())
print("Writing GasLimit",GasLimit_int," ","0x"+GasLimit_byte.hex())
print("Writing Noice",Noice_int," ","0x"+Noice_byte.hex())

Transaction_array = bytes([0x01]) + bytes([len(addr)])+ addr + \
          bytes([0x02]) + bytes([len(Value_byte)])+ Value_byte + \
          bytes([0x03]) + bytes([len(GasPrice_byte)])+ GasPrice_byte + \
          bytes([0x04]) + bytes([len(GasLimit_byte)])+ GasLimit_byte + \
          bytes([0x05]) + bytes([len(Noice_byte)])+ Noice_byte + \
          bytes([0x06]) + bytes([len(data_byte)])+ data_byte 

print("Trasnaction Package:",Transaction_array.hex())
print(len(Transaction_array))
Transaction_array = Transaction_array + bytes(128-len(Transaction_array ))
print(len(Transaction_array))
print("Trasnaction Package:",Transaction_array.hex())


Transaction_array = aes_encrypt(Transaction_array,AES_Key )
Transaction_array = bytes(_IV)+Transaction_array


print("Transaction Package enc:",Transaction_array.hex())

Transaction_GATT.write(Transaction_array)
print("Wait for Txn")
'''
try:
  Token_to_address.write(sys.argv[6])
  Token_method.write( bytearray.fromhex("a9059cbb") )
  Value_int = int(sys.argv[7])
  Token_Value.write( Value_int.to_bytes((Value_int.bit_length() + 7) // 8, byteorder='big')  )
  print("Token Address:", bytearray.fromhex(sys.argv[6]))
  print("Token Value:", int(sys.argv[7]))
except:
  pass

'''
while 1:
  if p.waitForNotifications(1.0):
        print("reading Txn")
        Txn = p.getCharacteristics(uuid=MainCharacteristics['Txn_UUID'])[0]
        print (Txn.read().hex())
        exit()
  pass





