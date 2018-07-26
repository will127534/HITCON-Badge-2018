#ifndef	HOST_APPLICATIONS_DLLS_MANUFACTURINGDLL_CP210XMANUFACTURINGDLL_H_INCLUDED_XF3N9QM3BK
#define	HOST_APPLICATIONS_DLLS_MANUFACTURINGDLL_CP210XMANUFACTURINGDLL_H_INCLUDED_XF3N9QM3BK

#include "silabs_sal.h"

#ifdef _WIN32
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CP210xDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CP210xDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CP210xDLL_EXPORTS
#define CP210xDLL_API
#else
#define CP210xDLL_API __declspec(dllimport)
#pragma comment (lib, "CP210xManufacturing.lib")
#endif
#else // !_WIN32
#define CP210xDLL_API
#define WINAPI
#endif // !_WIN32

#if ! defined(_PREFAST_)
// For SAL annotations used below in this header file, for each annotation used, add a suitable "if not defined" no-op
// definition for it here so that the SAL annotations do not break builds in IDEs that do not have SAL-annotation support.
#if ! defined(_Check_return_)
#define	_Check_return_
#endif // ! defined(_Check_return_)
#if ! defined(_Ret_range_)
#define _Ret_range_(lb,ub)
#endif // ! defined(_Ret_range_)
#if ! defined(_Success_)
#define	_Success_(expr)
#endif // ! defined(_Success_)
#if ! defined(_In_)
#define	_In_
#endif // ! defined(_In_)
#if ! defined(_Out_writes_bytes_)
#define	_Out_writes_bytes_(n)
#endif // ! defined(_Out_writes_bytes_)
#if ! defined(_Pre_defensive_)
#define	_Pre_defensive_
#endif // ! defined(_Pre_defensive_)
#endif // ! defined(_PREFAST_)

// GetProductString() function flags
#define		CP210x_RETURN_SERIAL_NUMBER			0x00
#define		CP210x_RETURN_DESCRIPTION			0x01
#define		CP210x_RETURN_FULL_PATH				0x02

// GetDeviceVersion() return codes
#define		CP210x_CP2101_VERSION				0x01
#define		CP210x_CP2102_VERSION				0x02
#define		CP210x_CP2103_VERSION				0x03
#define		CP210x_CP2104_VERSION				0x04
#define		CP210x_CP2105_VERSION				0x05
#define		CP210x_CP2108_VERSION				0x08
#define     CP210x_CP2109_VERSION               0x09
#define     CP210x_CP2102N_QFN28_VERSION		0x20
#define     CP210x_CP2102N_QFN24_VERSION		0x21
#define     CP210x_CP2102N_QFN20_VERSION		0x22

// Return codes									
#define		CP210x_SUCCESS						0x00
#define		CP210x_DEVICE_NOT_FOUND				0xFF
#define		CP210x_INVALID_HANDLE				0x01
#define		CP210x_INVALID_PARAMETER			0x02
#define		CP210x_DEVICE_IO_FAILED				0x03
#define		CP210x_FUNCTION_NOT_SUPPORTED		0x04
#define		CP210x_GLOBAL_DATA_ERROR			0x05
#define		CP210x_FILE_ERROR					0x06
#define		CP210x_COMMAND_FAILED				0x08
#define		CP210x_INVALID_ACCESS_TYPE			0x09

// Type definitions
typedef		int		CP210x_STATUS;
						
// Buffer size limits
//
//
// CP2101/2/3/4/9
#define		CP210x_MAX_DEVICE_STRLEN			256
#define     CP210x_MAX_MANUFACTURER_STRLEN      45

#define		CP210x_MAX_PRODUCT_STRLEN			126
#define		CP210x_MAX_SERIAL_STRLEN			63
//
// CP2105
#define     CP2105_MAX_MANUFACTURER_STRLEN      12
#define		CP2105_MAX_PRODUCT_STRLEN			47
#define		CP2105_MAX_SERIAL_STRLEN			16
#define		CP2105_MAX_INTERFACE_STRLEN			32
//
// CP2108
#define     CP2108_MAX_MANUFACTURER_STRLEN      126
#define		CP2108_MAX_PRODUCT_STRLEN			126
#define		CP2108_MAX_SERIAL_STRLEN			126
#define		CP2108_MAX_INTERFACE_STRLEN			126
//
// CP2102N
#define		CP2102N_MAX_
// Type definitions
typedef		char	CP210x_DEVICE_STRING[CP210x_MAX_DEVICE_STRLEN];
typedef		char	CP210x_MANUFACTURER_STRING[CP210x_MAX_MANUFACTURER_STRLEN];
typedef		char	CP210x_PRODUCT_STRING[CP210x_MAX_PRODUCT_STRLEN];
typedef		char	CP210x_SERIAL_STRING[CP210x_MAX_SERIAL_STRLEN];
typedef		char	CP2105_INTERFACE_STRING[CP2105_MAX_INTERFACE_STRLEN];
typedef		char	CP2108_INTERFACE_STRING[CP2108_MAX_INTERFACE_STRLEN];

