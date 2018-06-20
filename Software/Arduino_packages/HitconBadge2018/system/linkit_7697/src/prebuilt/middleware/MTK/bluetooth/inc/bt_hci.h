/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __BT_HCI_H__
#define __BT_HCI_H__

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothCommon Common
 * @{
 * @addtogroup BluetoothCommonHCI HCI
 * @{
 * This section introduces the host controller interface API, including controller error code, command structures
 * and command type codes used by the GAP module. For more information, please refer to \ref GAP.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b ACL                        | Asynchronous Connection-Oriented Logical Transport. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Asynchronous_Connection-Less">Wikipedia</a>. |
 * |\b HCI                        | Host Controller Interface. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#HCI">Wikipedia</a>. |
 * |\b LE                         | Low Energy. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Bluetooth_low_energy">Wikipedia</a>. |
 * |\b LMP                        | Link Manager Protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#LMP_.28Link_Management_Protocol.29">Wikipedia</a>. |
 * |\b MIC                        | Message Integrity Check. It is used to authenticate the data PDU. |
 * |\b PDU                        | Protocol Data Unit. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Protocol_data_unit">Wikipedia</a>. |
 * |\b QoS                        | Quality of Service. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Quality_of_service">Wikipedia</a>. |
 * |\b SCO                        | Synchronous Connection Oriented. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#Synchronous_connection_oriented_.28SCO.29_link">Wikipedia</a>. |
 *
 * @section bt_hci_spec_api_usage How to use this module
 *
 * 1. Call the GAP APIs to set the HCI structures as parameters.
 *
 * 2. Call #bt_app_event_callback() to parse the payload of the confirmation event with the HCI confirmation structure.
 *
 * For more information on GAP APIs applied in the example implementation, please refer to \ref GAP.
 *
 * - Request example
 *      @code
 *          // Declare the HCI read RSSI structure and call bt_gap_le_read_rssi() to read the RSSI.
 *          bt_hci_cmd_read_rssi_t read_rssi = {
 *              .handle = 0x0200,
 *          };
 *
 *          bt_gap_le_read_rssi(&read_rssi);
 *      @endcode
 *
 * - Confirmation Example
 *      @code
 *          // Use the HCI read RSSI confirmation structure to parse the payload of the read RSSI confirmation event in the function bt_app_event_callback().
 *          bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *          {
 *              switch(msg)
 *              {
 *                  case BT_GAP_LE_READ_RSSI_CNF:
 *                      const bt_hci_evt_cc_read_rssi_t *rssi = (bt_hci_evt_cc_read_rssi_t *)buff;
 *                      BT_LOGI("APP", "connection handle=0x%04x", rssi->handle);
 *                      if (rssi->rssi == 127) {
 *                          BT_LOGI("APP", "The RSSI cannot be read");
 *                      } else {
 *                          if ((rssi->rssi>>7)>0){
 *                              BT_LOGI("APP", "rssi=%ddBm", ((~rssi->rssi)&0xFF)+0x01);
 *                          } else {
 *                              BT_LOGI("APP", "rssi=%ddBm", rssi->rssi);
 *                          }
 *                      }
 *                      break;
 *                  // Add other cases if needed.
 *              }
 *          }
 *      @endcode
 *
 */

#include "bt_type.h"

/**
 * @defgroup Bluetoothhbif_hci_le_define Define
 * @{
 * Define HCI data types and values.
 */
/**
 * @brief HCI error codes, please refer to the <a href="https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439&_ga=1.241778370.1195131350.1437993589">Bluetooth core specification version 4.2 [VOL 2, part D]</a>.
 */
#define BT_HCI_STATUS_SUCCESS                                0x00   /**< The operation completed successfully. */
#define BT_HCI_STATUS_UNKNOWN_HCI_COMMAND                    0x01   /**< The controller did not recognize the HCI command sent by the host. */
#define BT_HCI_STATUS_UNKNOWN_CONNECTION_IDENTIFIER          0x02   /**< Unknown connection ID. */
#define BT_HCI_STATUS_HARDWARE_FAILURE                       0x03   /**< Hardware failure. */
#define BT_HCI_STATUS_PAGE_TIMEOUT                           0x04   /**< Page timeout. */
#define BT_HCI_STATUS_AUTHENTICATION_FAILURE                 0x05   /**< Authentication failure. */
#define BT_HCI_STATUS_PIN_OR_KEY_MISSING                     0x06   /**< PIN code missing. */
#define BT_HCI_STATUS_MEMORY_CAPACITY_EXCEEDED               0x07   /**< Memory capacity exceeded. */
#define BT_HCI_STATUS_CONNECTION_TIMEOUT                     0x08   /**< Connection timeout. */
#define BT_HCI_STATUS_CONNECTION_LIMIT_EXCEEDED              0x09   /**< Hardware connection limit has reached. */
#define BT_HCI_STATUS_SYNCHRONOUS_CONNECTION_LIMIT_TO_A_DEVICE_EXCEEDED 0x0A   /**< Synchronous connection limit has reached. */
#define BT_HCI_STATUS_ACL_CONNECTION_ALREADY_EXISTS          0x0B   /**< The connection already exists. */
#define BT_HCI_STATUS_COMMAND_DISALLOWED                     0x0C   /**< Command disallowed. */
#define BT_HCI_STATUS_CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES      0x0D   /**< Rejected due to a lack of resources. */
#define BT_HCI_STATUS_CONNECTION_REJECTED_DUE_TO_SECURITY_REASONS       0x0E   /**< Rejected due to security. */
#define BT_HCI_STATUS_CONNECTION_REJECTED_DUE_TO_UNACCEPTABLE_BDADDR    0x0F   /**< Rejected due to unacceptable device address. */
#define BT_HCI_STATUS_CONNECTION_ACCEPT_TIMEOUT_EXCEEDED     0x10   /**< Connection attempt timeout. */
#define BT_HCI_STATUS_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE 0x11   /**< Unsupported feature or parameter value. */
#define BT_HCI_STATUS_INVALID_HCI_COMMAND_PARAMETERS         0x12   /**< Invalid command parameters. */
#define BT_HCI_STATUS_REMOTE_USER_TERMINATED_CONNECTION      0x13   /**< Remote user terminated the connection. */
#define BT_HCI_STATUS_REMOTE_TERMINATED_CONNECTION_DUE_TO_LOW_RESOURCES 0x14   /**< Connection terminated due to a lack of resources. */
#define BT_HCI_STATUS_REMOTE_TERMINATED_CONNECTION_DUE_TO_POWER_OFF     0x15   /**< Connection terminated due to power off. */
#define BT_HCI_STATUS_CONNECTION_TERMINATED_BY_LOCAL_HOST    0x16   /**< The local device terminated the connection. */
#define BT_HCI_STATUS_REPEATED_ATTEMPTS                      0x17   /**< The controller forbids an authentication or pairing procedure because too little
                                                                         time has elapsed since the last failed attempt for authentication or pairing. */
#define BT_HCI_STATUS_PAIRING_NOT_ALLOWED                    0x18   /**< Pairing is not allowed. */
#define BT_HCI_STATUS_UNKNOWN_LMP_PDU                        0x19   /**< Unknown LMP PDU. */
#define BT_HCI_STATUS_UNSUPPORTED_REMOTE_FEATURE_OR_LMP_FEATURE         0x1A   /**< Unsupported remote feature. */
#define BT_HCI_STATUS_SCO_OFFSET_REJECTED                    0x1B   /**< The SCO offset is rejected. */
#define BT_HCI_STATUS_SCO_INTERVAL_REJECTED                  0x1C   /**< The SCO interval is rejected. */
#define BT_HCI_STATUS_SCO_AIR_MODE_REJECTED                  0x1D   /**< The SCO air mode is rejected. */
#define BT_HCI_STATUS_INVALID_LMP_PARAMETERS_OR_LL_PARAMETERS           0x1E   /**< Invalid LMP PDU/LL Control PDU parameters. */
#define BT_HCI_STATUS_UNSPECIFIED_ERROR                      0x1F   /**< Unspecified error. */
#define BT_HCI_STATUS_UPSUPPORTED_LMP_OR_LL_PARAMETER_VALUE  0x20   /**< Unsupported parameters. */
#define BT_HCI_STATUS_ROLE_CHANGE_NOT_ALLOWED                0x21   /**< Role changing is not allowed. */
#define BT_HCI_STATUS_LMP_RESPONSE_TIMEOUT_OR_LL_RESPONSE_TIMEOUT       0x22   /**< An LMP transaction failed to respond within the LMP response timeout. */
#define BT_HCI_STATUS_LMP_ERROR_TRANSACTION_COLLISION        0x23   /**< An LMP transaction has collided with the same transaction that is already in progress. */
#define BT_HCI_STATUS_LMP_PDU_NOT_ALLOWED                    0x24   /**< LMP PDU is not allowed. */
#define BT_HCI_STATUS_ENCRYPTION_MODE_NOT_ACCEPTABLE         0x25   /**< Encryption mode is not acceptable. */
#define BT_HCI_STATUS_LINK_KEY_CANNOT_BE_CHANGED             0x26   /**< Link key cannot be changed because a fixed unit key is being used. */
#define BT_HCI_STATUS_REQUESTED_QOS_NOT_SUPPORTED            0x27   /**< The requested QoS is not supported. */
#define BT_HCI_STATUS_INSTANT_PASSED                         0x28   /**< An LMP PDU or LL PDU that includes an instant cannot be performed because the instant when this would have occurred has passed. */
#define BT_HCI_STATUS_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED    0x29   /**< Impossible to establish pairing as the requested unit key is not supported. */
#define BT_HCI_STATUS_DIFFERENT_TRANSACTION_COLLISION        0x2A   /**< An initiated LMP transaction collided with an ongoing transaction. */
#define BT_HCI_STATUS_QOS_UNACCEPTABLE_PARAMETER             0x2C   /**< The specified QoS parameters could not be accepted. */
#define BT_HCI_STATUS_QOS_REJECTED                           0x2D   /**< The specified QoS parameters cannot be accepted and QoS negotiation should be terminated. */
#define BT_HCI_STATUS_CHANNEL_ASSESSMENT_NOT_SUPPORTED       0x2E   /**< Channel assessment is not supported. */
#define BT_HCI_STATUS_INSUFFICIENT_SECURITY                  0x2F   /**< Sending the HCI command or LMP PDU is only possible on an encrypted link. */
#define BT_HCI_STATUS_PARAMETER_OUT_OF_MANADATORY_RANGE      0x30   /**< The parameter is out of mandatory range. */
#define BT_HCI_STATUS_ROLE_SWITCH_PENDING                    0x32   /**< Role switching is pending. */
#define BT_HCI_STATUS_RESERVED_SLOT_VIOLATION                0x34   /**< Reserved slot violation error. */
#define BT_HCI_STATUS_ROLE_SWITCH_FAILED                     0x35   /**< Role switching failed. */
#define BT_HCI_STATUS_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE    0x36   /**< Extended inquiry response is too large. */
#define BT_HCI_STATUS_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST   0x37   /**< Simple pairing is not supported by the host. */
#define BT_HCI_STATUS_HOST_BUSY_PAIRING                      0x38   /**< The host is busy for pairing. */
#define BT_HCI_STATUS_CONNECTION_REJECTED_DUE_TO_NOT_SUITABLE_CHANNEL_FOUND 0x39   /**< No suitable channel found, the connection is rejected. */
#define BT_HCI_STATUS_CONTROLLER_BUSY                        0x3A   /**< The controller is busy. */
#define BT_HCI_STATUS_UNACCEPTABLE_CONNECTION_PARAMETERS     0x3B   /**< Unacceptable connection parameters. */
#define BT_HCI_STATUS_DIRECTED_ADVERTISING_TIMEOUT           0x3C /**< Directed advertising completed without a connection being created. */
#define BT_HCI_STATUS_CONNECTION_TERMINATED_DUE_TO_MIC_FAILURE          0x3D   /**< Connection terminated due to MIC failure. */
#define BT_HCI_STATUS_CONNECTION_FAILED_TO_BE_ESTABLISHED    0x3E   /**< The connection failed to establish. */
#define BT_HCI_STATUS_MAC_CONNECTION_FAILED                  0x3F   /**< MAC connection failed. */
#define BT_HCI_STATUS_COARSE_CLOCK_ADJUSTMENT_REJECTED       0x40    /**< The coarse clock adjustment is rejected. */

/* Proprietary. */
#define BT_HCI_STATUS_CHIP_LIMITATION                        0xF1 /**< Hardware limitation. */
#define BT_HCI_STATUS_FORCED_TERMINATE                       0xF4 /**< The connection is terminated immediately without flushing the TX buffer. */
typedef uint8_t bt_hci_status_t;                                  /**< Defines the status of the controller events. Please refer to BT_HCI_STATUS_XXX. */
typedef bt_hci_status_t bt_hci_disconnect_reason_t;               /**< The reason to disconnect. */

/* Common definitions. */
#define BT_HCI_DISABLE                  0x00 /**< Disable. */
#define BT_HCI_ENABLE                   0x01 /**< Enable. */
typedef uint8_t bt_hci_enable_t;             /**< HCI boolean type. */

#define BT_HCI_REJECT                   0x00 /**< Reject. */
#define BT_HCI_ACCEPT                   0x01 /**< Accept. */
typedef uint16_t bt_hci_accept_t;            /**< Accept types. */

#define BT_HCI_SCAN_TYPE_PASSIVE        0x00 /**< Passive scan. */
#define BT_HCI_SCAN_TYPE_ACTIVE         0x01 /**< Active scan. */
typedef uint8_t bt_hci_scan_type_t;          /**< Scan type. For more information, please refer to the <a href="https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439&_ga=1.241778370.1195131350.1437993589">Bluetooth core specification version 4.2 [VOL 2, part E] Section 7.8.10</a>. */

#define BT_HCI_SCAN_ADDR_PUBLIC         0x00                        /**< Scan using a public address type. */
#define BT_HCI_SCAN_ADDR_RANDOM         0x01                        /**< Scan using a random address type. */
#define BT_HCI_SCAN_ADDR_RESOLVE_PRIVATE_WITH_PUBLIC        0x02    /**< Scan using a public identity address type. */
#define BT_HCI_SCAN_ADDR_RESOLVE_PRIVATE_WITH_RANDOM        0x03    /**< Scan using a random identity address type. */
typedef uint8_t bt_hci_scan_addr_type_t;                            /**< Scan address type. For more information, please refer to the <a href="https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439&_ga=1.241778370.1195131350.1437993589">Bluetooth core specification version 4.2 [VOL 2, part E] Section 7.8.10</a>. */

#define BT_HCI_SCAN_FILTER_ACCEPT_ALL_ADVERTISING_PACKETS                   0x00   /**< No scanning filter. */
#define BT_HCI_SCAN_FILTER_ACCEPT_ONLY_ADVERTISING_PACKETS_IN_WHITE_LIST    0x01   /**< Scan using the white list filter. */
typedef uint8_t bt_hci_scan_filter_type_t;                                         /**< Scan filter type. For more information, please refer to the <a href="https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439&_ga=1.241778370.1195131350.1437993589">Bluetooth core specification version 4.2 [VOL 2, part E] Section 7.8.10</a>. */

#define BT_HCI_CONN_FILTER_ASSIGNED_ADDRESS                 0x00   /**< Direct connection. */
#define BT_HCI_CONN_FILTER_WHITE_LIST_ONLY                  0x01   /**< Auto connection. */
typedef uint8_t bt_hci_conn_filter_type_t;                         /**< Connection filter type. For more information, please refer to the <a href="https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439&_ga=1.241778370.1195131350.1437993589">Bluetooth core specification version 4.2 [VOL 2, part E] Section 7.8.12</a>. */

#define BT_HCI_ADV_TYPE_CONNECTABLE_UNDIRECTED              0x00  /**< Connectable undirected advertising. */
#define BT_HCI_ADV_TYPE_CONNECTABLE_DIRECTED_HIGH           0x01  /**< Connectable high duty cycle directed advertising. */
#define BT_HCI_ADV_TYPE_SCANNABLE_UNDIRECTED                0x02  /**< Scannable undirected advertising. */
#define BT_HCI_ADV_TYPE_NON_CONNECTABLE_UNDIRECTED          0x03  /**< Non connectable undirected advertising. */
#define BT_HCI_ADV_TYPE_CONNECTABLE_DIRECTED_LOW            0x04  /**< Connectable low duty cycle directed advertising. */
typedef uint8_t bt_hci_advertising_type_t;                        /**< Advertising type. For more information, please refer to the <a href="https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439&_ga=1.241778370.1195131350.1437993589">Bluetooth core specification version 4.2 [VOL 2, part E] Section 7.8.5</a>. */

#define BT_GAP_LE_ADD_TO_RESOLVING_LIST                     0x2027  /**< Operation type: add a device to the resolving list with the #bt_hci_cmd_le_add_device_to_resolving_list_t pointer type parameters. */
#define BT_GAP_LE_REMOVE_FROM_RESOLVING_LIST                0x2028  /**< Operation type: remove a device from the resolving list with the #bt_hci_cmd_le_remove_device_from_resolving_list_t pointer type parameters. */
#define BT_GAP_LE_CLEAR_RESOLVING_LIST                      0x2029  /**< Operation type: clean the resolving list with NULL type parameters. */
typedef uint32_t bt_gap_le_set_resolving_list_op_t;                 /**< Operation type of the set resolving list. */

/**
 * @}
 */

/**
 * @defgroup Bluetoothhbif_hci_le_struct Struct
 * @{
 * Define HCI commands and event data structures.
 */

/**
 *  @brief      Disconnect command.
 */
BT_PACKED (
typedef struct {
    bt_handle_t                 connection_handle; /**< The connection handle to disconnect. */
    bt_hci_disconnect_reason_t  reason;            /**< The reason to disconnect. */
}) bt_hci_cmd_disconnect_t;

/**
 *  @brief      LE long term key request reply command complete event.
 */
BT_PACKED (
typedef struct {
    bt_hci_status_t   status;            /**< Status. */
    bt_handle_t       connection_handle; /**< Connection handle. */
}) bt_hci_evt_cc_le_long_term_key_request_reply_t;

/**
 *  @brief      LE set data length command complete event.
 */
typedef bt_hci_evt_cc_le_long_term_key_request_reply_t bt_hci_evt_cc_le_set_data_length_t;

/**
 * @brief For more information on the status parameters, please refer to <a href="https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439&_ga=1.241778370.1195131350.1437993589">Bluetooth core specification version 4.2 [VOL 2, Part E] Section 7.5</a>.
 */
/**
 *  @brief      Read the RSSI command.
 */
BT_PACKED (
typedef struct {
    bt_handle_t       handle; /**< The connection handle to read the RSSI. */
}) bt_hci_cmd_read_rssi_t;

/**
 *  @brief      LE set advertising parameters command.
 */
BT_PACKED (
typedef struct {
    uint16_t          advertising_interval_min;   /**< Minimum advertising interval. The range is from 0x0020 to 0x4000. */
    uint16_t          advertising_interval_max;   /**< Maximum advertising interval. The range is from 0x0020 to 0x4000 and should be greater than the minimum advertising interval. */
    bt_hci_advertising_type_t   advertising_type; /**< Advertising type. */
    bt_addr_type_t    own_address_type;           /**< Local device address type. */
    bt_addr_t         peer_address;               /**< Peer address. */
    uint8_t           advertising_channel_map;    /**< Advertising channel map. */
    uint8_t           advertising_filter_policy;  /**< Advertising filter policy. */
}) bt_hci_cmd_le_set_advertising_parameters_t;

/**
 *  @brief      LE set advertising data command.
 */
BT_PACKED (
typedef struct {
    uint8_t           advertising_data_length; /**< Length of the advertising data. */
    uint8_t           advertising_data[31];    /**< Advertising data. */
}) bt_hci_cmd_le_set_advertising_data_t;

/**
 *  @brief      LE set scan response data command.
 */
BT_PACKED (
typedef struct {
    uint8_t           scan_response_data_length; /**< Length of the scan response data. */
    uint8_t           scan_response_data[31];    /**< Scan response data. */
}) bt_hci_cmd_le_set_scan_response_data_t;

/**
 *  @brief      LE set advertising enable command.
 */
BT_PACKED (
typedef struct {
    bt_hci_enable_t   advertising_enable; /**< Enable or disable advertising. */
}) bt_hci_cmd_le_set_advertising_enable_t;

/**
 *  @brief      LE set scan parameters command.
 */
BT_PACKED (
typedef struct {
    bt_hci_scan_type_t          le_scan_type;           /**< Scan type. */
    uint16_t                    le_scan_interval;       /**< Scan interval. The range is from 0x0004 to 0x4000. */
    uint16_t                    le_scan_window;         /**< Scan window. The range is from 0x0004 to 0x4000. */
    bt_hci_scan_addr_type_t     own_address_type;       /**< Local device address type. */
    bt_hci_scan_filter_type_t   scanning_filter_policy; /**< Scanning filter policy. */
}) bt_hci_cmd_le_set_scan_parameters_t;

/**
 *  @brief      LE set scan enable command.
 */
BT_PACKED (
typedef struct {
    bt_hci_enable_t   le_scan_enable;    /**< Enable or disable scan. */
    bt_hci_enable_t   filter_duplicates; /**< Enable or disable duplicate filtering. */
}) bt_hci_cmd_le_set_scan_enable_t;

/**
 *  @brief      LE create connection command.
 */
BT_PACKED (
typedef struct {
    uint16_t          le_scan_interval;                  /**< Scan interval. The range is from 0x0004 to 0x4000. */
    uint16_t          le_scan_window;                    /**< Scan window. The range is from 0x0004 to 0x4000. */
    bt_hci_conn_filter_type_t   initiator_filter_policy; /**< Initiator filter policy. */
    bt_addr_t         peer_address;                      /**< The address of the peer device. */
    bt_addr_type_t    own_address_type;                  /**< Address type of the local device. */
    uint16_t          conn_interval_min;                 /**< The minimum value for the connection event interval. The range is from 0x0006 to 0x0190. */
    uint16_t          conn_interval_max;                 /**< The maximum value for the connection event interval. The range is from 0x0006 to 0x0190. */
    uint16_t          conn_latency;                      /**< Slave latency for the connection. The range is from 0x0000 to 0x01F3. */
    uint16_t          supervision_timeout;               /**< Supervision timeout for the LE link. The range is from 0x000A to 0x0C80. */
    uint16_t          minimum_ce_length;                 /**< The minimum length of a connection event needed for this LE connection. The range is from 0x0000 to 0xFFFF. */
    uint16_t          maximum_ce_length;                 /**< The maximum length of a connection event needed for this LE connection. The range is from 0x0000 to 0xFFFF. */
}) bt_hci_cmd_le_create_connection_t;

/**
 *  @brief      LE add device to the white list command.
 */
BT_PACKED (
typedef struct {
    bt_addr_t   address;                         /**< The address of the device added to the white list. */
}) bt_hci_cmd_le_add_device_to_white_list_t;

/**
  *  @brief      LE remove device from the white list command.
  */
typedef bt_hci_cmd_le_add_device_to_white_list_t bt_hci_cmd_le_remove_device_from_white_list_t;

/**
 *  @brief      LE connection update command.
 */
BT_PACKED (
typedef struct {
    bt_handle_t       connection_handle;   /**< Connection handle. */
    uint16_t          conn_interval_min;   /**< The minimum value for the connection event interval. The range is from 0x0006 to 0x0190. */
    uint16_t          conn_interval_max;   /**< The maximum value for the connection event interval. The range is from 0x0006 to 0x0190. */
    uint16_t          conn_latency;        /**< Slave latency for the connection. The range is from 0x0000 to 0x01F3. */
    uint16_t          supervision_timeout; /**< Supervision timeout for the LE link. The range is from 0x000A to 0x0C80. */
    uint16_t          minimum_ce_length;   /**< The minimum length of a connection event needed for this LE connection. The range is from 0x0000 to 0xFFFF. */
    uint16_t          maximum_ce_length;   /**< The maximum length of a connection event needed for this LE connection. The range is from 0x0000 to 0xFFFF. */
}) bt_hci_cmd_le_connection_update_t;

/**
 *  @brief      LE remote connection parameter request reply command.
 */
typedef bt_hci_cmd_le_connection_update_t bt_hci_cmd_le_remote_connection_parameter_request_reply_t;

/**
 *  @brief      LE set data length command.
 */
BT_PACKED (
typedef struct {
    bt_handle_t       connection_handle; /**< Connection handle. */
    uint16_t          tx_octets;         /**< TX octets. */
    uint16_t          tx_time;           /**< TX time. */
}) bt_hci_cmd_le_set_data_length_t;

/**
 *  @brief      LE set transmit power command.
 */
BT_PACKED (
typedef struct {
    bt_handle_t       connection_handle; /**< Connection handle. */
    uint16_t          tx_power_level;    /**< TX power level index. The range is between 0 and 7. The larger the value, the stronger the TX power is. */
}) bt_hci_cmd_le_set_tx_power_t;

/**
 *  @brief      LE add device to the resolving list command.
 */
BT_PACKED (
typedef struct {
    bt_addr_t   peer_identity_address;      /**< Identity address of the remote device. */
    bt_key_t    peer_irk;                   /**< Identity resolving key of the remote device. */
    bt_key_t    local_irk;                  /**< Identity resolving key of the local device. */
}) bt_hci_cmd_le_add_device_to_resolving_list_t;

/**
 *  @brief      LE remove device from the resolving list command.
 */
BT_PACKED (
typedef struct {
    bt_addr_t   peer_identity_address;                /**< Identity address of the remote device. */
}) bt_hci_cmd_le_remove_device_from_resolving_list_t;

/**
 *  @brief      LE read peer resolvable address command.
 */
typedef bt_hci_cmd_le_remove_device_from_resolving_list_t bt_hci_evt_cmd_le_read_peer_resolvable_address_t;
/**
 *  @brief      LE read local resolvable address command.
 */
typedef bt_hci_cmd_le_remove_device_from_resolving_list_t bt_hci_evt_cmd_le_read_local_resolvable_address_t;

/**
 *  @brief      LE set address resolution enable command.
 */
BT_PACKED (
typedef struct {
    uint8_t           address_resolution_enable; /**< Enable or disable. */
}) bt_hci_cmd_le_set_address_resolution_enable_t;

/**
 *  @brief      LE set resolvable private address timeout command.
 */
BT_PACKED (
typedef struct {
    uint16_t          rpa_timeout; /**< RPA timeout measured in seconds. The range is from 0x0001 to 0xA1B8. */
}) bt_hci_cmd_le_set_resolvable_private_address_timeout_t;

/**
 *  @brief      Read the RSSI command complete event.
 */
BT_PACKED (
typedef struct {
    bt_hci_status_t   status; /**< Status. */
    bt_handle_t       handle; /**< Handle. */
    int8_t           rssi;    /**< RSSI. */
}) bt_hci_evt_cc_read_rssi_t;

/**
 *  @brief      Disconnect complete event information.
 */
BT_PACKED (
typedef struct {
    bt_hci_status_t   status;            /**< Status. */
    bt_handle_t       connection_handle; /**< Connection handle. */
    bt_hci_disconnect_reason_t reason;   /**< Disconnect reason. */
}) bt_hci_evt_disconnect_complete_t;

/**
 * @}
 */

/**
 * @}
 * @}
 * @}
 */

#endif /* __BT_HCI_H__ */
