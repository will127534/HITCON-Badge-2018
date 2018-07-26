// TODO: Header guards? Or is that what _CP210x_STANDARD_DEF_ is doing? If so, that same token is used in CP210xRuntimeDLL.h, is that by design, or error?
#ifndef	HOST_APPLICATIONS_DLLS_RUNTIMEDLL_CP210XRUNTIMEDLL_H_INCLUDED_QU4N7WC20W
#define	HOST_APPLICATIONS_DLLS_RUNTIMEDLL_CP210XRUNTIMEDLL_H_INCLUDED_QU4N7WC20W

// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the CP210xRUNTIMEDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// CP210xRUNTIMEDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef CP210xRUNTIMEDLL_EXPORTS
#define CP210xRUNTIMEDLL_API __declspec(dllexport)
#else
#define CP210xRUNTIMEDLL_API __declspec(dllimport)
#pragma comment (lib, "CP210xRuntime.lib")
#endif

#if ! defined(_PREFAST_)
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
#if ! defined(_Pre_defensive_)
#define	_Pre_defensive_
#endif // ! defined(_Pre_defensive_)
#endif // ! defined(_PREFAST_)

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
#define     CP210x_SUCCESS                      0x00
#define     CP210x_DEVICE_NOT_FOUND             0xFF
#define     CP210x_INVALID_HANDLE               0x01
#define     CP210x_INVALID_PARAMETER            0x02
#define     CP210x_DEVICE_IO_FAILED             0x03
#define     CP210x_FUNCTION_NOT_SUPPORTED       0x04
#define     CP210x_GLOBAL_DATA_ERROR            0x05
#define     CP210x_FILE_ERROR                   0x06
#define     CP210x_COMMAND_FAILED               0x08
#define     CP210x_INVALID_ACCESS_TYPE          0x09

// Type definitions
typedef     int     CP210x_STATUS;

// Buffer size limits
#define     CP210x_MAX_PRODUCT_STRLEN           126
#define     CP210x_MAX_SERIAL_STRLEN            63

// Type definitions
typedef     char    CP210x_PRODUCT_STRING[CP210x_MAX_PRODUCT_STRLEN];
typedef     char    CP210x_SERIAL_STRING[CP210x_MAX_SERIAL_STRLEN];

// Mask and Latch value bit definitions
#define     CP210x_GPIO_0                       0x0001
#define     CP210x_GPIO_1                       0x0002
#define     CP210x_GPIO_2                       0x0004
#define     CP210x_GPIO_3                       0x0008
#define     CP210x_GPIO_4                       0x0010
#define     CP210x_GPIO_5                       0x0020
#define     CP210x_GPIO_6                       0x0040
#define     CP210x_GPIO_7                       0x0080
#define     CP210x_GPIO_8                       0x0100
#define     CP210x_GPIO_9                       0x0200
#define     CP210x_GPIO_10                      0x0400
#define     CP210x_GPIO_11                      0x0800
#define     CP210x_GPIO_12                      0x1000
#define     CP210x_GPIO_13                      0x2000
#define     CP210x_GPIO_14                      0x4000
#define     CP210x_GPIO_15                      0x8000

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/// @brief Reads and returns the current port latch value from the device
/// @param cyHandle is an open handle to the device
/// @param lpwLatch points at a buffer into which the port latch value will be written
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- lpwLatch is an unexpected value
///			CP210x_DEVICE_NOT_FOUND -- the device could not be determined to be a CP210x device
///			CP210x_FUNCTION_NOT_SUPPORTED -- the device does not support port latch
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xRUNTIMEDLL_API CP210x_STATUS WINAPI
CP210xRT_ReadLatch(_In_ _Pre_defensive_ const HANDLE cyHandle,
				   _Out_writes_bytes_(sizeof(WORD)) _Pre_defensive_ LPWORD lpwLatch);

/// @brief Writes a new port latch value to the device
/// @param cyHandle is an open handle to the device
/// @param mask the TBD
/// @param latch is the new port latch value to be written to the device
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_DEVICE_NOT_FOUND -- the device could not be determined to be a CP210x device
///			CP210x_FUNCTION_NOT_SUPPORTED -- the device does not support port latch
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xRUNTIMEDLL_API CP210x_STATUS WINAPI
CP210xRT_WriteLatch(_In_ _Pre_defensive_ const HANDLE cyHandle,
					_In_ _Pre_defensive_ const WORD mask,
					_In_ _Pre_defensive_ const WORD latch);

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
CP210xRUNTIMEDLL_API CP210x_STATUS WINAPI
CP210xRT_GetPartNumber(_In_ _Pre_defensive_ const HANDLE cyHandle,
					   _Out_writes_bytes_(1) _Pre_defensive_ LPBYTE lpbPartNum);