#define		CP210x_MAX_MAXPOWER					250


// Baud Rate Aliasing definitions
//
#define		NUM_BAUD_CONFIGS	32
//
// Baud Config Structure
typedef struct
{
	WORD	BaudGen;
	WORD	Timer0Reload;
	BYTE	Prescaler;
	DWORD	BaudRate;
} BAUD_CONFIG, *PBAUD_CONFIG;
//
// Size of the Baud Config Structure
#define		BAUD_CONFIG_SIZE	10
//
// Array of all Baud Rate Configurations
typedef		BAUD_CONFIG		BAUD_CONFIG_DATA[NUM_BAUD_CONFIGS];
//


// Flush Buffer definitions
//
// CP2104
#define FC_OPEN_TX		0x01	// When these bits are set, the device will flush that buffer
#define FC_OPEN_RX		0x02
#define FC_CLOSE_TX		0x04
#define FC_CLOSE_RX		0x08
//
// CP2105 - Standard Port
#define FC_OPEN_TX_SCI  FC_OPEN_TX
#define FC_OPEN_RX_SCI  FC_OPEN_RX
#define FC_CLOSE_TX_SCI FC_CLOSE_TX
#define FC_CLOSE_RX_SCI FC_CLOSE_RX
//
// CP2105 - Enhanced Port
#define FC_OPEN_TX_ECI  0x10
#define FC_OPEN_RX_ECI  0x20
#define FC_CLOSE_TX_ECI 0x40
#define FC_CLOSE_RX_ECI 0x80
//
// CP2108
#define FC_OPEN_TX_IFC0  0x0001
#define FC_OPEN_RX_IFC0  0x0002
#define FC_CLOSE_TX_IFC0 0x0004
#define FC_CLOSE_RX_IFC0 0x0008
#define FC_OPEN_TX_IFC1  0x0010
#define FC_OPEN_RX_IFC1  0x0020
#define FC_CLOSE_TX_IFC1 0x0040
#define FC_CLOSE_RX_IFC1 0x0080
#define FC_OPEN_TX_IFC2  0x0100
#define FC_OPEN_RX_IFC2  0x0200
#define FC_CLOSE_TX_IFC2 0x0400
#define FC_CLOSE_RX_IFC2 0x0800
#define FC_OPEN_TX_IFC3  0x1000
#define FC_OPEN_RX_IFC3  0x2000
#define FC_CLOSE_TX_IFC3 0x4000
#define FC_CLOSE_RX_IFC3 0x8000

//
//Port Config definitions
//

//
// CP2103/4 Port Config Structure
//
typedef		struct
{
	WORD Mode;			// Push-Pull = 1, Open-Drain = 0
	WORD Reset_Latch;	// Logic High = 1, Logic Low = =0
	WORD Suspend_Latch;	// Logic High = 1, Logic Low = =0
	unsigned char EnhancedFxn;
} PORT_CONFIG, *PPORT_CONFIG;
//
// Define bit locations for Mode/Latch for Reset and Suspend structures
#define PORT_RI_ON				0x0001
#define PORT_DCD_ON				0x0002
#define PORT_DTR_ON				0x0004
#define PORT_DSR_ON				0x0008
#define PORT_TXD_ON				0x0010
#define PORT_RXD_ON				0x0020
#define PORT_RTS_ON				0x0040
#define PORT_CTS_ON				0x0080
//
#define PORT_GPIO_0_ON			0x0100
#define PORT_GPIO_1_ON			0x0200
#define PORT_GPIO_2_ON			0x0400
#define PORT_GPIO_3_ON			0x0800
//
#define PORT_SUSPEND_ON			0x4000	//  Can't configure latch value
#define PORT_SUSPEND_BAR_ON		0x8000	//  Can't configure latch value
//
// Define bit locations for EnhancedFxn
#define EF_GPIO_0_TXLED				0x01	//  Under device control
#define EF_GPIO_1_RXLED				0x02	//  Under device control
#define EF_GPIO_2_RS485				0x04	//  Under device control
#define EF_RS485_INVERT				0x08	//  RS485 Invert bit
#define EF_WEAKPULLUP				0x10	//  Weak Pull-up on
#define EF_RESERVED_1				0x20	//	Reserved, leave bit 5 cleared
#define EF_SERIAL_DYNAMIC_SUSPEND	0x40	//  For 8 UART/Modem signals
#define EF_GPIO_DYNAMIC_SUSPEND		0x80	//  For 4 GPIO signals
//


