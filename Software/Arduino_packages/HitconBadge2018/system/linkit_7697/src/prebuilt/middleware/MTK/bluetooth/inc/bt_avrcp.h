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


#ifndef __BT_AVRCP_H__
#define __BT_AVRCP_H__

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothAVRCP AVRCP
 * @{
 * The Audio/Video Remote Control Profile (AVRCP) defines the features and procedures
 * to ensure interoperability between Bluetooth devices with audio/video functions.
 * The AVRCP API only supports controller role (CT) in the specification of Audio/Video remote control profile V1.6.0.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                              |
 * |------------------------------|-----------------------------------------------------|
 * |\b CT                         | Controller role or the device playing the Controller role. |
 * |\b TG                         | Target role or the device playing the Target role. |
 * |\b PDU                        | Protocol Data Unit. In AVRCP, it indicates the data unit for Metadata transfer data. |
 * |\b AV/C                       | The Audio/Video Digital Interface Command set. For more information, please refer to <a href="http://1394ta.org/wp-content/uploads/2015/07/2004006.pdf">AV/C Digital Interface Command Set</a>. |
 * |\b SDP                        | Service Discovery Protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Bluetooth#SDP">SDP</a> in Wikipedia. |
 *
 * @section bt_avrcp_api_usage How to use this module
 *
 *  - Step 1. Mandatory, implement the function #bt_app_event_callback() to handle the AVRCP events, such as connect, disconnect, send pass through, register notification, etc.
 *   - Sample code:
 *    @code
 *       void bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *       {
 *           switch (msg)
 *           {
 *               case BT_AVRCP_CONNECT_CNF:
 *               {
 *                   bt_avrcp_connect_cnf_t *params = (bt_avrcp_connect_cnf_t *)buff;
 *                   // Save the params->handle if params->result is BT_STATUS_SUCCESS.
 *                   break;
 *               }
 *               case BT_AVRCP_DISCONNECT_IND:
 *               {
 *                   bt_avrcp_disconnect_ind_t *params = (bt_avrcp_disconnect_ind_t *)buff;
 *                   // The AVRCP disconnect handler to clear the application state, context or other items.
 *                   break;
 *               }
 *               case BT_AVRCP_PASS_THROUGH_CNF:
 *               {
 *                   bt_avrcp_pass_through_cnf_t *cnf = (bt_avrcp_pass_through_cnf_t *)buff;
 *                   if (status == BT_STATUS_SUCCESS)
 *                   {
 *                       // Send a key release pass through command if the key press command confirmation is received.
 *                       if (cnf->op_state == BT_AVRCP_OPERATION_STATE_PUSH)
 *                       {
 *                           bt_avrcp_send_pass_through_command(cnf->conn_id, cnf->key_code, BT_AVRCP_OPERATION_STATE_RELEASED);
 *                           // Keep the current AVRCP application state and context until the BT_AVRCP_OPERATION_STATE_RELEASED event is confirmed.
 *                       }
 *                   }
 *                   break;
 *               }
 *               case BT_AVRCP_EVENT_NOTIFICATION_IND:
 *               {
 *                   bt_avrcp_event_notification_t *cnf = (bt_avrcp_event_notification_t *)buff;
 *                   if (status  == BT_STATUS_AVRCP_INTERIM)
 *                   {
 *                       // Do something.
 *                   } else if (status == BT_STATUS_SUCCESS)
 *                       // Response to a successfully registered notification.
 *                       // The registered notification is changed.
 *                       // An additional NOTIFY command is expected to be sent.
 *                       // Handle the notification.
 *
 *                   } else {
 *                       // Error handing.   
 *                   }
 *                   break;
 *               }
 *               default:
 *                   break;
 *           }
 *       }
 *    @endcode
 *  - Step 2. Mandatory, connect to the remote device, as shown in the figure titled as "AVRCP connection establishment" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *    @code
 *       // Connect to the remote device with a remote address and connection handle given as input parameters.
 *       // The event BT_AVRCP_CONNECT_CNF is in bt_avrcp_common_callback().
 *       ret = bt_avrcp_connect(&handle, &dev_addr);
 *
 *    @endcode
 *  - Step 3. Mandatory, disconnect from the remote device, as shown in the figure titled as "AVRCP connection release" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *    @code
 *       // Disconnect the remote device, with a given connection handle.
 *       ret = bt_avrcp_disconnect(handle);
 *
 *    @endcode
 *  - Step 4. Mandatory, in the CT role, register notification and send pass through
 *           command after the AVRCP connection is established, as shown in the figure titled as "AV/C command procedure" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *    @code
 *       // Pass through command.
 *       // Send a pass through command, pass the connection handle, operation ID and operation state,
 *       // then handle the event BT_AVRCP_PASS_THROUGH_CNF in #bt_app_event_callback().
 *       ret = bt_avrcp_send_pass_through_command(handle, BT_AVRCP_OPERATION_ID_PLAY, BT_AVRCP_OPERATION_STATE_PUSH);
 *
 *       // Register a notification event.
 *       // Register a notification event, pass the connection handle, event ID and interval, then
 *       // handle the event BT_AVRCP_EVENT_NOTIFICATION_IND in #bt_app_event_callback().
 *       // Note, according to the AVRCP specification, interval is applicable only for BT_AVRCP_EVENT_PLAYBACK_POS_CHANGED.
 *       ret = bt_avrcp_register_notification(handle, BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED, 255);
 *    @endcode
 *  - Step 5. Optional, list player application setting attributes, get
 *           or set the player application setting values, as shown in the figure titled as "AV/C command procedure" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *    @code
 *       // A command to list the player application setting attributes.
 *       // Send a list player application setting attributes command, pass the connection handle
 *       // then handle the event BT_AVRCP_LIST_APP_SETTING_ATTRIBUTES_CNF in #bt_app_event_callback().
 *       ret = bt_avrcp_list_app_setting_attributes(handle);
 *
 *
 *       // A command to get the player application setting values.
 *       // Send a get player application setting values command, pass the connection handle and list of attribute IDs
 *       // then handle the event BT_AVRCP_GET_APP_SETTING_VALUE_CNF in #bt_app_event_callback().
 *       ret = bt_avrcp_get_app_setting_value(handle, atrribute_list_len,  &attributes_id_list);
 *    @endcode
 *  - Step 6. Optional, play element attributes, as shown in the figure titled as "AV/C command procedure" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *    @code
 *
 *       // A command to get the element attributes.
 *       // Send a get element attributes command, passing connection handle and media attribute IDs
 *       // then handle the event BT_AVRCP_GET_ELEMENT_ATTRIBUTES_CNF in #bt_app_event_callback().
 *       ret = bt_avrcp_get_element_attributes(conn_id, attribute_list_len, &attribute_id_list);
 *    @endcode
 */


#include "stdint.h"
#include "stdbool.h"
#include "bt_type.h"

BT_EXTERN_C_BEGIN
/**
 * @defgroup Bluetoothbt_avrcp_define Define
 * @{
 */

/**
 * @brief Event reported to the user
 */

