from bluepy.btle import Scanner, DefaultDelegate
from bluepy.btle import Peripheral
import bluepy.btle as btle
import time
import binascii
import sys

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

scanner = Scanner().withDelegate(ScanDelegate())
devices = scanner.scan(5.0)
wallet_address = 0
for dev in devices:
    #print ("Device %s (%s), RSSI=%d dB" % (dev.addr, dev.addrType, dev.rssi) )
    for (adtype, desc, value) in dev.getScanData():
        #print ("  %s = %s" % (desc, value))
        if "Wallet" in value :
                print("Get Wallet address:%s!" % dev.addr)
                wallet_address = dev.addr

if wallet_address == 0:
        print ("No wallet found")
        exit()

p = Peripheral(wallet_address,btle.ADDR_TYPE_PUBLIC)
#print(p.setMTU(1000)) #not actually changing MTU
Wallet_Address = p.getCharacteristics(uuid='484954434f4e42414447453230313821')[0]
print ("Wallet Address:",binascii.hexlify( Wallet_Address.read()))


ToAddress = p.getCharacteristics(uuid='484954434f4e42414447453230313811')[0]
Value = p.getCharacteristics(uuid='484954434f4e42414447453230313813')[0]
Noice = p.getCharacteristics(uuid='484954434f4e42414447453230313817')[0]
GasPrice = p.getCharacteristics(uuid='484954434f4e42414447453230313814')[0]
GasLimit = p.getCharacteristics(uuid='484954434f4e42414447453230313815')[0]

Token_to_address = p.getCharacteristics(uuid='484954434f4e42414447453230313841')[0]
Token_method = p.getCharacteristics(uuid='484954434f4e42414447453230313842')[0]
Token_Value = p.getCharacteristics(uuid='484954434f4e42414447453230313843')[0]



print("Writing Tx msg...")
addr = sys.argv[1]
GasPrice_int = int(sys.argv[3])
GasLimit_int = int(sys.argv[4])


try:
  Token_to_address.write(sys.argv[6])
  Token_method.write( bytearray.fromhex("a9059cbb") )
  Value_int = int(sys.argv[7])
  Token_Value.write( Value_int.to_bytes((Value_int.bit_length() + 7) // 8, byteorder='big')  )
  print("Token Address:", bytearray.fromhex(sys.argv[6]))
  print("Token Value:", int(sys.argv[7]))
except:
  pass



print("To:",addr)
ToAddress.write( bytearray.fromhex(addr))

Value_eth = float(sys.argv[2])
Value_int = int(Value_eth*1000000000*1000000000)
print(Value_int/1000000000000000000," Eth")
Value_byte = Value_int.to_bytes((Value_int.bit_length() + 7) // 8, byteorder='big')
#print(Value_byte)

if(Value_int == 0):
  Value_byte = bytes(0x01)

#print(len(Value_byte))

Value.write(Value_byte)



print("Writing GasPrice",GasPrice_int,"gwei")
GasPrice.write(bytes([GasPrice_int]))

print("Writing GasLimit",GasLimit_int)
#print(GasLimit_int.to_bytes((GasLimit_int.bit_length() + 7) // 8, byteorder='little'))
GasLimit.write(GasLimit_int.to_bytes((GasLimit_int.bit_length() + 7) // 8, byteorder='little'))

Noice_int = int(sys.argv[5])
print("Writing Noice",Noice_int)
Noice.write(bytes([Noice_int]))



time.sleep(0.2)
print("reading Txn")
Txn = p.getCharacteristics(uuid='484954434f4e42414447453230313818')[0]

print (Txn.read())