//
// CP2105 Dual Port Config Structure
//
typedef		struct
{
	WORD Mode;			// Push-Pull = 1, Open-Drain = 0
	WORD Reset_Latch;	// Logic High = 1, Logic Low = =0
	WORD Suspend_Latch;	// Logic High = 1, Logic Low = =0
	unsigned char EnhancedFxn_ECI;
	unsigned char EnhancedFxn_SCI;
	unsigned char EnhancedFxn_Device;
} DUAL_PORT_CONFIG;
//
// CP2105 Define bit locations for Mode/Latch for Reset and Suspend structures
#define PORT_RI_SCI_ON			0x0001
#define PORT_DCD_SCI_ON			0x0002
#define PORT_DTR_SCI_ON			0x0004
#define PORT_DSR_SCI_ON			0x0008
#define PORT_TXD_SCI_ON			0x0010
#define PORT_RXD_SCI_ON			0x0020
#define PORT_RTS_SCI_ON			0x0040
#define PORT_CTS_SCI_ON			0x0080
#define PORT_GPIO_0_SCI_ON		0x0002
#define PORT_GPIO_1_SCI_ON		0x0004
#define PORT_GPIO_2_SCI_ON		0x0008
#define PORT_SUSPEND_SCI_ON		0x0001	//  Can't configure latch value
//
#define PORT_RI_ECI_ON			0x0100
#define PORT_DCD_ECI_ON			0x0200
#define PORT_DTR_ECI_ON			0x0400
#define PORT_DSR_ECI_ON			0x0800
#define PORT_TXD_ECI_ON			0x1000
#define PORT_RXD_ECI_ON			0x2000
#define PORT_RTS_ECI_ON			0x4000
#define PORT_CTS_ECI_ON			0x8000
#define PORT_GPIO_0_ECI_ON		0x0400
#define PORT_GPIO_1_ECI_ON		0x0800
#define PORT_SUSPEND_ECI_ON		0x0100	//  Can't configure latch value
//
// CP2105 Define bit locations for EnhancedFxn_ECI
#define EF_GPIO_0_TXLED_ECI			0x01	//  Under device control
#define EF_GPIO_1_RXLED_ECI			0x02	//  Under device control
#define EF_GPIO_1_RS485_ECI			0x04	//  Under device control
#define EF_RS485_INVERT				0x08	//  Under device control
#define EF_INVERT_SUSPEND_ECI		0x10	//  RS485 Invert bit
#define EF_DYNAMIC_SUSPEND_ECI		0x40	//  For GPIO signals
//
// CP2105 Define bit locations for EnhancedFxn_SCI
#define EF_GPIO_0_TXLED_SCI			0x01	//  Under device control
#define EF_GPIO_1_RXLED_SCI			0x02	//  Under device control
#define EF_INVERT_SUSPEND_SCI		0x10	//  RS485 Invert bit
#define EF_DYNAMIC_SUSPEND_SCI		0x40	//  For GPIO signals
//
// CP2105 Define bit locations for EnhancedFxn_Device
#define EF_WEAKPULLUP				0x10	//  Weak Pull-up on
//

//
// CP2108 Quad Port Config Structure
//
typedef struct
{
    WORD Mode_PB0;
    WORD Mode_PB1;
    WORD Mode_PB2;
    WORD Mode_PB3;
    WORD Mode_PB4;

    WORD LowPower_PB0;
    WORD LowPower_PB1;
    WORD LowPower_PB2;
    WORD LowPower_PB3;
    WORD LowPower_PB4;

    WORD Latch_PB0;
    WORD Latch_PB1;
    WORD Latch_PB2;
    WORD Latch_PB3;
    WORD Latch_PB4;
} QUAD_PORT_STATE;

