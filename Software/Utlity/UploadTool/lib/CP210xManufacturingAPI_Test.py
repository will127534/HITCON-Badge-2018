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

#-------------------------------------------------------------------------------
# USER GUIDE
# This app tests the DLL on all found CP2102 devices.
# Returns 0 to shell on success, non-zero on error
# The app doesn't change VID PID. It changes several other customizable
# parameters of the device, resets the device and then reads them back
# to verify. It has 2 sets of the parameters: "normal" (what the stock CP2102
# would have) and "test". On every run, it first programs the "test" parameters,
#then "normal", leaving CP2102 in its stock state.

import sys
import time
from CP210xManufacturing import *

def StrToUshort(s):
    retval = int(s, 16) # this too can raise ValueError
    if retval > 0xffff:
        raise ValueError
    if retval < 0:
        raise ValueError
    return retval

#-------------------------------------------------------------------------------
# Customizable parameters changed by the app
# index 0 - "normaL", index 1 - "test"

CustSelfPower = (False,True)
CustMaxPower = (0x32, 0x37) # 100 mA, 110 mA
CustVersion = (0x0100, 0x0137)
CustProductString = ("CP2102 USB to UART Bridge Controller", "API_Test_CP2102 USB to UART Bridge Controller")
CustSerialNumber = ("0001", "API_Test_0001")

#------------------------------------------------------
# Open the device and make sure it's CP2102
def OpenCP2102(lib, DevIndex, argVID, argPID):
    PRINTV("Open CP2102")

    if lib.CP210x_Open(DevIndex) != CP210x_SUCCESS:
        return -1
# A second open should fail.
#    if device.CP210x_Open(DevIndex) == CP210x_SUCCESS:
#        print("A second opened did not fail (and should?)")
#        device.CP210x_Close()
#        return -1

    (status, devVID) = lib.CP210x_GetDeviceVid()
    if status != CP210x_SUCCESS:
        lib.CP210x_Close()
        return -1
    (status, devPID) = lib.CP210x_GetDevicePid()
    if status != CP210x_SUCCESS:
        lib.CP210x_Close()
        return -1
    if devVID != argVID:
        print("unexpected: VID 0x%x (expected 0x%x)" % (devVID.value, argVID))
        # lib.CP210x_Close()
        # return -1
    if devPID != argPID and devPID != 0xEA60 and devPID != 0xEA61 and devPID != 0xEA70 and devPID != 0xEA71 and devPID != 0xEAD0:
        print("unexpected: PID 0x%x (expected 0x%x)" % (devPID.value, argPID))
        # lib.CP210x_Close()
        # return -1
    (status, PartNum) = lib.CP210x_GetPartNumber()
    if status != CP210x_SUCCESS :
        lib.CP210x_Close()
        return -1
    if PartNum != CP210x_CP2102_VERSION:
        print("error: PartNumber %x isn't CP2102" % PartNum)
        lib.CP210x_Close()
        return -1
    return 0

def CustomizeOpenedCP2102(lib, CfgIndex):

    if lib.CP210x_SetSelfPower(CustSelfPower[CfgIndex]) != CP210x_SUCCESS:
        return -1

    if lib.CP210x_SetMaxPower(CustMaxPower[CfgIndex]) != CP210x_SUCCESS:
        return -1

    if lib.CP210x_SetDeviceVersion(CustVersion[CfgIndex]) != CP210x_SUCCESS:
        return -1

    if lib.CP210x_SetProductString(CustProductString[CfgIndex].encode('utf-8')) != CP210x_SUCCESS :
        return -1

    if lib.CP210x_SetSerialNumber(CustSerialNumber[CfgIndex].encode('utf-8')) != CP210x_SUCCESS :
        return -1

    return 0;

