#!/usr/bin/env python2

################################################################################
## Copyright (c) 2015 by Silicon Laboratories Inc.  All rights reserved.
## The program contained in this listing is proprietary to Silicon Laboratories,
## headquartered in Austin, Texas, U.S.A. and is subject to worldwide copyright
## protection, including protection under the United States Copyright Act of 1976
## as an unpublished work, pursuant to Section 104 and Section 408 of Title XVII
## of the United States code.  Unauthorized copying, adaptation, distribution,
## use, or display is prohibited by this law.
################################################################################

# Python 3.4

import sys
import ctypes
import struct

#-------------------------------------------------------------------------------
if sys.platform == 'win32':
    g_DLL = ctypes.windll.LoadLibrary("CP210xManufacturing.dll")
elif sys.platform.startswith('linux'):
    g_DLL = ctypes.cdll.LoadLibrary("./libcp210xmanufacturing.so.1.0")
elif sys.platform == 'darwin':
    g_DLL = ctypes.cdll.LoadLibrary("libCP210xManufacturing.dylib")

# each func returns int CP210x_STATUS
for cp210x_function in [
    "CP210x_GetNumDevices",
    "CP210x_Open", 
    "CP210x_Close",
    "CP210x_GetPartNumber",
    "CP210x_GetDeviceVid",
    "CP210x_GetDevicePid",
    "CP210x_GetDeviceMode",
    "CP210x_GetBaudRateConfig",
    "CP210x_SetBaudRateConfig",
    "CP210x_SetVid",
    "CP210x_SetPid",
    "CP210x_SetDeviceMode",
    "CP210x_Reset",
    "CP210x_GetProductString",
    "CP210x_GetDeviceProductString",
    "CP210x_GetDeviceSerialNumber",
    "CP210x_GetDeviceInterfaceString",
    "CP210x_SetManufacturerString",
    "CP210x_SetProductString",
    "CP210x_SetInterfaceString",
    "CP210x_SetSerialNumber",
    "CP210x_SetSelfPower",
    "CP210x_SetMaxPower",
    "CP210x_SetFlushBufferConfig",
    "CP210x_SetDeviceVersion",
    "CP210x_SetPortConfig",
    "CP210x_SetDualPortConfig",
    "CP210x_SetQuadPortConfig",
    "CP210x_SetLockValue",
    "CP210x_GetDeviceManufacturerString",
    "CP210x_GetSelfPower",
    "CP210x_GetMaxPower",
    "CP210x_GetFlushBufferConfig",
    "CP210x_GetDeviceVersion",
    "CP210x_GetBaudRateConfig",
    "CP210x_GetPortConfig",
    "CP210x_GetDualPortConfig",
    "CP210x_GetQuadPortConfig",
    "CP210x_GetLockValue",
    "CP210x_CreateHexFile",
    "CP210x_GetFirmwareVersion",
    "CP210x_GetConfig",
    ]:
    fnc = getattr(g_DLL, cp210x_function)
    fnc.restype = ctypes.c_int

"""
Uncomment this if you want to have fun with GPIOs.
This is here just because CP210xRuntime.py needs pyserial and it's a hassle.
    g_RtDLL = ctypes.WinDLL(os.path.join(os.path.dirname(__file__), "CP210xRuntime.dll"))
    # each func returns int CP210x_STATUS
    for cp210x_function in [
        "CP210xRT_ReadLatch",
        "CP210xRT_WriteLatch",
        ]:
        fnc = getattr(g_RtDLL, cp210x_function)
        fnc.restype = ctypes.c_int
"""

#-------------------------------------------------------------------------------
# Constant definitions copied from the public DLL header
CP210x_MAX_DEVICE_STRLEN = 256
# Return codes									
CP210x_SUCCESS           = 0x00
CP210x_FUNCTION_NOT_SUPPORTED = 0x04
CP210x_DEVICE_NOT_FOUND	 = 0xFF

CP210x_INVALID_PARAMETER = 0x02
# GetProductString() function flags
CP210x_RETURN_SERIAL_NUMBER = 0
CP210x_RETURN_DESCRIPTION   = 1
CP210x_RETURN_FULL_PATH     = 2

