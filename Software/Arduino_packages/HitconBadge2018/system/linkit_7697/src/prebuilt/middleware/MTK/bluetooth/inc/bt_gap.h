/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
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
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
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

#ifndef __BT_GAP_H__
#define __BT_GAP_H__

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothGAP GAP
 * @{
 * @addtogroup GAP_EDR Classic Bluetooth
 * @{
 * This section describes the Generic Access Profile (GAP) APIs for Bluetooth with Enhanced Data Rate (EDR).
 * The GAP defines the generic procedures related to discovery of
 * Bluetooth devices and link management aspects of connecting to Bluetooth devices.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b GAP                        | Generic Access Profile. This profile defines the generic procedures related to discovery of Bluetooth devices and link management aspects of connecting to Bluetooth devices. It also defines procedures related to use of different security levels. |
 * |\b RSSI                       | Received Signal Strength Indication. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Received_signal_strength_indication">Wikipedia</a>. |
 * |\b SSP                        | Secure Simple Pairing. Simplifies the pairing process and improves Bluetooth security. |
 * |\b IO \b Capability          | Input/Output Capability. It is used in pairing feature exchange process to determine which pairing method to use. |
 * |\b MITM                       | Man-in-the-middle Protection. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Man-in-the-middle_attack">Wikipedia</a>. |
 * |\b COD                        | Class of Device, indicating the type of device. |
 * |\b EIR                        | Extended Inquiry Response data. It is sent during the inquiry response state. It can contain device name, transmit power level, service class UUIDs and more. |
 * |\b HCI                        | Host Controller Interface. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#HCI">Wikipedia</a>. |
 *
 * @section bt_gap_api_usage How to use this module
 *
 * - GAP confirmation or indication event is used to notify the upper layer whether a process has finished or an indication has been received.
 *   BT_GAP_XXX_CNF represents the confirmation events, such as the HCI has received the related command.
 *   BT_GAP_XXX_IND represents the indication events.
 * - GAP confirmation or indication event structures define the information required by the upper layer.
 *   The required data is an event parameter.
 *
 *   The event callback function, implemented by the upper layer, processes the received confirmation or indication events based on the event type.
 *  - Sample code:
 *   - 1. Configure GAP.
 *      @code
 *          const bt_gap_config_t bt_config_default = {
 *                 .inquiry_mode = BT_GAP_INQUIRY_MODE_RSSI_EIR,
 *                 .io_capability = BT_GAP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT,
 *                 .cod = 0x240404, // Audio device type.
 *                 .device_name = {"HB Duo dev"},
 *                 };
 *
 *          // Implement the GAP configuration callback to pass the configuration parameters to initialize the GAP process.
 *          const bt_gap_config_t* bt_gap_get_local_configuration(void)
 *          {
 *              return &bt_config_default;
 *          }
 *      @endcode
 *
 *   - 2. Search for nearby devices, as shown in the figure titled as "GAP inquiry message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *      @code
 *          void user_application_search_nearby_devices(void)
 *          {
 *              bt_gap_inquiry(15, 20); // Duration = 15*1.28sec, maximum count=20.
 *          }
 *          // Implement the event callback to process the related GAP events.
 *          bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *          {
 *              switch (msg)
 *              {
 *                  case BT_GAP_INQUIRY_CNF:
 *                      {
 *                          if (status == BT_STATUS_SUCCESS) {
 *                              // The inquiry command executed successfully.
 *                          } else {
 *                              // The inquiry command has failed.
 *                          }
 *                      }
 *                      break;
 *                  case BT_GAP_INQUIRY_IND:
 *                      {
 *                          // Find a nearby device.
 *                          bt_gap_inquiry_ind_t* device = (bt_gap_inquiry_ind_t*) buff;
 *                          // Handle the event, example connecting to the device.
 *                      }
 *                      break;
 *                  case BT_GAP_INQUIRY_COMPLETE_IND:
 *                      {
 *                          // The inquiry is complete.
 *                      }
 *                      break;
 *                  default:
 *                      break;
 *              }
 *              return status;
 *          }
 *      @endcode
 *
 *   - 3. Make the device visible, as shown in the figure titled as "Set the scan mode message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *      @code
 *          void user_application_make_visiable(void)
 *          {
 *              bt_gap_set_scan_mode(BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE);
 *          }
 *          // Implement the event callback to process the related GAP events.
 *          bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *          {
 *              switch (msg)
 *              {
 *                  case BT_GAP_SET_SCAN_MODE_CNF:
 *                      {
 *                          if (status == BT_STATUS_SUCCESS) {
 *                              // bt_gap_set_scan_mode() completed successfully, it can be found by nearby devices.
 *                          } else {
 *                              // Setting the scan mode has failed.
 *                          }
 *                      }
 *                      break;
 *                  default:
 *                      break;
 *              }
 *              return status;
 *          }
 *      @endcode
 *
 *   - 4. Create links.
 *      @code
 *          // Call the related profile API, such as bt_hfp_connect().
 *          // Implement the event callback to process the related GAP events.
 *          bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *          {
 *              switch (msg)
 *              {
 *                  case BT_GAP_LINK_STATUS_UPDATED_IND:
 *                      {
 *                          bt_gap_link_status_updated_ind_t* param = (bt_gap_link_status_updated_ind_t*) buff;
 *                          // Handle the link status update event.
 *                          if (param->link_status == BT_GAP_LINK_STATUS_DISCONNECTED) {
 *                               // The link is disconnected and it cannot re-connect in this function call stack.
 *                          } elseif (param->link_status >= BT_GAP_LINK_STATUS_CONNECTED_0) {
 *                               // The link is connected.
 *                          }
 *                          break;
 *                      }
 *                  default:
 *                      break;
 *              }
 *              return status;
 *          }
 *      @endcode
 *
 *   - 5. Bonding, as shown in the figure titled as "Bonding using number comparison message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *      @code
 *          // Start bonding when profiles are connecting and required security or the remote device request bonding.
 *          // Implement the event callback to process the related GAP events.
 *          bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *          {
 *              switch (msg)
 *              {
 *                  case BT_GAP_BONDING_START_IND:
 *                      {
 *                          bt_gap_connection_handle_t* link_handle = (bt_gap_connection_handle_t*) buff;
 *                          // link_handle is going to bond.
 *                          break;
 *                      }
 *                  case BT_GAP_IO_CAPABILITY_REQ_IND:
 *                      {
 *                          // This event is received after BT_GAP_BONDING_START_IND and both devices support SSP.
 *                          bt_gap_connection_handle_t handle;
 *                          handle = (bt_gap_connection_handle_t)buff;
 *                          // Call the function bt_gap_reply_io_capability_request() to accept bonding or call bt_gap_reject_io_capability_request() to terminate bonding.
 *                          break;
 *                      }
 *                  case BT_GAP_USER_CONFIRM_REQ_IND:
 *                      {
 *                          // This event is received after BT_GAP_IO_CAPABILITY_REQ_IND and IO Capability of both devices is BT_GAP_IO_CAPABILITY_DISPLAY_YES_NO.
 *                          // Call the function bt_gap_reply_user_confirm_request(true) to accept bonding or call bt_gap_reply_user_confirm_request(false) to terminate bonding.
 *                          break;
 *                      }
 *                  case BT_GAP_USER_PASSKEY_NOTIFICATION_IND:
 *                      {
 *                          // This event is received after BT_GAP_IO_CAPABILITY_REQ_IND, and local IO Capability is BT_GAP_IO_CAPABILITY_DISPLAY_ONLY or BT_GAP_IO_CAPABILITY_DISPLAY_YES_NO and remote IO Capability is BT_GAP_IO_CAPABILITY_KEYBOARD_ONLY
 *                          // Display the passkey on the screen.
 *                          uint32_t* passkey = (uint32_t*) buff;
 *                          // Call GDI APIs to show the passkey.
 *                          break;
 *                      }
 *                  case BT_GAP_USER_PASSKEY_NOTIFICATION_END_IND:
 *                      {
 *                          // This event will be received after BT_GAP_USER_PASSKEY_NOTIFICATION_IND.
 *                          // User should stop displaying the passkey.
 *                          break;
 *                      }
 *                  case BT_GAP_USER_REPLY_PASSKEY_IND:
 *                      {
 *                          // This event will be received after BT_GAP_IO_CAPABILITY_REQ_IND, and remote IO Capability is BT_GAP_IO_CAPABILITY_DISPLAY_ONLY or BT_GAP_IO_CAPABILITY_DISPLAY_YES_NO and local IO Capability is BT_GAP_IO_CAPABILITY_KEYBOARD_ONLY
 *                          // Call the function bt_gap_reply_passkey_request() to replay the passkey before passkey timeout(25 second).
 *                          break;
 *                      }
 *                  case BT_GAP_LINK_KEY_NOTIFICATION_IND:
 *                      {
 *                          // This event is received before BT_GAP_BONDING_COMPLETE_IND and bonding success or the old link key is phased out.
 *                          bt_gap_link_key_notification_ind_t * key_info = (bt_gap_link_key_notification_ind_t *) buff;
 *                          if (key_info->key_type == BT_GAP_LINK_KEY_TYPE_INVALID) {
 *                              // The old link key is phased out, delete it from the link key database.
 *                          }
 *                          else {
 *                              // Save the new link key, and key_type to link key database.
 *                          }
 *                          break;
 *                      }
 *                  case BT_GAP_BONDING_COMPLETE_IND:
 *                      {
 *                          // Bonding has finished.
 *                          if (status == BT_STATUS_SUCCESS) {
 *                              // Bonding is successfully complete.
 *                          } else {
 *                              // Bonding has Failed.
 *                          }
 *                          break;
 *                      }
 *                  default:
 *                      break;
 *              }
 *              return status;
 *          }
 *          // Provide the link key in a user-defined API implementation.
 *          void bt_gap_get_link_key(bt_gap_link_key_notification_ind_t* key_information)
 *          {
 *              bool is_key_find;
 *              // Find the link key according to key_information->address from the link key database.
 *              // If the link key is found, fill the key_information->key and the key_information->key_type.
 *              if (is_key_find) {
 *                  key_information->key_type = type;
 *                  memcpy(&(key_information->key), key, BT_KEY_SIZE);
 *              }
 *          }
 *
 *          // If local or remote device doesn't support SSP, the pin code pairing will be used.
 *          // Default pin code is "0000", to change it, implement the function bt_gap_get_pin_code().
 *          static const bt_gap_pin_code_information_t bt_gap_my_pin_code =
 *          {
 *              .pin_len = 4,
 *              .pin_code = {"1111"},
 *          };
 *
 *          // A user-defined API.
 *          const bt_gap_pin_code_information_t* bt_gap_get_pin_code(void)
 *          {
 *              return &bt_gap_my_pin_code;
 *          }
 *      @endcode
 *
 */

#include "bt_type.h"
#include "bt_system.h"

BT_EXTERN_C_BEGIN

/**
 * @defgroup Bluetoothhbif_gap_edr_define Define
 * @{
 * Define Classic Bluetooth GAP data types and values.
 */
#define BT_GAP_LINK_STATUS_UPDATED_IND              (BT_MODULE_GAP | 0x0101)    /**< This event indicates the the link status changed, with #bt_gap_link_status_updated_ind_t payload. */
#define BT_GAP_IO_CAPABILITY_REQ_IND                (BT_MODULE_GAP | 0x0103)    /**< This event asks user for IO Capability response, with #bt_gap_connection_handle_t payload, user need invoke #bt_gap_reply_io_capability_request() API to accept and reply the pairing request,
                                                                                    or invoke #bt_gap_reject_io_capability_request() API to reject the pairing request. */
#define BT_GAP_USER_CONFIRM_REQ_IND                 (BT_MODULE_GAP | 0x0104)    /**< This event asks user for value confirm, with uint32_t payload. The user then needs to invoke #bt_gap_reply_user_confirm_request() API to replay it according to the payload. */
#define BT_GAP_USER_PASSKEY_NOTIFICATION_IND        (BT_MODULE_GAP | 0x0105)    /**< This event notifies the passkey value to the user, with uint32_t payload. The user then needs to display the passkey on the screen. */
#define BT_GAP_USER_PASSKEY_NOTIFICATION_END_IND    (BT_MODULE_GAP | 0x0106)    /**< This event notifies the passkey entered to the user, with NULL payload. The user needs to cancel the passkey showing on the screen. */
#define BT_GAP_LINK_KEY_NOTIFICATION_IND            (BT_MODULE_GAP | 0x0107)    /**< This event notifies user to update link key information with #bt_gap_link_key_notification_ind_t payload. */
#define BT_GAP_USER_REPLY_PASSKEY_IND               (BT_MODULE_GAP | 0x0108)    /**< This event asks user for passkey with NULL payload. The user needs to invoke the #bt_gap_reply_passkey_request() API to replay it according to the display of the remote device. */
#define BT_GAP_SET_SCAN_MODE_CNF                    (BT_MODULE_GAP | 0x0109)    /**< This event indicates the confirmation of setting scan mode with NULL payload. */
#define BT_GAP_INQUIRY_CNF                          (BT_MODULE_GAP | 0x010A)    /**< This event indicates the confirmation of inquiry with NULL payload. */
#define BT_GAP_CANCEL_INQUIRY_CNF                   (BT_MODULE_GAP | 0x010B)    /**< This event indicates the confirmation of cancel inquiry with NULL payload. */
#define BT_GAP_INQUIRY_IND                          (BT_MODULE_GAP | 0x010C)    /**< This event indicates the result of inquiry with #bt_gap_inquiry_ind_t payload. */
#define BT_GAP_INQUIRY_COMPLETE_IND                 (BT_MODULE_GAP | 0x010D)    /**< This event indicates the conpletement of inquiry with NULL payload. */
#define BT_GAP_SET_ROLE_CNF                         (BT_MODULE_GAP | 0x010E)    /**< This event indicates the confirmation of #bt_gap_set_role() with NULL payload. */
#define BT_GAP_ROLE_CHANGED_IND                     (BT_MODULE_GAP | 0x010F)    /**< This event indicates the link role changed with #bt_gap_role_changed_ind_t payload. */
#define BT_GAP_READ_REMOTE_NAME_CNF                 (BT_MODULE_GAP | 0x0110)    /**< This event indicates the confirmation of #bt_gap_read_remote_name() with NULL payload. */
#define BT_GAP_READ_REMOTE_NAME_COMPLETE_IND        (BT_MODULE_GAP | 0x0111)    /**< This event indicates the result of #bt_gap_read_remote_name() with #bt_gap_read_remote_name_complete_ind_t payload. */
#define BT_GAP_READ_CANCEL_REMOTE_NAME_CNF          (BT_MODULE_GAP | 0x0112)    /**< This event indicates the confirmation of #bt_gap_cancel_name_request() with NULL payload. */
#define BT_GAP_ENTER_TEST_MODE_CNF                  (BT_MODULE_GAP | 0x0113)    /**< This event indicates the confirmation of #bt_gap_enter_test_mode() with NULL payload. */
#define BT_GAP_WRITE_INQUIRY_TX_PWR_CNF             (BT_MODULE_GAP | 0x0114)    /**< This event indicates the confirmation of #bt_gap_write_inquiry_tx() with NULL payload. */
#define BT_GAP_READ_INQUIRY_RESPONSE_TX_CNF         (BT_MODULE_GAP | 0x0115)    /**< This event indicates the confirmation of #bt_gap_read_inquiry_response_tx() with uint8_t payload. */
#define BT_GAP_READ_RSSI_CNF                        (BT_MODULE_GAP | 0x0116)    /**< This event indicates the confirmation of #bt_gap_read_rssi() with #bt_gap_read_rssi_cnf_t payload. */
#define BT_GAP_GET_ROLE_CNF                         (BT_MODULE_GAP | 0x0117)    /**< This event indicates the confirmation of #bt_gap_get_role() with bt_gap_get_role_cnf_t payload. */
#define BT_GAP_SET_EIR_CNF                          (BT_MODULE_GAP | 0x0118)    /**< This event indicates the confirmation of #bt_gap_set_extended_inquiry_response() with NULL payload. */
#define BT_GAP_BONDING_START_IND                    (BT_MODULE_GAP | 0x0119)    /**< This event indicates the start of bonding with #bt_gap_connection_handle_t payload. */
#define BT_GAP_BONDING_COMPLETE_IND                 (BT_MODULE_GAP | 0x011A)    /**< This event indicates the result of bonding with NULL payload. */
#define BT_GAP_WRITING_PAGE_SCAN_CNF                (BT_MODULE_GAP | 0x011B)    /**< This event indicates the confirmation of #bt_gap_write_page_scan_activity() with NULL payload. */
#define BT_GAP_WRITING_INQUIRY_SCAN_CNF             (BT_MODULE_GAP | 0x011C)    /**< This event indicates the confirmation of #bt_gap_write_inquiry_scan_activity() with NULL payload. */
#define BT_GAP_SNIFF_MODE_CHANGE_IND                (BT_MODULE_GAP | 0x011F)          /**< This event indicates link mode change, with #bt_gap_sniff_mode_changed_ind_t payload. */

#define BT_GAP_SCAN_MODE_NOT_ACCESSIBLE              0x00       /**< The device is not accessible. */
#define BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY           0x01       /**< The device is discoverable only. */
#define BT_GAP_SCAN_MODE_CONNECTABLE_ONLY            0x02       /**< The device is connectable only. */
#define BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE          0x03       /**< The device is discoverable and connectable. */
typedef uint8_t bt_gap_scan_mode_t;                             /**< GAP scan mode types for #bt_gap_set_scan_mode(). */

#define BT_GAP_MAX_PIN_CODE_LENGTH          16  /**< The maximum length of pin code. */
#define BT_GAP_MAX_DEVICE_NAME_LENGTH       31  /**< The maximum length of the device name, including null-ending char.*/

#define BT_GAP_OOB_DATA_PRESENTED_NONE               0x00       /**< Out-of-band data is not present. */
#define BT_GAP_OOB_DATA_PRESENTED_P192               0x01       /**< P192 Out-of-band data is present. */
#define BT_GAP_OOB_DATA_PRESENTED_P256               0x02       /**< P256 Out-of-band data is present. */
#define BT_GAP_OOB_DATA_PRESENTED_P192_AND_P256      0x03       /**< P192 and P256 Out-of-band data are present. */
typedef uint8_t bt_gap_oob_data_presented_t;                    /**< The type of Out-of-band data are present. */

#define BT_GAP_IO_CAPABILITY_DISPLAY_ONLY            0x00       /**< The device has display only. */
#define BT_GAP_IO_CAPABILITY_DISPLAY_YES_NO          0x01       /**< The device can display "YES" and "NO" buttons on the display. */
#define BT_GAP_IO_CAPABILITY_KEYBOARD_ONLY           0x02       /**< The device has keyboard only. */
#define BT_GAP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT      0x03       /**< The device does not have input or output. */
typedef uint8_t bt_gap_io_capability_t;                         /**< The device IO Capability types. */

#define BT_GAP_SECURITY_LEVEL_0         0x8000             /**< No security. */
#define BT_GAP_SECURITY_LEVEL_1         0x9000             /**< Minimal user interaction. */
#define BT_GAP_SECURITY_LEVEL_2         0xA000             /**< Encrypted. */
#define BT_GAP_SECURITY_LEVEL_3         0xB000             /**< MITM protected. */
#define BT_GAP_SECURITY_LEVEL_4         0xC000             /**< The highest security, P256 key. */
typedef uint16_t bt_gap_security_level_t;                  /**< The link security level types. */

#define BT_GAP_LINK_STATUS_NONE                 0x0000                        /**< The link status is none. */
#define BT_GAP_LINK_STATUS_DISCONNECTED         0x0004                        /**< The link is disconnected. */
#define BT_GAP_LINK_STATUS_CONNECTED_0          BT_GAP_SECURITY_LEVEL_0       /**< The link is connected without security. */
#define BT_GAP_LINK_STATUS_CONNECTED_1          BT_GAP_SECURITY_LEVEL_1       /**< The link is connected with Minimal user interaction security. */
#define BT_GAP_LINK_STATUS_CONNECTED_2          BT_GAP_SECURITY_LEVEL_2       /**< The link is connected with unauthenticated encrypted security. */
#define BT_GAP_LINK_STATUS_CONNECTED_3          BT_GAP_SECURITY_LEVEL_3       /**< The link is connected with authenticated encrypted security. */
#define BT_GAP_LINK_STATUS_CONNECTED_4          BT_GAP_SECURITY_LEVEL_4       /**< The link is connected with the highest security. */
typedef uint16_t             bt_gap_link_status_t;                            /**< The link status types, refer to #BT_GAP_LINK_STATUS_UPDATED_IND. */

#define BT_GAP_SECURITY_AUTH_REQUEST_NO_BONDING_AUTO_ACCEPTED           0x00    /**< The authenticated request for no bonding and auto-reply confirmation. */
#define BT_GAP_SECURITY_AUTH_REQUEST_MITM_NO_BONDING                    0x01    /**< The authenticated request for no bonding and MITM protect. */
#define BT_GAP_SECURITY_AUTH_REQUEST_DEDICATED_BONDING_AUTO_ACCEPTED    0x02    /**< The authenticated request for dedicated bonding and auto-reply confirmation. */
#define BT_GAP_SECURITY_AUTH_REQUEST_MITM_DEDICATED_BONDING             0x03    /**< The authenticated request for dedicated bonding and MITM protect. */
#define BT_GAP_SECURITY_AUTH_REQUEST_GENERAL_BONDING_AUTO_ACCEPTED      0x04    /**< The authenticated request for general bonding and auto-reply confirmation. */
#define BT_GAP_SECURITY_AUTH_REQUEST_MITM_GENERAL_BONDING               0x05    /**< The authenticated request for general bonding and MITM protect. */
typedef uint8_t bt_gap_security_auth_request_t;                                 /**< The types of authenticated request, refer to # bt_gap_reply_io_capability_request(). */

#define BT_GAP_LINK_KEY_TYPE_COMBINATION           0x00   /**< The combination link-key type. */
#define BT_GAP_LINK_KEY_TYPE_LOCAL_UNIT            0x01   /**< The local unit link-key type. */
#define BT_GAP_LINK_KEY_TYPE_REMOTE_UNIT           0x02   /**< The remote unit link-key type. */
#define BT_GAP_LINK_KEY_TYPE_DEBUG                 0x03   /**< The debug link-key type. */
#define BT_GAP_LINK_KEY_TYPE_UNAUTH_P192           0x04   /**< The P192 unauthenticated link-key type. */
#define BT_GAP_LINK_KEY_TYPE_AUTH_P192             0x05   /**< The P192 authenticated link-key type. */
#define BT_GAP_LINK_KEY_TYPE_CHANGED               0x06   /**< The changed link-key type. */
#define BT_GAP_LINK_KEY_TYPE_UNAUTH_P256           0x07   /**< The P256 unauthenticated link-key type. */
#define BT_GAP_LINK_KEY_TYPE_AUTH_P256             0x08   /**< The P256 authenticated link-key type. */
/**
 * @deprecated Please use #BT_GAP_LINK_KEY_TYPE_INVALID instead.
 */
#define BT_GAP_LINK_KEY_TYPE_INVAILIDE             0xFF   /**< The link-key is phase out, delete it. */
#define BT_GAP_LINK_KEY_TYPE_INVALID               0xFF   /**< The link-key is phase out, delete it. */
typedef uint8_t bt_gap_link_key_type_t;                   /**< The link-key types. */

#define BT_GAP_INQUIRY_MODE_STANDARD               0x00   /**< The standard inquiry mode. It is the default mode. */
#define BT_GAP_INQUIRY_MODE_RSSI                   0x01   /**< The RSSI inquiry mode. */
#define BT_GAP_INQUIRY_MODE_RSSI_EIR               0x02   /**< The RSSI and EDR inquiry mode. */
typedef uint8_t bt_gap_inquiry_mode_t;                    /**< The inquiry mode types. */

#define BT_GAP_LINK_SNIFF_TYPE_ACTIVE                  0x00     /**< Connection active status. */
#define BT_GAP_LINK_SNIFF_TYPE_SNIFFED                 0x02     /**< Connection sniff status. */
#define BT_GAP_LINK_SNIFF_TYPE_SUB_SNIFFED             0x03     /**< Connection sniff subrating status. */
typedef uint8_t bt_gap_link_sniff_status_t;                     /**< The connection sniff status types. */

#define BT_GAP_ACCEPT_AS_MASTER                               0x00       /**< Accept the connecting request as master role. */
#define BT_GAP_ACCEPT_AS_SLAVE                                0x01       /**< Accept the connecting request as slave role. */
#define BT_GAP_REJECTED_DUE_TO_LIMITED_RESOURCES              0x0D       /**< Reject the connecting request due to a lack of resources. */
#define BT_GAP_REJECTED_DUE_TO_SECURITY_REASONS               0x0E       /**< Reject the connecting request due to security. */
#define BT_GAP_ACCEPT_REJECTED_DUE_TO_UNACCEPTABLE_BDADDR     0x0F       /**< Reject the connecting request due to unacceptable device address. */
typedef uint8_t bt_gap_accept_connection_status_t;              /**< Connection request status check. */

typedef uint32_t bt_gap_connection_handle_t;              /**< The link handle type. */
/**
 * @}
 */

/**
 * @defgroup Bluetoothhbif_gap_edr_struct Struct
 * @{
 * Define Classic Bluetooth GAP structures for GAP APIs and events.
 */

/**
 * @brief This structure defines the #BT_GAP_ROLE_CHANGED_IND parameter type.
 */
typedef struct {
    bt_gap_connection_handle_t      handle;                 /**< The link handle. */
    bt_role_t                       local_role;             /**< The current role of local device. */
} bt_gap_role_changed_ind_t;

typedef bt_gap_role_changed_ind_t bt_gap_get_role_cnf_t;   /**< Define the #BT_GAP_GET_ROLE_CNF parameter type */

/**
 * @brief This structure defines the default sniff parameters used in the function #bt_gap_set_default_sniff_parameters().
 */
typedef struct {
    uint16_t    max_sniff_interval; /**< The maximum acceptable interval between each consecutive sniff periods with a range from 0x0006 to 0x0540. Time = N * 0.625msec. */
    uint16_t    min_sniff_interval; /**< The minimum acceptable interval between each consecutive sniff periods with a range from 0x0006 to 0x0540. Time = N * 0.625msec. */
    uint16_t    sniff_attempt;      /**< The number of master-to-slave transmission slots during which a device should listen for traffic with a range from 0x0001 to 0x7FFF. Time = N * 1.25msec. */
    uint16_t    sniff_timeout;      /**< The amount of time before a sniff timeout occurs with a range from 0x0000 to 0x0028. Time = N * 1.25msec. */
} bt_gap_default_sniff_params_t;

/**
 * @brief This structure defines the #BT_GAP_SNIFF_MODE_CHANGE_IND parameter type.
 */
typedef struct {
    bt_gap_connection_handle_t    handle;         /**< Connection handle. */
    bt_gap_link_sniff_status_t    sniff_status;   /**< The status of the connection. */
    uint16_t                      sniff_interval; /**< It is not available when notifying BT_GAP_LINK_SNIFF_TYPE_SUB_SNIFFED event, Time = N * 0.625msec. */
} bt_gap_sniff_mode_changed_ind_t;

/**
 * @brief This structure defines the default sniff subrating parameters used in the function #bt_gap_set_default_sniff_subrating_parameters().
 */
typedef struct {
    uint16_t    latency;              /**< The maximum latency that the remote device may use with a range from 0x0002 to 0xFFFE. Time = N * 0.625msec. */
    uint16_t    min_remote_timeout;   /**< The minimum base sniff subrate timeout that the remote device may use with a range from 0x0006 to 0x0540. Time = N * 0.625msec. */
    uint16_t    min_local_timeout;    /**< The minimum base sniff subrate timeout that the local device may use with a range from 0x0001 to 0x7FFF. Time = N * 1.25msec. */
} bt_gap_default_sniff_subrating_params_t;

/**
 * @brief This structure defines the #BT_GAP_LINK_STATUS_UPDATED_IND parameter type.
 */
typedef struct {
    bt_gap_connection_handle_t      handle;                 /**< The link handle. */
    bt_gap_link_status_t            link_status;            /**< The status. */
    const bt_bd_addr_t*             address;                /**< Address of a remote device. */
} bt_gap_link_status_updated_ind_t;

/**
 * @brief This structure defines the #BT_GAP_READ_RSSI_CNF parameter type.
 */
typedef struct {
    bt_gap_connection_handle_t      handle;                 /**< The link handle. */
    int8_t                          rssi;                   /**< The RSSI. */
} bt_gap_read_rssi_cnf_t;

/**
 * @brief This structure defines the pin code information used in the function #bt_gap_get_pin_code().
 */
BT_PACKED (
typedef struct {
    uint8_t                pin_len;                                    /**< The length of the pin_code. */
    uint8_t                pin_code[BT_GAP_MAX_PIN_CODE_LENGTH];       /**< The pin_code buffer. */
}) bt_gap_pin_code_information_t;

/**
 * @brief This structure defines the #BT_GAP_INQUIRY_IND event parameter type.
 */
typedef struct {
    const bt_bd_addr_t*         address;            /**< Address of a remote device. */
    uint32_t                    cod;                /**< The class of a remote device with a range from 0 to 0xFFFFFF.*/
    const uint8_t*              rssi;               /**< A pointer to the RSSI, it can be NULL. The RSSI length is 1 byte with a range from -127 to 20. */
    const uint8_t*              eir;                /**< A pointer to the EIR information, it can be NULL. The maximum length of EIR is 240 bytes. */
} bt_gap_inquiry_ind_t;

/**
 * @brief This structure defines the #BT_GAP_READ_REMOTE_NAME_COMPLETE_IND event parameter type.
 */
typedef struct {
    const bt_bd_addr_t*         address;            /**< Address of a remote device. */
    const char*                 name;               /**< A UTF-8 encoded user-friendly descriptive name for the remote device. */
} bt_gap_read_remote_name_complete_ind_t;

/**
 * @brief This structure defines the #BT_GAP_LINK_KEY_NOTIFICATION_IND events parameter type.
 */
BT_PACKED (
typedef struct {
    bt_bd_addr_t             address;            /**< Address of a remote device. */
    bt_key_t                 key;                /**< The passkey. */
    bt_gap_link_key_type_t   key_type;           /**< The passkey type. */
}) bt_gap_link_key_notification_ind_t;

/**
* @brief     This structure defines the Bluetooth configuration information, please refer to #bt_gap_get_local_configuration().
*/
typedef struct {
    bt_gap_inquiry_mode_t    inquiry_mode;                 /**< Indicates the inquiry result format. */
    bt_gap_io_capability_t    io_capability;               /**< Defines the IO Capability of the local device. */
    uint32_t   cod;                                        /**< Defines the class of the local device. Please refer to <a href="https://www.bluetooth.com/specifications/assigned-numbers/baseband">The Class of Device field</a>.*/
    char       device_name[BT_GAP_MAX_DEVICE_NAME_LENGTH]; /**< Defines the name of the local device with '\0' ending. */
} bt_gap_config_t;

/**
 * @}
 */

/**
 * @brief     This function sets the accessible mode.
 * @param[in] mode is the accessing mode.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully.
 *            #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *            #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_set_scan_mode(bt_gap_scan_mode_t mode);

/**
 * @brief     This function inquires the search for nearby devices. The #BT_GAP_INQUIRY_IND event is received once a new device is found.
 * @param[in] duration                   is the maximum time of the inquiry. The inquiry stops after the timeout.
                                         The range is from 0x01 to 0x30. Time = N * 1.28sec.
 * @param[in] max_count                  is the maximum device count of the inquiry, stops the inquiry, if the number of searched devices reaches the max_count.
                                         The range is from 0x00 to 0xFF, 0 - unlimited number of responses.
 * @return                               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                       #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *                                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_inquiry(uint8_t duration, uint8_t max_count);

/**
 * @brief                                This function stops the inquiry.
 * @return                               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                       #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *                                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_cancel_inquiry(void);

/**
 * @brief     This function exits the sniff mode for the link.
 * @param[in] handle                     is the link handle.
 * @return                               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                       #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *                                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 *                                       #BT_STATUS_CONNECTION_NOT_FOUND, the operation has failed, the link does not exist.
 */
bt_status_t bt_gap_exit_sniff_mode(bt_gap_connection_handle_t handle);

/**
 * @brief     This function disconnects the special link.
 * @param[in] handle                     is the link handle to be disconnected.
 * @return                               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                       #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *                                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 *                                       #BT_STATUS_CONNECTION_NOT_FOUND, the operation has failed, the link does not exist.
 */
bt_status_t bt_gap_disconnect(bt_gap_connection_handle_t handle);

/**
 * @brief     This function changes the device's role type. The #BT_GAP_ROLE_CHANGED_IND event is received when the role changed.
 * @param[in] handle                     is the link handle to change.
 * @param[in] role                       is the role to change.
 * @return                               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                       #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *                                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_set_role(bt_gap_connection_handle_t handle, bt_role_t role);

/**
 * @brief     This function gets the device's role type. The #BT_GAP_GET_ROLE_CNF event is received when finished.
 * @param[in] handle                     is the link handle.
 * @return                               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                       #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *                                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_get_role(bt_gap_connection_handle_t handle);

/**
 * @brief     This function gets the device's name, no need to be connected to the device. The #BT_GAP_READ_REMOTE_NAME_COMPLETE_IND event is received once the operation is complete.
 * @param[in] address                    is the address of the remote device.
 * @return                               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                       #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *                                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_read_remote_name(const bt_bd_addr_t* address);

/**
 * @brief     This function cancels the request of #bt_gap_read_remote_name().
 * @param[in] address                    is the address of the remote device.
 * @return                               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                       #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *                                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_cancel_name_request(const bt_bd_addr_t* address);

/**
 * @brief     This function sets the device into a test mode.
 * @return                               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                       #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *                                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_enter_test_mode(void);

/**
 * @brief     This function sets the EIR data.
 * @param[in] eir_data                   is the content of EIR data.
 * @param[in] data_length                is the size of the EIR data, the maximum length is 240.
 * @return                               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                       #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *                                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_set_extended_inquiry_response(uint8_t* eir_data, uint32_t data_length);

/**
 * @brief     This function writes the inquiry transmit power.
 * @param[in] tx_power                   is the transmit power, the range is from -70 to 20.
 * @return                               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                       #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *                                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_write_inquiry_tx(int8_t tx_power);

/**
 * @brief     This function reads the inquiry response transmit power. The #BT_GAP_READ_INQUIRY_RESPONSE_TX_CNF event is received once the operation is complete.
 * @return                               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                       #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *                                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_read_inquiry_response_tx(void);

/**
 * @brief     This function writes the values for the interval and the window of page scanning. The #BT_GAP_WRITING_PAGE_SCAN_CNF event is received once the operation is complete.
 * @param[in] interval                   is the page scan interval, default value is 0x0800, the range is from 0x12 to 0x1000. Time = N * 0.626 ms.
 * @param[in] window                     is the page scan window, it shall be less than or equal to interval, default value is 0x0012, the range is from 0x12 to 0x1000. Time = N * 0.626 ms.
 * @return                               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                       #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *                                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_write_page_scan_activity(uint16_t interval, uint16_t window);

/**
 * @brief     This function writes values for the interval and the window of inquiry scanning. The #BT_GAP_WRITING_INQUIRY_SCAN_CNF event will be received when finished.
 * @param[in] interval                   is the inquiry scan interval, default value is 0x1000, the range is from 0x12 to 0x1000. Time = N * 0.626 ms.
 * @param[in] window                     is the inquiry scan window, it shall be less than or equal to interval, default value is 0x0012, the range is from 0x12 to 0x1000. Time = N * 0.626 ms.
 * @return                               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                       #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *                                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_write_inquiry_scan_activity(uint16_t interval, uint16_t window);

/**
 * @brief     This function gets the address of a connected device.
 * @param[in] handle                     is the connection handle.
 * @return                               A pointer to the address, if the operation completed successfully.
 *                                       NULL, if it failed.
 */
const bt_bd_addr_t* bt_gap_get_remote_address(bt_gap_connection_handle_t handle);

/**
 * @brief     This function reads the RSSI of the connected device. #BT_GAP_READ_RSSI_CNF event is received once the operation is complete.
 * @param[in] handle                     is the link handle.
 * @return                               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                       #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 *                                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_read_rssi(bt_gap_connection_handle_t handle);

/**
 * @brief     This is a user defined callback to get the pin code and pass it to the GAP process.
 * @return    A pointer to the pin information.
 */
const bt_gap_pin_code_information_t* bt_gap_get_pin_code(void);

/**
 * @brief     This function sends a reply to the #BT_GAP_IO_CAPABILITY_REQ_IND event with Out-of-band data presented and security authentication request.
 * @param[in] oob_present                is the Out-of-band data presented.
 * @param[in] auth_request               is the security authentication request.
 * @return                               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                       #BT_STATUS_FAIL, the operation has timed out.
 *                                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_reply_io_capability_request(bt_gap_oob_data_presented_t oob_present, bt_gap_security_auth_request_t auth_request);

/**
 * @brief     This function rejects the #BT_GAP_IO_CAPABILITY_REQ_IND event and terminates the bonding.
 * @return                               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                       #BT_STATUS_FAIL, the operation has timed out.
 *                                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_reject_io_capability_request(void);

/**
 * @brief     This function is a reply to the #BT_GAP_USER_REPLY_PASSKEY_IND event.
 * @param[in] passkey    is a pointer to the user's passkey. If it is NULL, the reply is rejected.
 * @return               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                       #BT_STATUS_FAIL, the operation has timed out.
 *                       #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_reply_passkey_request(const uint32_t* passkey);

/**
 * @brief     This function is a reply to the #BT_GAP_USER_CONFIRM_REQ_IND event.
 * @param[in] accept            defines whether the pairing is accepted.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has timed out.
 *                              #BT_STATUS_OUT_OF_MEMORY, the operation has failed, out of memory.
 */
bt_status_t bt_gap_reply_user_confirm_request(bool accept);

/**
 * @brief   This is a user defined callback to get the link key and pass it to the GAP process.
 * @param[in,out] key_information     provide the key and key_type for a given address.
 * @return                            None.
 */
void bt_gap_get_link_key(bt_gap_link_key_notification_ind_t* key_information);

/**
 * @brief     This is a user defined callback to get the configuration parameters for the GAP initialization process.
 * @return    the configuration information supplied by user.
 */
const bt_gap_config_t* bt_gap_get_local_configuration(void);

/**
 * @brief     This function sets the default sniff parameters. The default settings remain effective till the system reboot.
 * @param[in] sniff_params is the default sniff parameters.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully.
 *            #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 */
bt_status_t bt_gap_set_default_sniff_parameters(const bt_gap_default_sniff_params_t* sniff_params);

/**
 * @brief     This function sets the default subrating_parameters. The default settings remain effective till the system reboot.
 * @param[in] sniff_subrating_params is the default sniff subrating parameters.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully.
 *            #BT_STATUS_FAIL, an error occurred, the operation has terminated.
 */
bt_status_t bt_gap_set_default_sniff_subrating_parameters(const bt_gap_default_sniff_subrating_params_t* sniff_subrating_params);

/**
 * @brief     This is a user defined callback to check whether to accept the connecting request or reject it.
 * @param[in] address     is the address of a connecting device.
 * @param[in] cod         is the class of a device.
 * @return    #BT_GAP_ACCEPT_AS_MASTER, accept the connecting request as master role.
 *            #BT_GAP_ACCEPT_AS_SLAVE, accept the connecting request as slave role.
 *            #BT_GAP_REJECTED_DUE_TO_LIMITED_RESOURCES, reject the connecting request due to a lack of resources.
 *            #BT_GAP_REJECTED_DUE_TO_SECURITY_REASONS, reject the connecting request due to security.
 *            #BT_GAP_ACCEPT_REJECTED_DUE_TO_UNACCEPTABLE_BDADDR, reject the connecting request due to unacceptable device address.
 */
bt_gap_accept_connection_status_t bt_gap_check_connect_request(bt_bd_addr_ptr_t address, uint32_t cod);

BT_EXTERN_C_END

/**
 * @}
 * @}
 * @}
 */

#endif