typedef struct
{
	QUAD_PORT_STATE Reset_Latch;
	QUAD_PORT_STATE Suspend_Latch;
    BYTE IPDelay_IFC0;
    BYTE IPDelay_IFC1;
    BYTE IPDelay_IFC2;
    BYTE IPDelay_IFC3;
    BYTE EnhancedFxn_IFC0;
    BYTE EnhancedFxn_IFC1;
    BYTE EnhancedFxn_IFC2;
    BYTE EnhancedFxn_IFC3;
    BYTE EnhancedFxn_Device;
    BYTE ExtClk0Freq;
    BYTE ExtClk1Freq;
    BYTE ExtClk2Freq;
    BYTE ExtClk3Freq;
} QUAD_PORT_CONFIG;

//
// CP2108 Define bit locations for Mode/Latch for Reset and Suspend structures
// PB0
#define  PORT_TX0       0x0001
#define  PORT_RX0       0x0002
#define  PORT_RTS0      0x0004
#define  PORT_CTS0      0x0008
#define  PORT_DTR0      0x0010
#define  PORT_DSR0      0x0020
#define  PORT_DCD0      0x0040
#define  PORT_RI0       0x0080
#define  PORT_TX1       0x0100
#define  PORT_RX1       0x0200
#define  PORT_RTS1      0x0400
#define  PORT_CTS1      0x0800
#define  PORT_DTR1      0x1000
#define  PORT_DSR1      0x2000
#define  PORT_DCD1      0x4000
#define  PORT_RI1       0x8000

// PB1
#define  PORT_GPIO_0    0x0001	// (1<<0)
#define  PORT_GPIO_1    0x0002	// (1<<1)
#define  PORT_GPIO_2    0x0004	// (1<<2)
#define  PORT_GPIO_3    0x0008	// etc.
#define  PORT_GPIO_4    0x0010
#define  PORT_GPIO_5    0x0020
#define  PORT_GPIO_6    0x0040
#define  PORT_GPIO_7    0x0080
#define  PORT_GPIO_8    0x0100
#define  PORT_GPIO_9    0x0200
#define  PORT_GPIO_10   0x0400
#define  PORT_GPIO_11   0x0800
#define  PORT_GPIO_12   0x1000
#define  PORT_GPIO_13   0x2000
#define  PORT_GPIO_14   0x4000
#define  PORT_GPIO_15   0x8000

// PB2
#define  PORT_SUSPEND   0x0001
#define  PORT_SUSPEND_BAR   0x0002
#define  PORT_DTR2      0x0004
#define  PORT_DSR2      0x0008

// PB3
#define  PORT_TX2       0x0001
#define  PORT_RX2       0x0002
#define  PORT_RTS2      0x0004
#define  PORT_CTS2      0x0008
#define  PORT_DCD2      0x0010
#define  PORT_RI2       0x0020
#define  PORT_DTR3      0x0040
#define  PORT_DSR3      0x0080
#define  PORT_DCD3      0x0100
#define  PORT_RI3       0x0200

// PB4
#define  PORT_RTS3      0x0001
#define  PORT_CTS3      0x0002
#define  PORT_TX3       0x0004
#define  PORT_RX3       0x0008

//
// CP2108 Define bit locations for EnhancedFxn_IFCx
#define EF_IFC_GPIO_TXLED   0x01
#define EF_IFC_GPIO_RXLED   0x02
#define EF_IFC_GPIO_RS485   0x04
// If the next bit is clear, GPIO1 is low while sending UART data.
// If it is set, GPIO1 is high while sending UART data, and low otherwise
#define EF_IFC_GPIO_RS485_LOGIC 0x08
#define EF_IFC_GPIO_CLOCK       0x10
#define EF_IFC_DYNAMIC_SUSPEND  0x40