# GetDeviceVersion() return codes
CP210x_CP2101_VERSION = 0x01
CP210x_CP2102_VERSION = 0x02
CP210x_CP2103_VERSION = 0x03
CP210x_CP2104_VERSION = 0x04
CP210x_CP2105_VERSION = 0x05
CP210x_CP2108_VERSION = 0x08
CP210x_CP2109_VERSION = 0x09
CP210x_CP2102n_QFN28_VERSION = 0x20
CP210x_CP2102n_QFN24_VERSION = 0x21
CP210x_CP2102n_QFN20_VERSION = 0x22


# For CP210x_GetBaudRateConfig and CP210x_SetBaudRateConfig:
NUM_BAUD_CONFIGS            = 32
#typedef struct {
#    WORD BaudGen;
#    WORD Timer0Reload;
#    BYTE Prescaler;
#    DWORD BaudRate;
#} BAUD_CONFIG;
BAUD_CONFIG_FMT    = 'HHBL' # the above structure layout encoded for "struct" functions
BAUD_CONFIG_size   = struct.calcsize(BAUD_CONFIG_FMT)

class BAUD_CONFIG(ctypes.Structure):
    _fields_ = [("BaudGen",      ctypes.c_ushort),
                ("Timer0Reload", ctypes.c_ushort),
                ("Prescaler",    ctypes.c_ubyte),
                ("BaudRate",     ctypes.c_ulong)
                ]

class firmware_t(ctypes.Structure):
    _fields_ = [("Major", ctypes.c_ubyte),
                ("Minor", ctypes.c_ubyte),
                ("Build", ctypes.c_ubyte)
                ]        
firmware_t_FMT = 'BBB'
firmware_t_size = struct.calcsize(firmware_t_FMT)

#-------------------------------------------------------------------------------
# This class trivially wraps the DLL. Class methods have same names as DLL functions.
# Every wrapper returns CP210x_STATUS it gets from DLL
# The parameters are the same unless the DLL declaration is given in a comment above wrapper to show difference.
# Also, the device file handle is stored in the object, so wrappers don't have it as the first parameter.
#
# See Function prototypes in CP210xManufacturingDLL.h and AN721 for functional description.
#
# Strings returned from CP210xManufacturing.dll functions can be as large as CP210x_MAX_DEVICE_STRLEN chars
# plus 0 terminator. That much pre-allocated storage must be passed to them, there is no way to pass less.