/// @brief Reads and returns the Product String directly from the device
/// @param cyHandle is an open handle to the device
/// @param lpProductString points at a buffer into which the Product String will be written
/// @param lpbProductStringLengthInBytes is the total number of characters in the Product String
/// @param bConvertToASCII is a true or false value indicating whether the Product String is an 16-bit UCS-2 character string needing converting to 8-bit ascii characters (true) to not (false).
/// @note Length of lpProductString must be <= CP210x_MAX_SETUP_STRING_SIZE
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- lpProductString or lpbProductStringLengthInBytes are unexpected values
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xRUNTIMEDLL_API CP210x_STATUS WINAPI
CP210xRT_GetDeviceProductString(_In_ _Pre_defensive_ const HANDLE cyHandle,
								_Out_writes_bytes_(*lpbProductStringLengthInBytes) _Pre_defensive_ LPVOID lpProductString,
								_Out_writes_bytes_(1) _Pre_defensive_ LPBYTE lpbProductStringLengthInBytes,
								_In_ _Pre_defensive_ const BOOL bConvertToASCII = TRUE
                                );

/// @brief Reads and returns the Serial Number String directly from the device
/// @param cyHandle is an open handle to the device
/// @param lpSerialNumberString points at a buffer into which the Serial Number String will be written
/// @param lpbSerialNumberStringLengthInBytes is the total number of characters in the Serial Number String
/// @param bConvertToASCII is a true or false value indicating whether the Serial Number String is an 16-bit UCS-2 character string needing converting to 8-bit ascii characters (true) to not (false).
/// @note Length of lpSerialNumberString must be <= CP210x_MAX_SETUP_STRING_SIZE
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- lpSerialNumberString or lpbSerialNumberStringLengthInBytes are unexpected values
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xRUNTIMEDLL_API CP210x_STATUS WINAPI
CP210xRT_GetDeviceSerialNumber(_In_ _Pre_defensive_ const HANDLE cyHandle,
							   _Out_writes_bytes_(*lpbSerialNumberStringLengthInBytes) _Pre_defensive_ LPVOID lpSerialNumberString,
							   _Out_writes_bytes_(1) _Pre_defensive_ LPBYTE lpbSerialNumberStringLengthInBytes,
							   _In_ _Pre_defensive_ const BOOL bConvertToASCII = TRUE
                               );

/// @brief Reads and returns the Interface String directly from the device
/// @param cyHandle is an open handle to the device
/// @param lpInterfaceString points at a buffer into which the Interface String will be written
/// @param lpbInterfaceStringLengthInBytes is the total number of characters in the Interface String
/// @param bConvertToASCII is a true or false value indicating whether the Interface String is an 16-bit UCS-2 character string needing converting to 8-bit ascii characters (true) to not (false).
/// @note Length of lpInterfaceString must be <= CP210x_MAX_SETUP_STRING_SIZE
/// @returns Returns CP210x_SUCCESS on success, another CP210x_STATUS if there is an error:
///			CP210x_INVALID_HANDLE -- cyHandle is invalid
///			CP210x_INVALID_PARAMETER -- lpInterfaceString or lpbInterfaceStringLengthInBytes are unexpected values
///			CP210x_DEVICE_IO_FAILED -- the device failed to respond to I/O in any expected manner
_Check_return_
_Ret_range_(CP210x_SUCCESS, CP210x_DEVICE_NOT_FOUND)
_Success_(return == CP210x_SUCCESS)
CP210xRUNTIMEDLL_API CP210x_STATUS WINAPI
CP210xRT_GetDeviceInterfaceString(_In_ _Pre_defensive_ const HANDLE	cyHandle,
								  _Out_writes_bytes_(*lpbInterfaceStringLengthInBytes) _Pre_defensive_ LPVOID lpInterfaceString,
								  _Out_writes_bytes_(1) _Pre_defensive_ LPBYTE lpbInterfaceStringLengthInBytes,
								  _In_ _Pre_defensive_ const BOOL bConvertToASCII);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif	// !HOST_APPLICATIONS_DLLS_RUNTIMEDLL_CP210XRUNTIMEDLL_H_INCLUDED_QU4N7WC20W