#define BT_AVRCP_CONNECT_CNF                      (BT_MODULE_AVRCP | 0x0000)   /**< The connection confirmation event is triggered after the connection is established between the local and remote devices. The structure for this event is defined as #bt_avrcp_connect_cnf_t. */
#define BT_AVRCP_CONNECT_IND                      (BT_MODULE_AVRCP | 0x0001)   /**< The connection indication event indicates whether the local device will be connected with the remote device. The structure for this event is defined as #bt_avrcp_connect_ind_t. */
#define BT_AVRCP_DISCONNECT_IND                   (BT_MODULE_AVRCP | 0x0002)   /**< The disconnect indication event indicates the connection disconnected between the local device and the remote device.
                                                                                    Not allowed to start a new connection in the same call tree because the AVRCP needs more time to free the resources. 
                                                                                    The structure for this event is defined as #bt_avrcp_disconnect_ind_t. */
#define BT_AVRCP_PASS_THROUGH_CNF                 (BT_MODULE_AVRCP | 0x0003)   /**< The pass through confirmation event after calling #bt_avrcp_send_pass_through_command(). The structure for this event is defined as #bt_avrcp_pass_through_cnf_t. */
#define BT_AVRCP_LIST_APP_SETTING_ATTRIBUTES_CNF  (BT_MODULE_AVRCP | 0x0004)   /**< The list application setting attributes confirmation event after calling #bt_avrcp_list_app_setting_attributes(). The structure for this event is defined as #bt_avrcp_list_attruibutes_response_t. */
#define BT_AVRCP_GET_APP_SETTING_VALUE_CNF        (BT_MODULE_AVRCP | 0x0005)   /**< The get application setting attributes confirmation event after calling #bt_avrcp_get_app_setting_value(). The structure for this event is defined as #bt_avrcp_get_app_setting_value_response_t. */
#define BT_AVRCP_SET_APP_SETTING_VALUE_CNF        (BT_MODULE_AVRCP | 0x0006)   /**< The set application setting attributes confirmation event after calling #bt_avrcp_set_app_setting_value(). The structure for this event is defined as #bt_avrcp_set_app_setting_value_response_t. */
#define BT_AVRCP_GET_ELEMENT_ATTRIBUTES_CNF       (BT_MODULE_AVRCP | 0x0007)   /**< The get element attributes confirmation event after calling #bt_avrcp_get_element_attributes(). The structure for this event is defined as #bt_avrcp_get_element_attributes_response_t. */
#define BT_AVRCP_EVENT_NOTIFICATION_IND           (BT_MODULE_AVRCP | 0x0008)   /**< The event notification indication to register an event 
                                                                                    after calling #bt_avrcp_register_notification(). The structure for this event is defined as #bt_avrcp_event_notification_t. */
#define BT_AVRCP_REQUEST_CONTINUING_CNF           (BT_MODULE_AVRCP | 0x0009)   /**< The request continuing confirmation event is only triggered if calling #bt_avrcp_request_continuing_response() failed. The structure for this event is defined as #bt_avrcp_metadata_error_response_t. */
#define BT_AVRCP_ABORT_CONTINUING_CNF             (BT_MODULE_AVRCP | 0x000a)   /**< The abort continuing confirmation event after calling #bt_avrcp_abort_continuing_response(). The structure for this event is defined as #bt_avrcp_abort_continuing_response_t. */

#define BT_AVRCP_SET_ABSOLUTE_VOLUME_CNF          (BT_MODULE_AVRCP | 0x000b)   /**< The set absolute volume confirmation event after calling #bt_avrcp_set_absolute_volume(). */

#define BT_AVRCP_PASS_THROUGH_COMMAND_IND         (BT_MODULE_AVRCP | 0x0010)   /**< The pass-through command indication event indicates pass through command is received from CT. */
#define BT_AVRCP_SET_ABSOLUTE_VOLUME_COMMAND_IND  (BT_MODULE_AVRCP | 0x0011)   /**< The event indicates set absolute volume command is received from CT. */
#define BT_AVRCP_REGISTER_NOTIFICATION_IND        (BT_MODULE_AVRCP | 0x0012)   /**< The event indicates register notification command is received from CT. */
#define BT_AVRCP_GET_PLAY_STATUS_NOTIFICATION_IND        (BT_MODULE_AVRCP | 0x0013)   /**< The event indicates get play status notification command is received from CT. */
#define BT_AVRCP_ELEMENT_METADATA_IND        (BT_MODULE_AVRCP | 0x0014)   /**< The event indicates element metadata indication command is received from CT. */
#define BT_AVRCP_GET_CAPABILITY_IND        (BT_MODULE_AVRCP | 0x0015)   /**< The event indicates get capability command indication is received from CT. */
#define BT_AVRCP_ELEMENT_METADATA_CONT_IND        (BT_MODULE_AVRCP | 0x0016)   /**< The event indicates element metadata continuation indication command is received from CT. */
#define BT_AVRCP_CONTINUATION_ABORT_IND        (BT_MODULE_AVRCP | 0x0017)   /**< Abort the continuation is received from CT. */
#define BT_STATUS_AVRCP_INVALID_PARAMETER           (BT_MODULE_AVRCP | 0x01)   /**< Invalid input parameter. */
#define BT_STATUS_AVRCP_FAILED                      (BT_MODULE_AVRCP | 0x02)   /**< Command execution failed with an unknown reason. */
#define BT_STATUS_AVRCP_BUSY                        (BT_MODULE_AVRCP | 0x03)   /**< The last command did not receive a response. */
#define BT_STATUS_AVRCP_LINK_ALREADY_EXISTED        (BT_MODULE_AVRCP | 0x04)   /**< Attempt to connect to an already connected device. */
#define BT_STATUS_AVRCP_SDP_QUERY_FAILED            (BT_MODULE_AVRCP | 0x05)   /**< The SDP query has failed. */
#define BT_STATUS_AVRCP_NOT_IMPLEMENTED             (BT_MODULE_AVRCP | 0x28)   /**< The command is not implemented by a remote device. */
#define BT_STATUS_AVRCP_REJECTED                    (BT_MODULE_AVRCP | 0x29)   /**< The command is rejected by a remote device. */
#define BT_STATUS_AVRCP_INTERIM                     (BT_MODULE_AVRCP | 0x2F)   /**< An interim response. */
#define BT_STATUS_AVRCP_REJECT_INVALID_COMMAND      (BT_MODULE_AVRCP | 0x30)   /**< The remote device rejected an unknown command.*/
#define BT_STATUS_AVRCP_REJECT_INVALID_PARAMETER    (BT_MODULE_AVRCP | 0x31)   /**< The remote device rejected the command as the given parameters are invalid. */
#define BT_STATUS_AVRCP_REJECT_PARAMTER_NOT_FOUND   (BT_MODULE_AVRCP | 0x32)   /**< The remote device rejected the command as the specified parameter was not found. */
#define BT_STATUS_AVRCP_REJECT_INTERNAL_ERROR       (BT_MODULE_AVRCP | 0x33)   /**< The remote device rejected the command due to an internal error. */


/**
 * @brief The operation ID of the AVRCP pass through command.
 */