def VerifyOpenedCP2102(device, CfgIndex):
    err = 0

    (status, bSelfPower) = device.CP210x_GetSelfPower()
    if status != CP210x_SUCCESS:
        return -1
    if bSelfPower != CustSelfPower[CfgIndex]:
        print("error: SelfPower: %x unexpected" % bSelfPower)
        err = -1

    (status, bMaxPower) = device.CP210x_GetMaxPower()
    if status != CP210x_SUCCESS:
        return -1
    if bMaxPower != CustMaxPower[CfgIndex]:
        print("error: MaxPower: 0x%x unexpected" % bMaxPower)
        err = -1

    (status, wVersion) = device.CP210x_GetDeviceVersion()
    if status != CP210x_SUCCESS:
        return -1
    if wVersion != CustVersion[CfgIndex]:
        print("error: DeviceVersion: 0x%x unexpected" % wVersion)
        err = -1

    (status, str) = device.CP210x_GetDeviceProductString()
    if status != CP210x_SUCCESS :
        return -1
    if str != CustProductString[CfgIndex]:
        print("error: DeviceProductString: %s unexpected" % str)
        err = -1

    (status, str) = device.CP210x_GetDeviceSerialNumber()
    if status != CP210x_SUCCESS :
        return -1
    if str != CustSerialNumber[CfgIndex]:
        print("error: DeviceSerialNumber: %s unexpected" % str)
        err = -1

    return err;

def CustomizeCP2102(lib, DevIndex, VID, PID, CfgIndex):
    PRINTV("--- CustomizeCP2102 #%d config #%d---" % (DevIndex, CfgIndex))
    if OpenCP2102(lib, DevIndex, VID, PID) != 0:
        return -1
    if CustomizeOpenedCP2102(lib, CfgIndex) != 0:
        lib.CP210x_Close()
        return -1
    PRINTV("Reset CP2102")
    if lib.CP210x_Reset() != CP210x_SUCCESS:
        lib.CP210x_Close()
        return -1
    if lib.CP210x_Close() != CP210x_SUCCESS:
        return -1
    time.sleep(8) # seconds. must wait some for device to re-enumerate after reset
    return 0

def VerifyCP2102(lib, DevIndex, VID, PID, CfgIndex):
    PRINTV("--- VerifyCP2102 #%d config #%d---" % (DevIndex, CfgIndex))
    if OpenCP2102(lib, DevIndex, VID, PID) != 0:
        return -1
    if VerifyOpenedCP2102(lib, CfgIndex) != 0:
        PRINTV("Verification FAILed")
        lib.CP210x_Close()
        return -1
    PRINTV("Successful verification")
    if lib.CP210x_Close() != CP210x_SUCCESS:
        return -1
    return 0

def TestCP2102(lib, NumDevices, VID, PID):
    for i in range(0, NumDevices):
        if CustomizeCP2102(lib, i, VID, PID, 1) != 0:
           return -1
    for i in range(0, NumDevices):
        if VerifyCP2102(lib, i, VID, PID, 1) != 0:
           return -1
    for i in range(0, NumDevices):
        if CustomizeCP2102(lib, i, VID, PID, 0) != 0:
           return -1
    for i in range(0, NumDevices):
        if VerifyCP2102(lib, i, VID, PID, 0) != 0:
           return -1
    return 0

if __name__ == "__main__":

    #------------------------------------------------------
    # Determine VID PID - the default is CP2102 or read 16-bit hex VID and PID from command line if specified
    VID = 0x10c4
    PID = 0xea60

    if len(sys.argv) != 1 :
        if len(sys.argv) != 3 :
            print("error: invalid command line arguments: %s" % str(sys.argv))
            sys.exit(1)

        try:
            VID = StrToUshort(sys.argv[1])
            PID = StrToUshort(sys.argv[2])
        except ValueError:
            print("error: invalid command line arguments: %s" % str(sys.argv))
            sys.exit(1)

    #------------------------------------------------------
    errorlevel = 1
    lib = CP210xManufacturing()
    try:
        (status, NumDevices) = lib.CP210x_GetNumDevices()
        if status == CP210x_SUCCESS:
            if TestInvalDevIndex(lib, NumDevices) != 0:
                sys.exit(1)
#            if NumDevices.value == 0:
#                print("API_Test:  no devices found")
#                sys.exit(1)
            if TestCP2102(lib, NumDevices, VID, PID) == 0:
                errorlevel = 0 # let shell know that test PASSED
    except:
        print("API_Test: Unhandled exception")
    finally:
        if errorlevel:
            print("FAIL\n")
        else:
            print("PASS\n")
        sys.exit(errorlevel)