class CP210xManufacturing:
    def __init__(self):
        self.hDev = ctypes.c_void_p(-1) # Windows HANDLE (INVALID_HANDLE_VALUE)

    #--------------------------------------------------
    # General info and open/close functions

    def CP210x_GetNumDevices(self):
        NumDevices = ctypes.c_ulong(0)
        err = g_DLL.CP210x_GetNumDevices(ctypes.byref(NumDevices));
        if err != CP210x_SUCCESS :
            print("CP210x_GetNumDevices err %x" % err)
        return err, NumDevices.value

    # Strings from SetupDi API
    def CP210x_GetProductString(self, DeviceNum, DeviceString, Options):
        err = g_DLL.CP210x_GetProductString(DeviceNum, DeviceString, Options)
        if err != CP210x_SUCCESS :
            print("CP210x_GetProductString err %x" % err)
        return err

    def CP210x_Open(self, DeviceNum):
        err = g_DLL.CP210x_Open(DeviceNum,  ctypes.byref(self.hDev))
        if err != CP210x_SUCCESS :
            print("CP210x_Open err 0x%x" % err)
        return err

    def CP210x_Close(self):
        err = g_DLL.CP210x_Close(self.hDev)
        if err != CP210x_SUCCESS :
            print("CP210x_Close err %x" % err)
        return err

    # Important for knowing which customization functions are valid for the device
    def CP210x_GetPartNumber(self):
        PartNum = ctypes.c_ubyte(0)
        err = g_DLL.CP210x_GetPartNumber(self.hDev, ctypes.byref( PartNum))
        if err != CP210x_SUCCESS :
            print("CP210x_GetPartNumber err %x" % err)
        return err, PartNum.value

    #--------------------------------------------------
    # Customization functions that work on any device

    # CP210x_GetDeviceManufacturerString(HANDLE cyHandle, LPVOID lpManufacturer, LPBYTE	lpbLength,	BOOL	bConvertToASCII = TRUE	);
    def CP210x_GetDeviceManufacturerString(self):
        cnt = ctypes.c_ubyte(0)
        buf = ctypes.create_string_buffer(CP210x_MAX_DEVICE_STRLEN)
        err = g_DLL.CP210x_GetDeviceManufacturerString(self.hDev, buf, ctypes.byref(cnt), True)
        if err != CP210x_SUCCESS :
            if CP210x_FUNCTION_NOT_SUPPORTED == err :
                print("CP210x_GetDeviceManufacturerString function not supported")
            else :
                print("CP210x_GetDeviceManufacturerString err %x" % err)
        return err, buf.value.decode()
    # CP2108
    # CP210x_SetManufacturerString(	LPVOID	lpvManufacturer, BYTE	bLength,	BOOL	bConvertToUnicode = TRUE );
    def CP210x_SetManufacturerString(self, str):
        err = g_DLL.CP210x_SetManufacturerString(self.hDev, str, len( str), True)
        if err != CP210x_SUCCESS :
            print("CP210x_SetManufacturerString err %x" % err)
        return err

    # CP210xRT_GetDeviceProductString(HANDLE cyHandle, LPVOID lpProduct, LPBYTE lpbLength, BOOL bConvertToASCII = TRUE);
    def CP210x_GetDeviceProductString(self):
        cnt = ctypes.c_ubyte(0)
        buf = ctypes.create_string_buffer(CP210x_MAX_DEVICE_STRLEN)
        err = g_DLL.CP210x_GetDeviceProductString(self.hDev, buf, ctypes.byref(cnt), True)
        if err != CP210x_SUCCESS :
            print("CP210x_GetDeviceProductString err %x" % err)
        return err, buf.value.decode()
    # CP210x_SetProductString(	LPVOID	lpvProduct,	BYTE	bLength,	BOOL	bConvertToUnicode = TRUE	);
    def CP210x_SetProductString(self, str):
        err = g_DLL.CP210x_SetProductString(self.hDev, str, len(str), True)
        if err != CP210x_SUCCESS :
            print("CP210x_SetProductString err %x" % err)
        return err

    # CP210xRT_GetDeviceSerialNumber(HANDLE cyHandle, LPVOID lpSerialNumber, BYTE InterfaceNumber, LPBYTE lpbLength, BOOL bConvertToASCII = TRUE);
    def CP210x_GetDeviceSerialNumber(self):
        cnt = ctypes.c_ubyte(0)
        buf = ctypes.create_string_buffer(CP210x_MAX_DEVICE_STRLEN)
        err = g_DLL.CP210x_GetDeviceSerialNumber(self.hDev, buf, ctypes.byref(cnt), True)
        if err != CP210x_SUCCESS :
            print("CP210x_GetDeviceSerialNumber err %x" % err)
        return err, buf.value.decode()
    # CP210x_SetSerialNumber(	LPVOID	lpvSerialNumber,	BYTE	bLength,	BOOL	bConvertToUnicode = TRUE	);
    def CP210x_SetSerialNumber(self, str):
        err = g_DLL.CP210x_SetSerialNumber(self.hDev, str, len( str), True)
        if err != CP210x_SUCCESS :
            print("CP210x_SetSerialNumber err %x" % err)
        return err

    def CP210x_GetSelfPower(self):
        bSelfPower = ctypes.c_bool(False)
        err = g_DLL.CP210x_GetSelfPower(self.hDev, ctypes.byref(bSelfPower))
        if err != CP210x_SUCCESS :
            print("CP210x_GetSelfPower err %x" % err)
        return err, bSelfPower.value
    def CP210x_SetSelfPower(self, bSelfPower):
        err = g_DLL.CP210x_SetSelfPower(self.hDev, bSelfPower)
        if err != CP210x_SUCCESS :
            print("CP210x_SetSelfPower err %x" % err)
        return err

    def CP210x_GetMaxPower(self):
        bPower = ctypes.c_ubyte(0)
        err = g_DLL.CP210x_GetMaxPower(self.hDev, ctypes.byref(bPower))
        if err != CP210x_SUCCESS :
            print("CP210x_GetMaxPower err %x" % err)
        return err, bPower.value
    def CP210x_SetMaxPower(self, bPower):
        err = g_DLL.CP210x_SetMaxPower(self.hDev, bPower)
        if err != CP210x_SUCCESS :
            print("CP210x_SetMaxPower err %x" % err)
        return err

    def CP210x_GetDeviceVersion(self):
        wVersion = ctypes.c_ushort(0)
        err = g_DLL.CP210x_GetDeviceVersion(self.hDev, ctypes.byref(wVersion))
        if err != CP210x_SUCCESS :
            print("CP210x_GetDeviceVersion err %x" % err)
        return err, wVersion.value
    def CP210x_SetDeviceVersion(self, wVersion):
        err = g_DLL.CP210x_SetDeviceVersion(self.hDev, wVersion)
        if err != CP210x_SUCCESS :
            print("CP210x_SetDeviceVersion err %x" % err)
        return err

    def CP210x_GetDeviceVid(self):
        vid = ctypes.c_ushort(0)
        err = g_DLL.CP210x_GetDeviceVid(self.hDev, ctypes.byref(vid))
        if err != CP210x_SUCCESS :
            print("CP210x_GetDeviceVid err %x" % err)
        return err, vid.value
    def CP210x_SetVid(self, wVid):
        err = g_DLL.CP210x_SetVid(self.hDev, wVid)
        if err != CP210x_SUCCESS :
            print("CP210x_SetVid err %x" % err)
        return err

    def CP210x_GetDevicePid(self):
        pid = ctypes.c_ushort(0)
        err = g_DLL.CP210x_GetDevicePid(self.hDev, ctypes.byref(pid))
        if err != CP210x_SUCCESS :
            print("CP210x_GetDevicePid err %x" % err)
        return err, pid.value
    def CP210x_SetPid(self, wPid):
        err = g_DLL.CP210x_SetPid(self.hDev, wPid)
        if err != CP210x_SUCCESS :
            print("CP210x_SetPid err %x" % err)
        return err

    def CP210x_Reset(self):
        err = g_DLL.CP210x_Reset( self.hDev)
        if err != CP210x_SUCCESS :
            print("CP210x_Reset err %x" % err)
        return err

    #--------------------------------------------------
    # Customization functions that work only on specific devices

    # CP2104, CP2105, CP2108
    def CP210x_GetFlushBufferConfig(self):
        wFlushBufferConfig = ctypes.c_ushort(0)
        err = g_DLL.CP210x_GetFlushBufferConfig(self.hDev, ctypes.byref(wFlushBufferConfig))
        if err != CP210x_SUCCESS :
            print("CP210x_GetFlushBufferConfig err %x" % err)
        return err, wFlushBufferConfig.value
    # CP2104, CP2105, CP2108
    def CP210x_SetFlushBufferConfig(self, wFlushBufferConfig):
        err = g_DLL.CP210x_SetFlushBufferConfig(self.hDev, wFlushBufferConfig)
        if err != CP210x_SUCCESS :
            print("CP210x_SetFlushBufferConfig err %x" % err)
        return err

    # CP2105
    def CP210x_GetDeviceMode(self):
        bDeviceModeECI = ctypes.c_ubyte(0)
        bDeviceModeSCI = ctypes.c_ubyte(0)
        err = g_DLL.CP210x_GetDeviceMode(self.hDev, ctypes.byref(bDeviceModeECI), ctypes.byref(bDeviceModeSCI))
        if err != CP210x_SUCCESS :
            print("CP210x_GetDeviceMode err %x" % err)
        return err, bDeviceModeECI.value, bDeviceModeSCI.value
    # CP2105
    def CP210x_SetDeviceMode(self, bDeviceModeECI, bDeviceModeSCI):
        err = g_DLL.CP210x_SetDeviceMode(self.hDev, bDeviceModeECI, bDeviceModeSCI)
        if err != CP210x_SUCCESS :
            print("CP210x_SetDeviceMode err %x" % err)
        return err

    # CP2105 and CP2108
    # CP210xRT_GetDeviceInterfaceString(HANDLE cyHandle, BYTE bInterfaceNumber, LPVOID lpInterfaceString,
    #       LPBYTE lpbLength, BOOL bConvertToASCII = TRUE);
    def CP210x_GetDeviceInterfaceString(self, InterfaceNumber):
        cnt = ctypes.c_ubyte(0)
        buf = ctypes.create_string_buffer(CP210x_MAX_DEVICE_STRLEN)
        err = g_DLL.CP210x_GetDeviceInterfaceString(self.hDev, InterfaceNumber, buf, ctypes.byref(cnt), True)
        if err != CP210x_SUCCESS :
            print("CP210x_GetDeviceInterfaceString err %x" % err)
        return err, buf.value.decode()

    # CP2105 and CP2108
    def CP210xRT_ReadLatch(self, lpLatch):
        err = g_RtDLL.CP210xRT_ReadLatch(self.hDev, ctypes.byref(lpLatch))
        if err != CP210x_SUCCESS :
            print("CP210xRT_ReadLatch err %x" % err)
        return err
    # CP2105 and CP2108
    def CP210xRT_WriteLatch(self, mask, latch):
        err = g_RtDLL.CP210xRT_WriteLatch(self.hDev, mask, latch)
        if err != CP210x_SUCCESS :
            print("CP210xRT_WriteLatch err %x" % err)
        return err

    # CP2102
    # CP210x_GetBaudRateConfig(HANDLE cyHandle, BAUD_CONFIG* baudConfigData);
    def CP210x_GetBaudRateConfig(self, BcList):
        if len(BcList) != 0:
            print("CP210x_SetBaudRateConfig err BcList must be empty")
            return CP210x_INVALID_PARAMETER
        # DLL must have this much and no less
        C_baudConfigData  = ctypes.create_string_buffer(BAUD_CONFIG_size * NUM_BAUD_CONFIGS)
        # read data from the device
        err = g_DLL.CP210x_GetBaudRateConfig(self.hDev, C_baudConfigData)
        if err != CP210x_SUCCESS :
            print("CP210x_GetBaudRateConfig err %x" % err)
            return err
        # re-code device data into the parameter list
        for i in range(0,NUM_BAUD_CONFIGS):
            BcList.append(BAUD_CONFIG())
            baudConfigData = struct.unpack_from(BAUD_CONFIG_FMT, C_baudConfigData, BAUD_CONFIG_size * i)
            BcList[i].BaudGen       = baudConfigData[0]
            BcList[i].Timer0Reload  = baudConfigData[1]
            BcList[i].Prescaler     = baudConfigData[2]
            BcList[i].BaudRate      = baudConfigData[3]
        return CP210x_SUCCESS
    # CP2102
    # CP210x_SetBaudRateConfig(	BAUD_CONFIG* baudConfigData	);
    def CP210x_SetBaudRateConfig(self, BcList):
        if len(BcList) != NUM_BAUD_CONFIGS:
            print("CP210x_SetBaudRateConfig err BcList must have NUM_BAUD_CONFIGS elements")
            return CP210x_INVALID_PARAMETER
        # DLL must have this much and no less
        C_baudConfigData  = ctypes.create_string_buffer(BAUD_CONFIG_size * NUM_BAUD_CONFIGS)
        # re-code the parameter list into the device data
        for i in range(0,NUM_BAUD_CONFIGS):
            struct.pack_into(BAUD_CONFIG_FMT, C_baudConfigData, BAUD_CONFIG_size * i, BcList[i].BaudGen, BcList[i].Timer0Reload, BcList[i].Prescaler, BcList[i].BaudRate)
        # write data to the device
        err = g_DLL.CP210x_SetBaudRateConfig(self.hDev, C_baudConfigData)
        if err != CP210x_SUCCESS :
            print("CP210x_SetBaudRateConfig err %x" % err)
        return err

    # CP2102N
    # Returns the firmware version burned on the device
    def CP210x_GetFirmwareVersion(self):
        C_FirmwareVersion  = ctypes.create_string_buffer(firmware_t_size)
        err = g_DLL.CP210x_GetFirmwareVersion(self.hDev, ctypes.byref(C_FirmwareVersion))        
        if err != CP210x_SUCCESS :
            print("CP210x_GetFirmwareVersion err %x" % err)
            return err
        return err, firmware_t(struct.unpack( "B", C_FirmwareVersion[0])[0],
            struct.unpack( "B", C_FirmwareVersion[1])[0],
            struct.unpack( "B", C_FirmwareVersion[2])[0],
            )

    # Returns the config block burned on the device
    def CP210x_GetConfig(self):
        C_ConfigData  = ctypes.create_string_buffer(1024)
        err = g_DLL.CP210x_GetConfig(self.hDev, ctypes.byref(C_ConfigData), 2)
        if err != CP210x_SUCCESS :
            print("CP210x_GetConfig err %x" % err)
            return err
        configSize = struct.unpack("B", C_ConfigData[0])[0] + (struct.unpack("B", C_ConfigData[1])[0] << 8)
        C_ConfigData = ctypes.create_string_buffer(configSize)
        err = g_DLL.CP210x_GetConfig(self.hDev, ctypes.byref(C_ConfigData), configSize)
        if err != CP210x_SUCCESS :
            print("CP210x_GetConfig err %x" % err)
            return err
        return err, C_ConfigData

    # Write the config block burned on the device
    # Function first reads the size of the config block currently on the device
    # and then sends that much ConfigData back.  Caller must ensure that the size
    # of ConfigData matches the size of the config block.
    def CP210x_SetConfig(self, ConfigData):
        C_ConfigSize = ctypes.create_string_buffer(2)
        err = g_DLL.CP210x_GetConfig(self.hDev, ctypes.byref(C_ConfigSize), 2)
        if err != CP210x_SUCCESS :
            print("CP210x_SetConfig err %x" % err)
            return err
        configSize = struct.unpack("B", C_ConfigSize[0])[0] + (struct.unpack("B", C_ConfigSize[1])[0] << 8)
        err = g_DLL.CP210x_SetConfig(self.hDev, ctypes.byref(ConfigData), configSize)
        if err != CP210x_SUCCESS :
            print("CP210x_SetConfig err %x" % err)
            return err
        return err  

    # Order the device to enter Bootloader mode
    # Currently only support CP2102N Bootloader
    # Separate app required for Bootloader upload.
    def CP210x_UpdateFirmware(self):
        err = g_DLL.CP210x_UpdateFirmware(self.hDev)
        if err != CP210x_SUCCESS :
            print("CP210x_UpdateFirmware err %x" % err)
            return err
        return err

    # Use the DLL to send a generic output setup command
    def CP210x_SetGeneric(self, Data, Size):
        err = g_DLL.CP210x_SetGeneric(self.hDev, ctypes.byref(Data), Size)
        if err != 0 :
            print("CP210x_SetGeneric err %x" % err)
        return err
        
    # Use the DLL to send a generic input setup command
    def CP210x_GetGeneric(self, Data, Size):
        err = g_DLL.CP210x_GetGeneric(self.hDev, ctypes.byref(Data), Size)
        if err != 0 :
            print("CP210x_GetGeneric err %x" % err)
        return err, Data