#define BT_AVRCP_OPERATION_ID_SELECT  0x00 /**< Select. */
#define BT_AVRCP_OPERATION_ID_UP  0x01 /**< Up. */
#define BT_AVRCP_OPERATION_ID_DOWN 0x02 /**< Down. */
#define BT_AVRCP_OPERATION_ID_LEFT 0x03 /**< Left. */
#define BT_AVRCP_OPERATION_ID_RIGHT 0x04 /**< Right. */
#define BT_AVRCP_OPERATION_ID_RIGHT_UP 0x05 /**< Right up. */
#define BT_AVRCP_OPERATION_ID_RIGHT_DOWN 0x06 /**< Right down. */
#define BT_AVRCP_OPERATION_ID_LEFT_UP 0x07 /**< Left up. */
#define BT_AVRCP_OPERATION_ID_LEFT_DOWN 0x08 /**< Left down. */
#define BT_AVRCP_OPERATION_ID_ROOT_MENU 0x09 /**< Root menu. */
#define BT_AVRCP_OPERATION_ID_SETUP_MENU 0x0A /**< Setup menu. */
#define BT_AVRCP_OPERATION_ID_CONTENTS_MENU 0x0B /**< Contents menu. */
#define BT_AVRCP_OPERATION_ID_FAVORITE_MENU 0x0C /**< Favorite menu. */
#define BT_AVRCP_OPERATION_ID_EXIT 0x0D /**< Exit. */

#define BT_AVRCP_OPERATION_ID_0 0x20 /**< Number 0. */
#define BT_AVRCP_OPERATION_ID_1 0x21 /**< Number 1. */
#define BT_AVRCP_OPERATION_ID_2 0x22 /**< Number 2. */
#define BT_AVRCP_OPERATION_ID_3 0x23 /**< Number 3. */
#define BT_AVRCP_OPERATION_ID_4 0x24 /**< Number 4. */
#define BT_AVRCP_OPERATION_ID_5 0x25 /**< Number 5. */
#define BT_AVRCP_OPERATION_ID_6 0x26 /**< Number 6. */
#define BT_AVRCP_OPERATION_ID_7 0x27 /**< Number 7. */
#define BT_AVRCP_OPERATION_ID_8 0x28 /**< Number 8. */
#define BT_AVRCP_OPERATION_ID_9 0x29 /**< Number 9. */
#define BT_AVRCP_OPERATION_ID_DOT 0x2A /**< Dot. */
#define BT_AVRCP_OPERATION_ID_ENTER 0x2B /**< Enter. */
#define BT_AVRCP_OPERATION_ID_CLEAR 0x2C /**< Clear. */

#define BT_AVRCP_OPERATION_ID_CHANNEL_UP 0x30 /**< Channel up. */
#define BT_AVRCP_OPERATION_ID_CHANNEL_DOWN 0x31 /**< Channel down. */
#define BT_AVRCP_OPERATION_ID_PREVIOUS_CHANNEL 0x32 /**< Previous channel. */
#define BT_AVRCP_OPERATION_ID_SOUND_SELECT 0x33 /**< Sound select. */
#define BT_AVRCP_OPERATION_ID_INPUT_SELECT 0x34 /**< Input select. */
#define BT_AVRCP_OPERATION_ID_DISPLAY_INFO 0x35 /**< Display information. */
#define BT_AVRCP_OPERATION_ID_HELP 0x36 /**< Help. */
#define BT_AVRCP_OPERATION_ID_PAGE_UP 0x37 /**< Page up. */
#define BT_AVRCP_OPERATION_ID_PAGE_DOWN 0x38 /**< Page down. */

#define BT_AVRCP_OPERATION_ID_POWER 0x40 /**< Power. */
#define BT_AVRCP_OPERATION_ID_VOLUME_UP 0x41 /**< Volume up. */
#define BT_AVRCP_OPERATION_ID_VOLUME_DOWN 0x42 /**< Volume down. */
#define BT_AVRCP_OPERATION_ID_MUTE 0x43 /**< Mute. */
#define BT_AVRCP_OPERATION_ID_PLAY 0x44 /**< Play. */
#define BT_AVRCP_OPERATION_ID_STOP 0x45 /**< Stop. */
#define BT_AVRCP_OPERATION_ID_PAUSE 0x46 /**< Pause. */
#define BT_AVRCP_OPERATION_ID_RECORD 0x47 /**< Record. */
#define BT_AVRCP_OPERATION_ID_REWIND 0x48 /**< Rewind. */
#define BT_AVRCP_OPERATION_ID_FAST_FORWARD 0x49 /**< Fast forward.*/
#define BT_AVRCP_OPERATION_ID_EJECT 0x4A /**< Eject. */
#define BT_AVRCP_OPERATION_ID_FORWARD 0x4B /**< Forward. */
#define BT_AVRCP_OPERATION_ID_BACKWARD 0x4C /**< Backward. */

#define BT_AVRCP_OPERATION_ID_ANGLE 0x50 /**< Angle. */
#define BT_AVRCP_OPERATION_ID_SUBPICTURE 0x51 /**< Sub picture. */

#define BT_AVRCP_OPERATION_ID_F1 0x71 /**< F1. */
#define BT_AVRCP_OPERATION_ID_F2 0x72 /**< F2. */
#define BT_AVRCP_OPERATION_ID_F3 0x73 /**< F3. */
#define BT_AVRCP_OPERATION_ID_F4 0x74 /**< F4. */
#define BT_AVRCP_OPERATION_ID_F5 0x75 /**< F5. */

#define BT_AVRCP_OPERATION_ID_RESERVED 0x7F /**< Reserved. */
typedef uint8_t bt_avrcp_operation_id_t; /**< The operation ID type. */

#define BT_AVRCP_FALSE 0
#define BT_AVRCP_TRUE 1

/**
* @brief The role of avrcp connection.
*/

#define BT_AVRCP_ROLE_CT             0          /**< Controller role. */
#define BT_AVRCP_ROLE_TG             1          /**< Target role. */
#define BT_AVRCP_ROLE_CT_AND_TG		 2
#define BT_AVRCP_ROLE_UNDEF		255
#define BT_AVRCP_MAX_CHANNEL   2 /* One Target and one Controller. */

typedef uint8_t bt_avrcp_role;


/**
* @brief The key state of the pass through command.
*/

#define BT_AVRCP_OPERATION_STATE_PUSH 0  /**< The push state. */
#define BT_AVRCP_OPERATION_STATE_RELEASED 1  /**< The released state. */
typedef uint8_t bt_avrcp_operation_state_t;  /**< The key state type of the operation. */

/**
* @brief The packet type indicating the fragmented state of the packet.
*/

#define BT_AVRCP_METADATA_PACKET_TYPE_NON_FRAGMENT 0x00     /**< The packet is not fragmented. */
#define BT_AVRCP_METADATA_PACKET_TYPE_START 0x01            /**< The starting packet of the fragmented packets. */
#define BT_AVRCP_METADATA_PACKET_TYPE_CONTINUE 0x02         /**< The packet is in the middle of the fragmented packets. */
#define BT_AVRCP_METADATA_PACKET_TYPE_END 0x03              /**< The final packet of the fragmented packets. */
typedef uint16_t bt_avrcp_metadata_packet_type_t;           /**< The packet type to indicate the packet fragmentation state. */


