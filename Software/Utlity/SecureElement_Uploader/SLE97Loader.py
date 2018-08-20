import serial
from serial.tools import list_ports
import time
from  binascii import hexlify
import sys
ports = list(list_ports.comports())
ports.sort()
print("There are %d COM port(s) found" % len(ports))
print("==============================")
for x in range(0,len(ports)):
    print("[%d] %s %s" % (x,ports[x][1],ports[x][2]))
    pass
print("==============================")
Got_Data="Get Data\r\n"
'''
x = raw_input(">>> Choose COM port(Enter number): ")
print ("waiting data")


'''
#x = raw_input(">>> Choose COM port(Enter number): ")
Serial = serial.Serial(sys.argv[2],115200)
#Serial = serial.Serial("COM5",115200)
time.sleep(1)
def send_cmd(cmd,data):
	if data == None:
		send_data = cmd.to_bytes(1, byteorder='big')+(0).to_bytes(2, byteorder='big')
		#print("Senddata:%s"%list(send_data))
		Serial.write(send_data)
		pass
	else:
		send_data = cmd.to_bytes(1, byteorder='big')+len(data).to_bytes(2, byteorder='big')
		#print("Senddata:%s"%list(send_data))
		#print("Payload:%s"%list(data))
		Serial.write(send_data)
		Serial.write(data)
		datareturn = Serial.read(int(len(data)))
		if data != datareturn:
			print("Transmit Error!")
			print(datareturn.hex())
			send_cmd(cmd,data)
			pass
		'''
		div = 64
		for x in range(int(len(data)/div)):
			Serial.write(data[x*div:x*div+div])
			print("Senddata:%s"%list(data[x*div:x*div+div]))
			print(Serial.read(div))
			#time.sleep(0.01)
			pass
		'''
	status = int.from_bytes(Serial.read(), byteorder='little') 
	if status != 0xFF:
		print("status error:",status)
		pass
	length = int.from_bytes(Serial.read(), byteorder='little') *256
	length = length+int.from_bytes(Serial.read(), byteorder='little') 
	if length>100:
		print("Length too large")
		status = 0xFE
		length = 0
		pass
	#print("Return bytes:%d"%length)
	data = Serial.read(int(length))
	#print("Return data:%s"%data.hex())
	return status,data

def IB_Device_Cos_LoadFinish():
	status,returndata = send_cmd(0x42,None)
	if(status!=0xFF):
		return -1
	else:
		return 1

def IB_Device_LoadsessAuth(authMtrl,encSMK):
	data = authMtrl+encSMK
	status,returndata = send_cmd(0x20,data)
	if( status!= 0xFF):
		return -1,None,None,None
	sessionTken = returndata[:4]
	encKey_N_IV = returndata[4:4+48]
	macKey = returndata[4+48:]
	return 1,sessionTken,encKey_N_IV,macKey

def IB_Device_Kv_Mac_Calc(data):
	status,returndata = send_cmd(0x30,data)
	if(status!=0xFF):
		return -1
	else:
		return 1

def IB_Device_Kv_Mac_Verify(myMac):
	status,returndata = send_cmd(0x31,myMac)
	if(status!=0xFF):
		return -1
	else:
		return 1

def IB_Device_Cos_LoadBegin(cosId):
	status,returndata = send_cmd(0x41,cosId.to_bytes(1, byteorder='big'))
	cosTkn = returndata
	if(status!=0xFF):
		return -1,cosTkn
	else:
		return status,cosTkn

def IB_Device_Cos_StRec(cosTkn,recId):
	data = cosTkn+recId.to_bytes(1, byteorder='big')
	status,returndata = send_cmd(0x43,data)
	if(status!=0xFF):
		return -1
	else:
		return 1

def IB_Device_Cos_WrBlk(cosTkn,blkId):
	data = cosTkn+blkId.to_bytes(4, byteorder='little')
	status,returndata = send_cmd(0x44,data)
	if(status!=0xFF):
		return -1
	else:
		return 1

def error_handle():
	exit()
	pass

f = open(sys.argv[1],"r")

#f.readline()

authMtrl = bytearray.fromhex(f.readline().strip("\n"))
encSMK = bytearray.fromhex(f.readline().strip("\n"))
print("AuthMtrl:",authMtrl.hex(),"\nEncSMK",encSMK.hex())

status = IB_Device_Cos_LoadFinish()
if status == -1:
	print("IB_Device_Cos_LoadFinish Failed!")
	pass

status,sessionTken, encKey_N_IV, macKey = IB_Device_LoadsessAuth(authMtrl, encSMK)
if status == -1:
	print("IB_Device_LoadsessAuth Failed!")
	while 1:
		print(Serial.readline())
		pass
	pass
print("sessionTken:%s\nencKey_N_IV:%s\nmacKey:%s"%(sessionTken.hex(), encKey_N_IV.hex(), macKey.hex()))


while(1):
	try:
		blkId = int(f.readline().strip("#"))
		print("===========Block ID:%d============"%blkId)
	except Exception as e:
		break
	data = bytearray.fromhex(f.readline().strip("\n"))
	print("data Length:%d"%len(data))

	mac = bytearray.fromhex(f.readline().strip("\n"))
	print("Mac:%s"%mac.hex())

	status =IB_Device_Kv_Mac_Calc(data)
	if status == -1:
		print("IB_Device_Kv_Mac_Calc Failed!")
		error_handle()
		pass
	print("IB_Device_Kv_Mac_Calc Success!")

	status = IB_Device_Kv_Mac_Verify(mac)
	if status == -1:
		print("IB_Device_Kv_Mac_Verify Failed!")
		error_handle()
		pass
	print("IB_Device_Kv_Mac_Verify Success!")

	status,cos_tkn = IB_Device_Cos_LoadBegin(0)
	if status == -1:
		print("IB_Device_Cos_LoadBegin Failed!")
		error_handle()
		pass
	print("cos_tkn:%s"%cos_tkn.hex())

	print("IB_Device_Cos_LoadBegin Success!")
	#Set Record
	for recId in range(8):
		print("IB_Device_Cos_StRec = %d"%recId)
		for retry in range(5):
			status = IB_Device_Cos_StRec(cos_tkn, recId)
			if status == 1:
				break
			else:
				print("IB_Device_Cos_StRec Failed!")
				continue
			pass
		pass

	status = IB_Device_Cos_WrBlk(cos_tkn, blkId)
	if status == -1:
		print("IB_Device_Cos_WrBlk Failed!")
		error_handle()
		pass
	print("IB_Device_Cos_WrBlk Success!")

	status = IB_Device_Cos_LoadFinish()
	if status == -1:
		print("IB_Device_Cos_LoadFinish Failed!")
		error_handle()
		pass
	print("IB_Device_Cos_LoadFinish Success!")