def PRINTV(*arg):
    print(arg)
    pass

def DumpDeviceInfo(DevIndex):
    buf = ctypes.create_string_buffer(CP210x_MAX_DEVICE_STRLEN)

    PRINTV("--- SetupDi data --- device #%d" % DevIndex)
    if device.CP210x_GetProductString(DevIndex, buf, CP210x_RETURN_SERIAL_NUMBER) != CP210x_SUCCESS:
        sys.exit(1)
    PRINTV("ProductString CP210x_RETURN_SERIAL_NUMBER : %s" % buf.value.decode())

    if device.CP210x_GetProductString(DevIndex, buf, CP210x_RETURN_DESCRIPTION) != CP210x_SUCCESS:
        sys.exit(1)
    PRINTV("ProductString CP210x_RETURN_DESCRIPTION   : %s" % buf.value.decode())

    if sys.platform == 'win32':
        if device.CP210x_GetProductString(DevIndex, buf, CP210x_RETURN_FULL_PATH) != CP210x_SUCCESS:
            sys.exit(1)
        PRINTV("ProductString CP210x_RETURN_FULL_PATH     : %s" % buf.value.decode())

    if device.CP210x_Open(DevIndex) != CP210x_SUCCESS:
        return -1
 #       sys.exit(1)

    PRINTV("--- Hardware data --- device #%d" % DevIndex)
    (status, PartNum) = device.CP210x_GetPartNumber()
    if status != CP210x_SUCCESS :
        sys.exit(1)
    PRINTV("PartNumber: 0x%X" % PartNum)

    (status, devVID) = device.CP210x_GetDeviceVid()
    if status != CP210x_SUCCESS:
        sys.exit(1)
    (status, devPID) = device.CP210x_GetDevicePid()
    if status != CP210x_SUCCESS:
        sys.exit(1)
    PRINTV("devVID 0x%X devPID 0x%X" % (devVID, devPID))

    (status, bSelfPower) = device.CP210x_GetSelfPower()
    if status != CP210x_SUCCESS:
        sys.exit(1)
    PRINTV("SelfPower: 0x%x" % bSelfPower)

    (status, bPower) = device.CP210x_GetMaxPower()
    if status != CP210x_SUCCESS:
        sys.exit(1)
    PRINTV("MaxPower: 0x%x" % bPower)

    (status, wVersion) = device.CP210x_GetDeviceVersion()
    if status != CP210x_SUCCESS:
        sys.exit(1)
    PRINTV("DeviceVersion: 0x%x" % wVersion)

    (status, str) = device.CP210x_GetDeviceManufacturerString()
    if status == CP210x_SUCCESS :
        PRINTV("DeviceManufacturerString: %s" % str)
    else :
        if CP210x_FUNCTION_NOT_SUPPORTED != status :
            return -1

    (status, str) = device.CP210x_GetDeviceProductString()
    if status != CP210x_SUCCESS :
        sys.exit(1)
    PRINTV("DeviceProductString: %s" % str)

    (status, str) = device.CP210x_GetDeviceSerialNumber()
    if status != CP210x_SUCCESS :
        sys.exit(1)
    PRINTV("DeviceSerialNumber: %s" % str)

    if PartNum not in (CP210x_CP2108_VERSION, CP210x_CP2105_VERSION, CP210x_CP2102n_QFN28_VERSION, CP210x_CP2102n_QFN24_VERSION, CP210x_CP2102n_QFN20_VERSION):
        BcList = []
        if device.CP210x_GetBaudRateConfig( BcList ) != CP210x_SUCCESS:
            sys.exit(1)
        for i in range(0, len(BcList)):
            PRINTV("BaudGen: %x Timer0Reload: %x Prescaler: %x BaudRate: %d" % (BcList[i].BaudGen, BcList[i].Timer0Reload, BcList[i].Prescaler, BcList[i].BaudRate))

    if PartNum in (CP210x_CP2102n_QFN28_VERSION, CP210x_CP2102n_QFN24_VERSION, CP210x_CP2102n_QFN20_VERSION):
        err, firmwareVersion = device.CP210x_GetFirmwareVersion()
        if err != CP210x_SUCCESS:
            sys.exit(1)
        PRINTV("Major ", firmwareVersion.Major, " Minor ", firmwareVersion.Minor, " Build ", firmwareVersion.Build)
        err, ConfigData = device.CP210x_GetConfig()
        if err != CP210x_SUCCESS:
            sys.exit(1)
        PRINTV("ConfigData ", ConfigData.raw)
        # Manufacturing.dll does not expose Write/Read latch functions
        # so write/read latch can be used to test generic functions
        GetGpio = ctypes.create_string_buffer(9)
        GetGpio[0] = b'\xC0'
        GetGpio[1] = b'\xFF'
        GetGpio[2] = b'\xC2'
        GetGpio[3] = b'\x00'
        GetGpio[4] = b'\x00'
        GetGpio[5] = b'\x00'
        GetGpio[6] = b'\x01' # 1 byte returned
        GetGpio[7] = b'\x00'
        GetGpio[8] = b'\x00' # 1 byte buffer for data
        err, result = device.CP210x_GetGeneric(GetGpio, 9)
        PRINTV("CP210x_GetGeneric ", result.raw)        
            
    if PartNum in (CP210x_CP2104_VERSION, CP210x_CP2105_VERSION, CP210x_CP2108_VERSION):
        (status, wFlushBufferConfig) = device.CP210x_GetFlushBufferConfig()
        if status != CP210x_SUCCESS:
            sys.exit(1)
        PRINTV("FlushBufferConfig: %x" % wFlushBufferConfig)

    if PartNum in (CP210x_CP2108_VERSION, CP210x_CP2105_VERSION):
        (status, str) = device.CP210x_GetDeviceInterfaceString(0)
        if status != CP210x_SUCCESS :
            sys.exit(1)
        PRINTV("InterfaceString #0: %s" % str)
        (status, str) = device.CP210x_GetDeviceInterfaceString(1)
        if status != CP210x_SUCCESS :
            sys.exit(1)
        PRINTV("InterfaceString #1: %s" % str)
    if PartNum == CP210x_CP2108_VERSION:
            (status, str) = device.CP210x_GetDeviceInterfaceString(2)
            if status != CP210x_SUCCESS :
                sys.exit(1)
            PRINTV("InterfaceString #2: %s" % str)
            (status, str) = device.CP210x_GetDeviceInterfaceString(3)
            if status != CP210x_SUCCESS :
                sys.exit(1)
            PRINTV("InterfaceString #3: %s" % str)
    if PartNum == CP210x_CP2105_VERSION:
        (status, bDeviceModeECI, bDeviceModeSCI) = device.CP210x_GetDeviceMode()
        if status != CP210x_SUCCESS:
            sys.exit(1)
        PRINTV("bDeviceModeECI: %x bDeviceModeSCI: %x" % (bDeviceModeECI, bDeviceModeSCI))