/* METADATA */

typedef enum
{
	BT_AVRCP_FIRST_OR_ONLY_IND_REQ = 0,/**< First or only metadata packet.*/
	BT_AVRCP_CONTINUATION_IND_REQ,/**< Continuation request, to continue transferring packets.*/
	BT_AVRCP_CONTINUATION_ABORT_IND_REQ,/**< Indication type, to delete the pending PDU data .*/
	BT_AVRCP_PACKET_TYPE_IND_TOTAL
}bt_avrcp_continuation_data_req_type;

typedef bt_avrcp_continuation_data_req_type data_type;

/**
* @brief The media attribute IDs.
*/

#define BT_AVRCP_MEDIA_ATTRIBUTE_TITLE 0x01                 /**< Display the title of the media. */
#define BT_AVRCP_MEDIA_ATTRIBUTE_ARTIST_NAME 0x02           /**< Display the name of the artist. */
#define BT_AVRCP_MEDIA_ATTRIBUTE_ALBUM_NAME 0x03            /**< Display the name of the album. */
#define BT_AVRCP_MEDIA_ATTRIBUTE_MEDIA_NUMBER 0x04          /**< Display the number of the media, such as the track number of the CD. */
#define BT_AVRCP_MEDIA_ATTRIBUTE_TOTAL_MEDIA_NUMBER 0x05    /**< Display the total number of the media, such as the total number of tracks on the CD. */
#define BT_AVRCP_MEDIA_ATTRIBUTE_GENRE 0x06                 /**< Display the music genre of the media. */
#define BT_AVRCP_MEDIA_ATTRIBUTE_PLAYING_TIME 0x07          /**< Display the playing duration in milliseconds. */
typedef uint32_t bt_avrcp_media_attribute_t;                 /**< The type of media attributes IDs. */

/**
* @brief The event of register notification.
*/

#define    BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED 0x01      /**< The playback status of the current track has changed. */
#define    BT_AVRCP_EVENT_TRACK_CHANGED 0x02                /**< The current track is changed. */
#define    BT_AVRCP_EVENT_TRACK_REACHED_END 0x03            /**< End of track reached. */
#define    BT_AVRCP_EVENT_TRACK_REACHED_START 0x04          /**< Start of a track. */
#define    BT_AVRCP_EVENT_PLAYBACK_POS_CHANGED 0x05         /**< The playback position has changed. */
#define    BT_AVRCP_EVENT_BATT_STATUS_CHANGED 0x06          /**< The battery status has changed. */
#define    BT_AVRCP_EVENT_SYSTEM_STATUS_CHANGED 0x07        /**< The system status has changed. */
#define    BT_AVRCP_EVENT_PLAYER_APP_SETTING_CHANGED 0x08   /**< The player application settings are changed. */
#define    BT_AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED 0x09 /**< */
#define    BT_AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED 0x0a	/**< Available Players Changed Notification: event enables the CT to be informed if a new player is available to address, for instance start or install.*/
#define    BT_AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED 0x0b	/**< Addressed Player Changed Notification: The response format for this event is the Player ID of the currently addressed player.*/
#define    BT_AVRCP_EVENT_UIDS_CHANGED 0x0c			/**< */
#define    BT_AVRCP_EVENT_VOLUME_CHANGED 0x0D               /**< The absolute volume has changed. */
typedef uint8_t bt_avrcp_event_t;  /**< The event type of register notification. */

/**
* @brief The System status for a registered event BT_AVRCP_EVENT_SYSTEM_STATUS_CHANGED.
*/

#define    BT_AVRCP_STATUS_SYSTEM_POWER_ON 0x00      /**< The player is on. */
#define    BT_AVRCP_STATUS_SYSTEM_POWER_OFF 0x01                /**<The player is off. */
#define    BT_AVRCP_STATUS_SYSTEM_UNPLUGGED 0x02            /**< The player is not connected to Bluetooth. */

/**
* @brief Batterystatus for a registered event BT_AVRCP_EVENT_BATT_STATUS_CHANGED.
*/

#define    BT_AVRCP_STATUS_BATT_NORMAL 0x00      /**< Battery operation is in normal state. */
#define    BT_AVRCP_STATUS_BATT_WARNING 0x01                /**< The battery level is low, operation will terminate soon.*/
#define    BT_AVRCP_STATUS_BATT_CRITICAL 0x02        /**< Critical battery level, the operation will terminate soon.  */
#define    BT_AVRCP_STATUS_BATT_EXTERNAL 0x03          /**< Plugged into an external power supply. */
#define    BT_AVRCP_STATUS_BATT_FULL_CHARGE 0x04         /**< The device is completely charged from an external power supply. */
             
/**
* @brief Playstatus for a registered event BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED.
*/

#define    BT_AVRCP_STATUS_PLAY_STOPPED 0x00      /**< The player has stopped. */
#define    BT_AVRCP_STATUS_PLAY_PLAYING 0x01                /**< The song is playing. */
#define    BT_AVRCP_STATUS_PLAY_PAUSED 0x02            /**< The song is paused. */
#define    BT_AVRCP_STATUS_PLAY_FWD_SEEK 0x03          /**< Forward seek. */
#define    BT_AVRCP_STATUS_PLAY_REV_SEEK 0x04         /**< Reverse seek. */
#define    BT_AVRCP_STATUS_PLAY_ERROR 0xFF            /**< An error occurred.  */

            
typedef uint8_t bt_avrcp_status_t;  /**< The event type of register notification. */

/**
* @brief The PDU ID of the VENDOR DEPENDENT command.
*/
#define BT_AVRCP_PDU_ID_GET_CAPABILITIES 0X10    /**< The PDU ID to get TG capabilities and provide it to CT. */
#define BT_AVRCP_PDU_ID_LIST_APP_SETTING_ATTRIBUTES 0X11    /**< The PDU ID for #bt_avrcp_list_app_setting_attributes(). */
#define BT_AVRCP_PDU_ID_GET_APP_SETTING_VALUE 0X13          /**< The PDU ID for #bt_avrcp_get_app_setting_value(). */
#define BT_AVRCP_PDU_ID_SET_APP_SETTING_VALUE 0X14          /**< The PDU ID for #bt_avrcp_set_app_setting_value(). */ 
#define BT_AVRCP_PDU_ID_GET_ELEMENT_ATTRIBUTES 0x20         /**< The PDU ID for #bt_avrcp_get_element_attributes(). */ 
#define BT_AVRCP_PDU_ID_GET_PLAY_STATUS 0x30                /**< The PDU ID for #bt_avrcp_media_send_play_status_response(). */ 
#define BT_AVRCP_PDU_ID_REGISTER_NOTIFICATION 0X31          /**< The PDU ID for #bt_avrcp_register_notification(). */ 
#define BT_AVRCP_PDU_ID_REQUEST_CONTINUING_RSP 0X40         /**< The PDU ID for #bt_avrcp_request_continuing_response(). */
#define BT_AVRCP_PDU_ID_ABORT_CONTINUING_RSP  0X41          /**< The PDU ID for #bt_avrcp_abort_continuing_response(). */
#define BT_AVRCP_PDU_ID_SET_ABSOLUTE_VOLUME   0x50          /**< The PDU ID for #bt_avrcp_set_absolute_volume(). */