//
// CP2108 Define bit locations for EnhancedFxn_Device
#define EF_DEVICE_WEAKPULLUP_RESET      0x10
#define EF_DEVICE_WEAKPULLUP_SUSPEND    0x20
#define EF_DEVICE_DYNAMIC_SUSPEND       0x40
//
typedef unsigned char uint8_t;
// Firmware version structure.  Not writeable by OEMs or end-users.
// Automatically populated by build infrastructure
typedef struct {
	uint8_t major;
	uint8_t minor;
	uint8_t build;
} firmware_t, *pFirmware_t;

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Determines the number of CP210x devices connected to the system
/// @param lpdwNumDevices a pointer to a DWORD/4-byte location to hold the returned device count
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_PARAMETER -- lpdwNumDevices is an unexpected value
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS WINAPI CP210x_GetNumDevices(
	_Out_writes_bytes_(sizeof(DWORD)) _Pre_defensive_ LPDWORD lpdwNumDevices
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS WINAPI CP210x_GetProductString(
	_In_ _Pre_defensive_ const DWORD dwDeviceNum,
	LPVOID lpvDeviceString,
	_In_ _Pre_defensive_ const DWORD dwFlags
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS WINAPI CP210x_Open(
	_In_ _Pre_defensive_ DWORD dwDevice,
	HANDLE*	cyHandle
	); 

/// @brief Closes an open handle to the device
/// @param cyHandle is an open handle to the device
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS WINAPI CP210x_Close(
	_In_ _Pre_defensive_ const HANDLE cyHandle
	);

/// @brief Reads and returns the Part Number from the device
/// @param cyHandle is an open handle to the device
/// @param lpbPartNum points at a buffer into which the Part Number value will be written
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- lpbPartNum is an unexpected value
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS WINAPI CP210x_GetPartNumber(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ LPBYTE lpbPartNum
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI 
CP210x_SetVid(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_ _Pre_defensive_ const WORD	wVid
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI 
CP210x_SetPid(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_ _Pre_defensive_ const WORD	wPid
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI 
CP210x_SetManufacturerString(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_ _Pre_defensive_ LPVOID lpvManufacturer,
	_In_ _Pre_defensive_ const BYTE	bLength,
	_In_ _Pre_defensive_ const BOOL bConvertToUnicode = TRUE
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI 
CP210x_SetProductString(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_ _Pre_defensive_ LPVOID lpvProduct,
	_In_ _Pre_defensive_ const BYTE bLength,
	_In_ _Pre_defensive_ const BOOL bConvertToUnicode = TRUE
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI 
CP210x_SetInterfaceString(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_ _Pre_defensive_ const BYTE bInterfaceNumber,
	_In_ _Pre_defensive_ LPVOID lpvInterface,
	_In_ _Pre_defensive_ const BYTE bLength,
	_In_ _Pre_defensive_ const BOOL bConvertToUnicode
	);

/// @brief Reprogram the Serial Number String in the device's USB Serial Number String Descriptor
/// @param cyHandle is an open handle to the device
/// @param lpvSerialNumber points at a buffer containing the Serial Number String to be reprogrammed to the device
/// @param bLengthInChars is the total number of characters in the Serial Number String
/// @param bConvertToUnicode is a true or false value indicating whether the Serial Number String is an 8-bit character ascii string needing converting to 16-bit UCS-2 characters (true) to not (false).
/// @note OTP parts can not often/indefinitely have configuration data (re-)written to them
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- lpvSerialNumber or bLengthInChars are unexpected values
///			CP210x_FUNCTION_NOT_SUPPORTED -- the device does not support setting of the Serial Number string
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI 
CP210x_SetSerialNumber(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_ _Pre_defensive_ LPVOID	lpvSerialNumber,
	_In_ _Pre_defensive_ const BYTE bLengthInChars,
	_In_ _Pre_defensive_ const BOOL bConvertToUnicode = TRUE
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI 
CP210x_SetSelfPower(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_ _Pre_defensive_ const BOOL bSelfPower
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI 
CP210x_SetMaxPower(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_ _Pre_defensive_ const BYTE bMaxPower
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI 
CP210x_SetFlushBufferConfig(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_ _Pre_defensive_ const WORD wFlushBufferConfig
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI 
CP210x_SetDeviceMode(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_ _Pre_defensive_ const BYTE bDeviceModeECI,
	_In_ _Pre_defensive_ const BYTE bDeviceModeSCI
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI 
CP210x_SetDeviceVersion(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_ _Pre_defensive_ const WORD wVersion
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS 
WINAPI 
CP210x_SetBaudRateConfig(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_reads_bytes_(sizeof(BAUD_CONFIG)) _Pre_defensive_ PBAUD_CONFIG baudConfigData
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS 
WINAPI 
CP210x_SetPortConfig(	
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_ _Pre_defensive_ PORT_CONFIG*	PortConfig
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS 
WINAPI 
CP210x_SetDualPortConfig(	
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_ _Pre_defensive_ DUAL_PORT_CONFIG*	DualPortConfig
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS 
WINAPI 
CP210x_SetQuadPortConfig(	
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_ _Pre_defensive_ QUAD_PORT_CONFIG*	QuadPortConfig
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS 
WINAPI 
CP210x_SetLockValue(
	_In_ _Pre_defensive_ const HANDLE cyHandle
	);

/// @brief Reads and returns the VID from the device
/// @param cyHandle is an open handle to the device
/// @param lpwVid points at a buffer into which the VID will be written
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- lpwVid is an unexpected value
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI
CP210x_GetDeviceVid(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ LPWORD lpwVid
	);

/// @brief Reads and returns the PID from the device
/// @param cyHandle is an open handle to the device
/// @param lpwPid points at a buffer into which the PID will be written
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- lpwPid is an unexpected value
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI
CP210x_GetDevicePid(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ LPWORD lpwPid
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI
CP210x_GetDeviceManufacturerString(	
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	LPVOID	lpManufacturer,
	LPBYTE	lpbLength,
	_In_ const BOOL	bConvertToASCII = TRUE
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI
CP210x_GetDeviceProductString(	
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	LPVOID	lpProduct,
	LPBYTE	lpbLength,
	_In_ _Pre_defensive_ const BOOL	bConvertToASCII = TRUE
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI
CP210x_GetDeviceInterfaceString(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_ _Pre_defensive_ const BYTE bInterfaceNumber,
	LPVOID lpInterface,
	LPBYTE lpbLength,
	_In_ _Pre_defensive_ const BOOL bConvertToASCII
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI
CP210x_GetDeviceSerialNumber(	
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	LPVOID	lpSerialNumber,
	LPBYTE	lpbLength,
	_In_ _Pre_defensive_ const BOOL	bConvertToASCII = TRUE
	);

/// @brief Reads and returns the Power attributes from the device
/// @param cyHandle is an open handle to the device
/// @param lpbSelfPower points to a buffer into which the Power attributes will be written
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- lpbSelfPower is an unexpected value
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI
CP210x_GetSelfPower(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_Out_writes_bytes_(sizeof(BOOL)) _Pre_defensive_ LPBOOL lpbSelfPower
	);

/// @brief Reads and returns the Maximum power from the device
/// @param cyHandle is an open handle to the device
/// @param lpbPower points to a buffer into which the Maximum power will be written
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- lpbPower is an unexpected value
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI
CP210x_GetMaxPower(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ LPBYTE lpbPower
	);

/// @brief Reads and returns the flush buffer behavior from the device
/// @param cyHandle is an open handle to the device
/// @param lpwFlushBufferConfig points to a buffer into which the flush buffer behavior will be written
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- lpwFlushBufferConfig is an unexpected value
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI 
CP210x_GetFlushBufferConfig(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ LPWORD lpwFlushBufferConfig
	);

/// @brief Reads and returns the device mode from the device
/// @param cyHandle is an open handle to the device
/// @param lpbDeviceModeECI points to a buffer into which the ECI will be written
/// @param lpbDeviceModeSCI points to a buffer into which the SCI will be written
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- lpbDeviceModeECI or lpbDeviceModeSCI are unexpected values
///			CP210x_FUNCTION_NOT_SUPPORTED -- the device does not support device mode
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI 
CP210x_GetDeviceMode(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ LPBYTE lpbDeviceModeECI,
	_Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ LPBYTE lpbDeviceModeSCI
	);

/// @brief Reads and returns the Version (Device from the USB Standard Device Descriptor) from the device
/// @param cyHandle is an open handle to the device
/// @param lpwVersion points to a buffer into which the Version will be written
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- lpwVersion is an unexpected value
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI
CP210x_GetDeviceVersion(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ LPWORD lpwVersion
	);

/// @brief Reads and returns the Baud Rate Configuration from the device
/// @param cyHandle is an open handle to the device
/// @param pBaudConfig points to a buffer into which the Baud Rate Configuration will be written
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- pBaudConfig is an unexpected value
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS 
WINAPI 
CP210x_GetBaudRateConfig(
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_Out_writes_bytes_(sizeof(BAUD_CONFIG)) _Pre_defensive_ PBAUD_CONFIG pBaudConfig
	);

/// @brief Reads and returns the Port Configuration from the device
/// @param cyHandle is an open handle to the device
/// @param pPortConfig points to a buffer into which the Port Configuration will be written
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- pPortConfig is an unexpected value
///			CP210x_FUNCTION_NOT_SUPPORTED -- the device does not support Port Configuration
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS 
WINAPI 
CP210x_GetPortConfig(	
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_Out_writes_bytes_(sizeof(PORT_CONFIG)) _Pre_defensive_ PPORT_CONFIG pPortConfig
	);

/// @brief Reads and returns the Dual Port Configuration from the Dual Port device
/// @param cyHandle is an open handle to the device
/// @param pDualPortConfig points to a buffer into which the Dual Port Configuration will be written
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- pDualPortConfig is an unexpected value
///			CP210x_FUNCTION_NOT_SUPPORTED -- the device does not support Dual Port Configuration
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS 
WINAPI 
CP210x_GetDualPortConfig(	
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_Out_writes_bytes_(sizeof(DUAL_PORT_CONFIG)) _Pre_defensive_ DUAL_PORT_CONFIG* pDualPortConfig
	);

/// @brief Reads and returns the Quad Port Configuration from the Quad Port device
/// @param cyHandle is an open handle to the device
/// @param pQuadPortConfig points to a buffer into which the Quad Port Configuration will be written
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- pQuadPortConfig is an unexpected value
///			CP210x_FUNCTION_NOT_SUPPORTED -- the device does not support Quad Port Configuration
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS 
WINAPI 
CP210x_GetQuadPortConfig(	
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_Out_writes_bytes_(sizeof(QUAD_PORT_CONFIG)) _Pre_defensive_ QUAD_PORT_CONFIG* pQuadPortConfig
	);

/// @brief Reads and returns the Lock Value from the  device
/// @param cyHandle is an open handle to the device
/// @param lpbLockValue points to a buffer into which the Quad Port Configuration will be written
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- lpbLockValue is an unexpected value
///			CP210x_FUNCTION_NOT_SUPPORTED -- the device does not support Lock Value
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS 
WINAPI 
CP210x_GetLockValue(	
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_Out_writes_bytes_(sizeof(BYTE)) _Pre_defensive_ LPBYTE lpbLockValue
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS	
WINAPI
CP210x_Reset(
	_In_ _Pre_defensive_ const HANDLE cyHandle
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS 
WINAPI 
CP210x_CreateHexFile(	
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_ LPCSTR lpvFileName
	);

/// @brief Reads and returns the Firmware Version from the  device
/// @param cyHandle is an open handle to the device
/// @param lpVersion points to a buffer into which the Firmware Version will be written
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- lpVersion is an unexpected value
///			CP210x_FUNCTION_NOT_SUPPORTED -- the device does not support reporting its Firmware Version
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS 
WINAPI 
CP210x_GetFirmwareVersion(	
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_Out_writes_bytes_(sizeof(firmware_t)) _Pre_defensive_ pFirmware_t lpVersion
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS 
WINAPI 
CP210x_GetConfig(	
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_Out_writes_bytes_(bLength) _Pre_defensive_ LPBYTE lpbConfig,
	_In_ _Pre_defensive_ const WORD	bLength
);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS 
WINAPI 
CP210x_SetConfig(	
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_reads_bytes_(bLength) _Pre_defensive_ LPBYTE lpbConfig,
	_In_ _Pre_defensive_ const WORD	bLength
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS 
WINAPI 
CP210x_UpdateFirmware(	
	_In_ _Pre_defensive_ const HANDLE cyHandle);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS 
WINAPI 
CP210x_GetGeneric(	
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_Out_writes_bytes_(bLength) _Pre_defensive_ LPBYTE lpbGeneric,
	_In_ _Pre_defensive_ const WORD	bLength
	);

_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xDLL_API
CP210x_STATUS 
WINAPI 
CP210x_SetGeneric(	
	_In_ _Pre_defensive_ const HANDLE cyHandle,
	_In_reads_bytes_(bLength) _Pre_defensive_ LPBYTE lpbGeneric,
	_In_ _Pre_defensive_ const WORD bLength
	);

#ifdef __cplusplus
}
#endif

#endif	// !HOST_APPLICATIONS_DLLS_MANUFACTURINGDLL_CP210XMANUFACTURINGDLL_H_INCLUDED_XF3N9QM3BK