# Uncomment this if you want to have fun with GPIOs.
#    if PartNum in (CP210x_CP2108_VERSION, CP210x_CP2105_VERSION, CP210x_CP2104_VERSION, CP210x_CP2103_VERSION):
#        latch = ctypes.c_ushort(0)
#        if device.CP210xRT_ReadLatch( latch) != CP210x_SUCCESS:
#            sys.exit(1)
#        print("latch %x" % latch.value)
#        if device.CP210xRT_WriteLatch( 0x000f, 0x000e) != CP210x_SUCCESS:
#            sys.exit(1)
#        if device.CP210xRT_ReadLatch( latch) != CP210x_SUCCESS:
#            sys.exit(1)
#        print("latch %x" % latch.value)

    if device.CP210x_Close() != CP210x_SUCCESS:
        sys.exit(1)

    return 0

def TestInvalDevIndex(device, NumDevices):
    if device.CP210x_Open(NumDevices) != CP210x_DEVICE_NOT_FOUND:
        return -1
    return 0

#------------------------------------------------------
# This main does no harm, just gets/dumps info about all found devices
# Its purpose is to call all non-destructive wrappers as unit-testing.
#
if __name__ == "__main__":
    import binascii

    device = CP210xManufacturing()

    (status, NumDevices) = device.CP210x_GetNumDevices()
    if status != CP210x_SUCCESS:
        print("FAIL\n")
        sys.exit(1)
    print("Number of CP210xDevices = %d\n" % NumDevices);

    if TestInvalDevIndex(device, NumDevices) != 0:
        print("FAIL\n")
        sys.exit(1)

    for i in range(0, NumDevices):
        if 0 != DumpDeviceInfo(i):
            print("FAIL\n")
            sys.exit(1)

    print("PASS\n")
    sys.exit(0)

#-------------------------------------------------------------------------------
# Functions yet to be wrapped
# All with HHANDLE as the first parameter

"""
CP2105 or CP2108
CP210x_SetInterfaceString(	BYTE bInterfaceNumber,	LPVOID lpvInterface,	BYTE bLength,	BOOL bConvertToUnicode	);

CP2102 doesn't support these
CP210x_SetPortConfig(		PORT_CONFIG*	PortConfig	);
CP210x_SetDualPortConfig(	DUAL_PORT_CONFIG*	DualPortConfig	);
CP210x_SetQuadPortConfig(	QUAD_PORT_CONFIG*	QuadPortConfig	);
CP210x_GetPortConfig(		PORT_CONFIG*	PortConfig	);
CP210x_GetDualPortConfig(	DUAL_PORT_CONFIG*	DualPortConfig	);
CP210x_GetQuadPortConfig(	QUAD_PORT_CONFIG*	QuadPortConfig	);

better keep away from this
CP210x_GetLockValue(	LPBYTE	lpbLockValue	);
CP210x_SetLockValue(	);

CP210x_CreateHexFile(	LPCSTR lpvFileName	);
"""