typedef uint8_t bt_avrcp_pdu_id_t;                          /**< The type of the PDU ID. */

#define BT_AVRCP_RESPONSE_NOT_IMPLEMENTED     0x08          /**< Response code for AVRCP command. TG does not implement the command sent by CT. */
#define BT_AVRCP_RESPONSE_ACCEPTED            0x09          /**< Response code for AVRCP command. TG accepts and executes the command.*/
#define BT_AVRCP_RESPONSE_REJECTED            0x0A          /**< Response code for AVRCP command. TG implements the requested command but cannot respond because the current state doesn't allow it. */
#define BT_AVRCP_RESPONSE_CHANGED             0x0D          /**< Response code for AVRCP command. TG sends the notification that the state of TG has changed. */
#define BT_AVRCP_RESPONSE_INTERIM             0x0F          /**< Response code for AVRCP command. TG has accepted the requested command but cannot return information within 100 milliseconds. */
typedef uint8_t bt_avrcp_response_t;

/* Notification */
#define    BT_AVRCP_EVENT_MEDIA_PLAY_STOPPED 0x00
#define    BT_AVRCP_EVENT_MEDIA_PLAYING 0x01
#define    BT_AVRCP_EVENT_MEDIA_PAUSED 0x02
#define    BT_AVRCP_EVENT_MEDIA_FWD_SEEK 0x03
#define    BT_AVRCP_EVENT_MEDIA_REV_SEEK 0x04
#define    BT_AVRCP_EVENT_MEDIA_ERROR 0xFF
typedef uint8_t bt_avrcp_media_play_status_event_t;  /**< The event type of register notification. */


/*4. Capabilities */
#define BT_AVRCP_CAPABILITY_COMPANY_ID 0x2
#define BT_AVRCP_CAPABILITY_EVENTS_SUPPORTED 0x3 /*Indicates the capability request from CT. */
typedef uint8_t bt_avrcp_capability_types_t;

#define BT_AVRCP_ELEMENT_ATTRIBUTE_MAX_DATA_SIZE 450

/* The maximum length of individual attribute content, including the maximum length of title, artist name, album name and genre.*/
#define BT_AVRCP_MAX_MEDIA_ATTRIBUTE_LEN 64

/**
 * @}
 */

/**
 * @defgroup Bluetoothbt_avrcp_struct Struct
 * @{
 */


typedef struct {
	uint32_t role;
} bt_avrcp_init_struct;

/**
* @brief The struct for #BT_AVRCP_CONNECT_CNF.
*/

typedef struct{
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
}bt_avrcp_connect_cnf_t;

/**
* @brief The struct for #BT_AVRCP_CONNECT_IND.
*/

typedef struct{
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
    bt_bd_addr_t *address;                                  /**< The address of a remote device to start a connection. */
}bt_avrcp_connect_ind_t;


/**
* @brief The struct for #BT_AVRCP_DISCONNECT_IND.
*/

typedef bt_avrcp_connect_cnf_t bt_avrcp_disconnect_ind_t;

/**
* @brief The struct for #BT_AVRCP_PASS_THROUGH_CNF.
*/
typedef struct{
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
    bt_avrcp_operation_id_t op_id;                          /**< The operation ID of the pass through command. */
    bt_avrcp_operation_state_t op_state;                    /**< The key state of the operation. */
}bt_avrcp_pass_through_cnf_t;

/**
 *@brief The struct for #BT_AVRCP_PASS_THROUGH_CMD.
 */

typedef bt_avrcp_pass_through_cnf_t bt_avrcp_pass_through_command_ind_t;

/**
* @brief The parameter of #bt_avrcp_get_app_setting_value.
*/

BT_PACKED (
typedef struct{
    uint8_t attribute_id;                                   /**< The player application setting attribute ID of the requested settings. */
})bt_avrcp_get_app_setting_value_t;

/**
* @brief The struct of player application setting value, with attribute ID and the corresponding value.
*/
BT_PACKED (
typedef struct{
    uint8_t attribute_id;                                   /**< The player application setting attribute ID. */
    uint8_t value_id;                                       /**< The player application setting value ID. */
})bt_avrcp_app_setting_value_t;


/**
* @brief The parameter of #bt_avrcp_get_element_attributes.
*/

BT_PACKED (
typedef struct{
    bt_avrcp_media_attribute_t attribute_id;                                  /**< The attribute ID for the media attribute to be retrieved. */
})bt_avrcp_get_element_attributes_t;

/**
* @brief The struct to list player application setting attribute.
*/
BT_PACKED (
typedef struct{
    uint8_t attribute_id;                                   /**< The player application setting attribute ID. */ 
})bt_avrcp_list_attributes_response_value_t;

/**
* @brief The struct of #BT_AVRCP_LIST_APP_SETTING_ATTRIBUTES_CNF.
*/
typedef struct{
    uint32_t handle;                                                    /**< The connection handle of the AVRCP. */
    bt_avrcp_metadata_packet_type_t  packet_type;                       /**< The packet type indicates if it is fragmented. */
    uint16_t length;                                                    /**< The total length of the attribute_list. */  
    union {
        struct{
            uint8_t number;                                             /**< The number of members in the attribute_list. */
            bt_avrcp_list_attributes_response_value_t *attribute_list;  /**< The list of player application setting attribute IDs. */
        };
        uint8_t *data;                                                  /**< The fragmented data. If the packet type is BT_AVRCP_METADATA_PACKET_TYPE_CONTINUE or
                                                                             BT_AVRCP_METADATA_PACKET_TYPE_END, the application should use the data to compose a complete attribute list. */ 
    };
}bt_avrcp_list_attruibutes_response_t;

/**
* @brief The struct of #BT_AVRCP_GET_APP_SETTING_VALUE_CNF.
*/
typedef struct{
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
    bt_avrcp_metadata_packet_type_t packet_type;            /**< The packed type indicates if it is fragmented. */ 
    uint16_t length;                                        /**< The total length of the attribute_list. */
    union {
        struct{
            uint8_t number;                                 /**< The number of members in the attribute_list. */
            bt_avrcp_app_setting_value_t *attribute_list;   /**< The list of requested player application setting attribute ID and corresponding setting value. */
        };
        uint8_t *data;                                      /**< The fragmented data. If the packet type is BT_AVRCP_METADATA_PACKET_TYPE_CONTINUE or
                                                                 BT_AVRCP_METADATA_PACKET_TYPE_END, the application should use the data to compose a complete attribute list. */ 
    };
}bt_avrcp_get_app_setting_value_response_t;

/**
* @brief The struct of #BT_AVRCP_SET_APP_SETTING_VALUE_CNF.
*/
typedef struct{
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
}bt_avrcp_set_app_setting_value_response_t;


/**
*@brief The struct of #BT_AVRCP_ELEMENT_METADATA_IND.
*/
typedef struct{
	uint32_t handle;                                        /**< The connection handle of the AVRCP. */
	uint8_t number;                                        /**< If NumAttributes is set to zero, all attribute information is returned, else attribute information for the specified attribute IDs is returned by the TG*/
	uint8_t data[1];                                        /**< attribute ID*/
}bt_avrcp_get_element_attribute_t;


/* @brief The struct of response values to get element attribute.
*/
BT_PACKED (
typedef struct{
    bt_avrcp_media_attribute_t attribute_id;                /**< The media attribute ID. */
    uint16_t character_set_id;                              /**< The character set ID to be displayed on Control. */
    uint16_t attribute_value_length;                        /**< The length of the attribute value. */
	/* For big data PDUs: Metadata Attributes for Current Media Item and Player Application Settings.

	There are two options to create AVRCP data (PDU): bt_avrcp_media_attribute_t
	Either restrict attribute_value length to 64 bytes OR restrict total size of bt_avrcp_get_element_attributes_response_t to 512 bytes
	BT_AVRCP_MAX_PDU_LEN 512 //Total size
	BT_AVRCP_ELEMENT_ATTRIBUTE_MAX_DATA_SIZE 400 //Combined attribute size:Title, Artist name, Album name, Genre
	BT_AVRCP_MAX_MEDIA_ATTRIBUTE_LEN 64
	*/
	/* The maximum AVRCP PDU length. */
	/* Fill BT_AVRCP_MEDIA_ATTRIBUTE_TITLE as first attribute in packet/data in attribute_value array. */

    uint8_t attribute_value[1]/*[8]*/;                              /**< The attribute value with a specified character set. */

})bt_avrcp_get_element_attributes_response_value_t;

/**
* @brief The struct of #BT_AVRCP_GET_ELEMENT_ATTRIBUTES_CNF.
*/
typedef struct{
    uint32_t handle;                                                          /**< The connection handle of the AVRCP. */
    bt_avrcp_metadata_packet_type_t packet_type;                                   /**< The packet type indicates if it is fragmented. */
    uint16_t length;                                                          /**< The total length of attribute_list. */
    union{
        struct{
            uint8_t number;                                                   /**< The number of members in the attribute_list. */
            bt_avrcp_get_element_attributes_response_value_t *attribute_list;  /**< The list of response values for requested media attribute ID. */
        };
        uint8_t *data;                                                        /**< The fragmented data. If the packet type is BT_AVRCP_METADATA_PACKET_TYPE_CONTINUE or
                                                                                   BT_AVRCP_METADATA_PACKET_TYPE_END, the application should use the data to compose a complete attribute list. */ 
    };
}bt_avrcp_get_element_attributes_response_t;


/**
* @brief The struct of response data format for #BT_AVRCP_EVENT_PLAYER_APP_SETTING_CHANGED.
*/
typedef struct{
    uint8_t number;                                         /**< The number of members in the attribute_list. */
    bt_avrcp_app_setting_value_t *attribute_list;           /**< The list of changed player application setting values and corresponding attribute ID. */
}bt_avrcp_application_setting_changed_t;

/**
* @brief The struct of #BT_AVRCP_EVENT_NOTIFICATION_IND.
*/
typedef struct{
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
    bt_avrcp_event_t event_id;                              /**< The register event ID for notification. */ 
    union{
        bt_avrcp_application_setting_changed_t setting;     /**< The response data for #BT_AVRCP_EVENT_PLAYER_APP_SETTING_CHANGED. */
        uint64_t id;                                        /**< The response data for #BT_AVRCP_EVENT_TRACK_CHANGED. */
        uint32_t playback_position;                         /**< The response data for #BT_AVRCP_EVENT_PLAYBACK_POS_CHANGED. */
        uint8_t status;                                     /**< The response data for #BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED, #BT_AVRCP_EVENT_BATT_STATUS_CHANGED,
                                                                 #BT_AVRCP_EVENT_SYSTEM_STATUS_CHANGED. */
        uint8_t volume;                                     /**< The response data for #BT_AVRCP_EVENT_VOLUME_CHANGED. */
    };
}bt_avrcp_event_notification_t;

/**
 *@brief The struct of #BT_AVRCP_REGISTER_NOTIFICATION_IND.
 */
typedef struct{
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
    bt_avrcp_event_t event_id;                              /**< The event ID for registering notification. */
    uint16_t parameter_length;                              /**< Length of parameters in the registering event. */
    uint8_t *parameters;                                    /**< Parameters of the registering event. */
}bt_avrcp_register_notification_commant_t;

/**
 *@brief The struct of #BT_AVRCP_GET_PLAY_STATUS_NOTIFICATION_IND.
 */

typedef struct{
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
}bt_avrcp_get_play_status_commant_t;


/**
* @brief The struct of #BT_AVRCP_ABORT_CONTINUING_CNF.
*/
typedef struct{
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
}bt_avrcp_abort_continuing_response_t;

/**
 *@brief The struct of #BT_AVRCP_SET_ABSOLUTE_VOLUME_CNF.
 */
typedef struct{
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
    uint8_t  volume;                                        /**< The absolute volume that has actually been set in the rendering device. Values ranging from 0x00 to 0x7F represents a percentage from 0% to 100%. Other values are invalid. */
}bt_avrcp_set_absolute_volume_response_t;

/**
 *@brief The struct BT_AVRCP_SET_ABSOLUTE_VOLUME_COMMAND_IND.
 */
typedef bt_avrcp_set_absolute_volume_response_t bt_avrcp_set_absolute_volume_event_t;

/**
* @brief The error structure of all metadata commands when the status of corresponding response is not #BT_STATUS_SUCCESS.
*/
typedef struct{
    uint32_t handle;                                        /**< The connection handle for the AVRCP. */
    bt_status_t rejection_reason;                           /**< The reason to reject the metadata commands when the response status is #BT_STATUS_AVRCP_REJECTED. */
}bt_avrcp_metadata_error_response_t;

/**
 * @brief The struct of sending register notification response parameters.
 */
typedef struct{
    bt_avrcp_response_t response_type;                      /**< Type of the response. */
    uint16_t parameter_length;                              /**< Parameter length of the corresponding event. Includes the event ID field. */
    bt_avrcp_event_t event_id;                              /**< The register event ID for notification. */
    union{
        bt_avrcp_application_setting_changed_t setting;     /**< The response data for #BT_AVRCP_EVENT_PLAYER_APP_SETTING_CHANGED. */
        uint64_t id;                                        /**< The response data for #BT_AVRCP_EVENT_TRACK_CHANGED. */
        uint32_t playback_position;                         /**< The response data for #BT_AVRCP_EVENT_PLAYBACK_POS_CHANGED. */
        bt_avrcp_status_t status;                           /**< The response data for #BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED, #BT_AVRCP_EVENT_BATT_STATUS_CHANGED,
                                                            #BT_AVRCP_EVENT_SYSTEM_STATUS_CHANGED. */
        uint8_t volume;                                     /**< Parameter for #BT_AVRCP_EVENT_VOLUME_CHANGED. */
    };
}bt_avrcp_send_register_notification_response_t;


typedef struct {
	uint32_t song_length;
	uint32_t song_position;
	uint8_t bt_avrcp_media_play_status_event_t;
}bt_avrcp_media_play_status_notification_t;


typedef struct{
    uint32_t handle;                                                    /**< The connection handle of the AVRCP. */
    uint16_t length;                                                    /**< Specifies the number of CompanyID returned: 1-255 
																		1 for BT_AVRCP_CAPABILITY_COMPANY_ID; 2-255 for BT_AVRCP_CAPABILITY_EVENTS_SUPPORTED*/
    union {
        struct{
            uint8_t number;                                             /**< The number of members in the attribute_list. */
            bt_avrcp_event_t *attribute_list;                           /**< The list of capability attribute IDs. : bt_avrcp_event_t*/
        };
        uint8_t *data;                                                  
                                                                              
    };
}bt_avrcp_capability_attributes_response_t;


/**
 * @}
 */


/**
 * @brief                 This function connects to the specified remote
 *                        device. The #BT_AVRCP_CONNECT_CNF event is sent to the upper layer
 *                        with the connection request result.
 * @param[out] handle     is the connection handle of the specified remote device. The handle is only valid when the return value is #BT_STATUS_SUCCESS.
 * @param[in]  address    is a pointer to the remote device's address.
 * @return                #BT_STATUS_SUCCESS, the connection request started successfully. #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the AVRCP.
 *                        #BT_STATUS_AVRCP_LINK_ALREADY_EXISTED, the AVRCP channel already exists.
 */

bt_status_t bt_avrcp_connect(uint32_t *handle, const bt_bd_addr_t *address);

/**
 * @brief                 This function responds to the specified remote device's incoming connection. 
 *                        The function should be called after #BT_AVRCP_CONNECT_IND is received. The #BT_AVRCP_CONNECT_CNF event is sent to the
 *                        upper layer with the connection request result.
 * @param[in] handle      is the AVRCP connection handle for the specified remote device.
 * @param[in] accept      is whether to accept or reject the remote device's connection request.
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request. #BT_STATUS_AVRCP_INVALID_PARAMETER, the handle or address is NULL. 
 */
bt_status_t bt_avrcp_connect_response(uint32_t handle, bool accept);

/**
 * @brief             This function disconnects the specified remote device.
 * @param[in] handle  is the AVRCP handle for the specified remote device.
 * @return            #BT_STATUS_SUCCESS, the disconnect request completed successfully. #BT_STATUS_AVRCP_INVALID_PARAMETER, the handle is NULL.
 */

bt_status_t bt_avrcp_disconnect(uint32_t handle);

/**
 * @brief               This function sends pass through command to
 *                      the specified TG device. #BT_AVRCP_PASS_THROUGH_CNF event is
 *                      sent to the upper layer with the request result.
 * @param[in] handle    is the AVRCP handle for the specified remote device.
 * @param[in] op_id     is the operation ID.
 * @param[in] op_state  is the operation state.
 * @return              #BT_STATUS_SUCCESS, the pass through command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                      #BT_STATUS_AVRCP_INVALID_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */

bt_status_t bt_avrcp_send_pass_through_command(uint32_t handle, bt_avrcp_operation_id_t op_id, bt_avrcp_operation_state_t op_state);

/**
 * @brief               This function sends pass through response after receiving pass through command from the remote CT device.
 *                      The function should only be called after #BT_AVRCP_PASS_THROUGH_COMMAND_IND is received.
 * @param[in] handle    is the AVRCP handle of the remote device.
 * @param[in] response  is the type of response. #BT_AVRCP_RESPONSE_NOT_IMPLEMENTED or #BT_AVRCP_RESPONSE_ACCEPTED or #BT_AVRCP_RESPONSE_REJECTED.
 * @param[in] op_id     is the corresponding operation ID.
 * @param[in] op_state  is the corresponding operation state. #BT_AVRCP_OPERATION_STATE_PUSH or #BT_AVRCP_OPERATION_STATE_RELEASE.
 * @return              #BT_STATUS_SUCCESS, the pass through command is sent successfully. #BT_STATUS_AVRCP_INVALID_PARAMETER, the handel is NULL.
                        #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_send_pass_through_response(uint32_t handle, bt_avrcp_response_t response, bt_avrcp_operation_id_t op_id, bt_avrcp_operation_state_t op_state);

/**
 * @brief             This function gets player application attributes
 *                    supported at TG device. #BT_AVRCP_LIST_APP_SETTING_ATTRIBUTES_CNF event
 *                    is sent to the upper layer with the request result.
 * @param[in] handle  is the AVRCP handle for the specified remote device.
 * @return            #BT_STATUS_SUCCESS, the list app setting command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                    #BT_STATUS_AVRCP_INVALID_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */

bt_status_t bt_avrcp_list_app_setting_attributes(uint32_t handle);

/**
 * @brief                    This function gets the player application setting values
 *                           for specified attribute ID at the TG device. #BT_AVRCP_GET_APP_SETTING_VALUE_CNF event
 *                           is sent to the upper layer with the request result.
 * @param[in] handle         is the AVRCP handle for the specified remote device.
 * @param[in] attribute_size is the total size of the list of attribute ID.
 * @param[in] attribute_list is the list of specified attribute ID.
 * @return                   #BT_STATUS_SUCCESS, the get app setting command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                           #BT_STATUS_AVRCP_INVALID_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_get_app_setting_value(uint32_t handle, uint16_t attribute_size, bt_avrcp_get_app_setting_value_t *attribute_list);
/**
 * @brief                          This function sets the player application setting values
 *                                 to specified attribute ID at the TG device. #BT_AVRCP_SET_APP_SETTING_VALUE_CNF event
 *                                 is sent to upper layer with the request result.
 * @param[in] handle               is the AVRCP handle for the specified remote device.
 * @param[in] attribute_size       is the total size of the list of attribute value and corresponding attribute ID.
 * @param[in] attribute_value_list is the list of attribute value and corresponding attribute ID.
 * @return                         #BT_STATUS_SUCCESS, the set app setting command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                                 #BT_STATUS_AVRCP_INVALID_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */

bt_status_t bt_avrcp_set_app_setting_value(uint32_t handle, uint16_t attribute_size, bt_avrcp_app_setting_value_t *attribute_value_list);

/**
 * @brief                     This function obtains the detailed information on a
 *                            particular media file playing currently at the TG device. The event
 *                            #BT_AVRCP_GET_ELEMENT_ATTRIBUTES_CNF is sent to the upper layer
 *                            with the request result.
 * @param[in] handle          is the AVRCP handle for the specified remote device.
 * @param[in] attribute_size  is the total size of the list of media attribute ID.
 * @param[in] attribute_list  is the list of media attribute ID.
 * @return                    #BT_STATUS_SUCCESS, the get element attributes command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                            #BT_STATUS_AVRCP_INVALID_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */

bt_status_t bt_avrcp_get_element_attributes(uint32_t handle, uint16_t attribute_size, bt_avrcp_get_element_attributes_t *attribute_list);
/**
 * @brief                        This function sends a register notification command to the
 *                               TG device. #BT_AVRCP_EVENT_NOTIFICATION_IND event
 *                               is sent to the upper layer with the request result.
 * @param[in] handle             is the AVRCP handle for the specified remote device.
 * @param[in] event_id           is the related AVRCP event ID.
 * @param[in] play_back_interval is the time interval (in seconds) at which the change in
 *                               playback position is noticed.
 * @return                       #BT_STATUS_SUCCESS, the register command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                               #BT_STATUS_AVRCP_INVALID_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_register_notification(uint32_t handle, bt_avrcp_event_t event_id, uint32_t play_back_interval);
/**
 * @brief             This function requests TG device to send continuing response packet for current PDU command.
 *                    The function should be called when the packet type of metadata response is start or continue. 
 *                    The response of current PDU command is sent to the upper layer with the request.
 * @param[in] handle  is the AVRCP handle for the specified remote device.
 * @param[in] pdu_id  is the current PDU id for the corresponding command.
 * @return            #BT_STATUS_SUCCESS, the request continuing command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                    #BT_STATUS_AVRCP_INVALID_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */

bt_status_t bt_avrcp_request_continuing_response(uint32_t handle, bt_avrcp_pdu_id_t pdu_id);


/**
 * @brief             This function requests TG device to abort continuing response packet for current PDU command.
 *                    The function should be called when the packet type of metadata response is start or continue. 
 *                    #BT_AVRCP_ABORT_CONTINUING_CNF is sent to the upper layer with the request.
 * @param[in] handle  is the AVRCP handle for the specified remote device.
 * @param[in] pdu_id  is the current PDU id for the corresponding command.
 * @return            #BT_STATUS_SUCCESS, the abort continuing command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                    #BT_STATUS_AVRCP_INVALID_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */


bt_status_t bt_avrcp_abort_continuing_response(uint32_t handle, bt_avrcp_pdu_id_t pdu_id);


/**
 * @brief             This function sets the volume in percentage for the rendering device.
 *                    Note that the actual volume level set in the rendering device is returned through the volume field of #bt_avrcp_set_absolute_volume_response_t in the event #BT_AVRCP_SET_ABSOLUTE_VOLUME_CNF.
 * @param[in] handle  is the AVRCP handle of the remote device.
 * @param[in] volume  is the volume to set in percentage. The top bit (bit 7) is reserved. Values ranging from 0x00 to 0x7F specify 0% to 100% of volume. Other values are invalid.
 * @return            #BT_STATUS_SUCCESS, the set absolute volume command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                    #BT_STATUS_AVRCP_INVALID_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MOMORY, the TX memory is not enough.
 */

bt_status_t bt_avrcp_set_absolute_volume(uint32_t handle, uint8_t volume);


/**
 * @brief             This function sends the response after receiving set absolute volume command from CT device.
 *                    The function should only be called after #BT_AVRCP_SET_ABSOLUTE_VOLUME_COMMAND_IND is received.
 *
 * @param[in] handle  is the AVRCP handle of the remote device.
 * @param[in] volume  is the actual volume to set in percentage. TG can accept the assigned volume in #BT_AVRCP_SET_ABSOLUTE_COMMAND_IND command or
 *                    return the actual set volume value.
 * @return            #BT_STATUS_SUCCESS, the set absolute volume response is sent successfully. #BT_STATUS_AVRCP_INVALID_PARAMETER, the handle is NULL.
                      #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_send_set_absoulte_volume_response(uint32_t handle, uint8_t volume);


/**
 * @brief              This function sends the register notification response to the CT device.
 *                     The function should only be called in the following two cases:
 *                     1. Register notification command #BT_AVRCP_REGISTER_NOTIFICATION_IND is received with valid parameters.
 *                     2. The register notification command has received and the corresponding registered event happenes.
 *
 * @param[in] handle   is the AVRCP handle of the remote device.
 * @param[in] rsp_data is the pointer to the register notification response parameters.
 * @return             #BT_STATUS_SUCCESS, the register notification response is sent successfully. #BT_STATUS_AVRCP_INVALID_PARAMETER, the handle is NULL.
                       #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_send_register_notification_response(uint32_t handle, bt_avrcp_send_register_notification_response_t *rsp_data);



/******************************************************** TG ***************************************************************************/

/**
 * @brief                 This function initializes the AVRCP context with CT or Target role.
 * @param[in] bt_role      is the role.
 */
void bt_avrcp_init(bt_avrcp_init_struct* bt_role);

/**
 * @brief                 This function removes the role from the AVRCP context.
 */

void bt_avrcp_deinit(void);

/**
 * @brief                 This function sends notification to the remote device for currenct status of playing media, including SongLength, SongPosition and more. 
 *                        The function should be called after #BT_AVRCP_CONNECT_IND is received. The #BT_AVRCP_CONNECT_CNF event is sent to the
 *                        upper layer with the connection request result.
 * @param[in] handle      is the AVRCP connection handle for the specified remote device.
 * @param[in] data     	  notification data to be transfered to the remote device: 
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request. #BT_STATUS_AVRCP_INVALID_PARAMETER, the handle or address is NULL. 
 */
bt_status_t bt_avrcp_media_send_play_status_response(uint32_t handle, bt_avrcp_media_play_status_notification_t *data);


/**
 * @brief                 This function sends response for the abort continuation pdu. 
 * @param[in] handle      is the AVRCP connection handle for the specified remote device.
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request. #BT_STATUS_AVRCP_INVALID_PARAMETER, the handle or address is NULL. 
 */
bt_status_t bt_avrcp_send_abort_data_response(uint32_t handle);


/**
 * @brief                 This function provides metadata attributes to the remote device for some specific element(s).
 *                        // bt_avrcp_media_attribute_t : 0x01 : Title, 0x02 : Artist Name, 0x3 : Album Name etc.
 * @param[in] handle      is the AVRCP connection handle for the specified remote device.
 * @param[in] rsp_data    metadata attributes to transfer/send to the remote device: 
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request. #BT_STATUS_AVRCP_INVALID_PARAMETER, the handle or address is NULL. 
 */
 bt_status_t
 bt_avrcp_element_metadata_attributes_response(uint32_t handle, bt_avrcp_get_element_attributes_response_t *rsp_data);


/**
 * @brief                 This function sends device/profile capability to the remote device.
 *                        bt_avrcp_capability_types_t
 * @param[in] handle      is the AVRCP connection handle for the specified remote device.
 * @param[in] rsp_data    device/profile capability attributes to transfer/send to the remote device: 
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request. #BT_STATUS_AVRCP_INVALID_PARAMETER, the handle or address is NULL. 
 */
bt_status_t bt_avrcp_send_capability_response(uint32_t handle, void* response_data_t, bt_avrcp_capability_types_t type);



BT_EXTERN_C_END

/**
 * @}
 * @}
 */


#endif

