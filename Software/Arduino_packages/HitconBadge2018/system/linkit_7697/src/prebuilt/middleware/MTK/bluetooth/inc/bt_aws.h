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

#ifndef __BT_AWS_H__
#define __BT_AWS_H__

#include "bt_a2dp.h"
#include "bt_type.h"

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothAWS AWS
 * @{
 * The Advanced Wireless Stereo (AWS) is MediaTek proprietary profile that defines the minimum set of functions such that two devices can transfer the audio data and synchronize the audio status.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b AWS                        | Advanced Wireless Stereo. |
 * |\b SBC                        | The Low Complexity Sub-Band Coding (SBC) is an audio sub-band codec specified by the Bluetooth Special Interest Group (SIG) for the Advanced Audio Distribution Profile (A2DP). SBC is a digital audio encoder and decoder used to transfer data to Bluetooth audio output devices like headphones or loudspeakers. For more information, please refer to <a href="https://en.wikipedia.org/wiki/SBC_(codec)">Wikipedia</a>.|
 * |\b AAC                        | Advanced Audio Coding (AAC) is an audio coding standard for lossy digital audio compression. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Advanced_Audio_Coding">Wikipedia</a>.|
 *
 * @section bt_aws_api_usage How to use this module
 * This section presents the AWS connection handle and a method to stream and synchronize audio signals.
 *  - 1. Mandatory, implement #bt_app_event_callback() to handle the AWS events, such as connect, disconnect, indication and more.
 *   - Sample code:
 *    @code
 *       void bt_app_event_callback(bt_msg_type_t msg_type, bt_status_t status, void *data)
 *       {
 *          switch (msg_type)
 *          {
 *              case BT_AWS_CONNECT_IND:
 *              {
 *                  bt_aws_connect_ind_t *conn_ind =
 *                      (bt_aws_connect_ind_t *)data;
 *                  bt_aws_connect_response(conn_ind->handle,true);
 *                  // Respond to the connection request.
 *                  break;
 *              }
 *              case BT_AWS_COMMAND_IND:
 *              {
 *                  // The application can get the command request when receiving this indication.
 *                  break;
 *              }
 *              case BT_AWS_NOTIFY_PLAY_IND:
 *              {
 *                  // The application can get the play notify information when receiving this indication.
 *                  break;
 *              }
 *              default:
 *              {
 *                  break;
 *              }
 *           }
 *       }
 *    @endcode
 *  - 2. Mandatory, connect to a remote device, as shown in figure titled as "AWS connection establishment message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *      Connect to a remote device by passing the remote Bluetooth address and getting the connection handle.
 *      The event #BT_AWS_CONNECT_CNF captured by #bt_app_event_callback() indicates the AWS is connected.
 *    @code
 *       bt_aws_connect(&handle, &addr);
 *    @endcode
 *  - 3. Mandatory, synchronize the start playing time with the Bluetooth clock, and play the streaming data at same time in both devices, as shown in figure titled as "AWS synchronize the voice message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *    @code
 *     // Device A
 * 
 *      bt_aws_play_notify_t play_noti;
 *      play_noti.play_time = time_dur; // The duration to play in microseconds.
 *      play_noti.sequence_num = 0;  // A user-defined packet sequence number.
 *      play_noti.play_bt_clock.nclk = nclk;  // The packet play time measured in 312.5us units.
 *      play_noti.play_bt_clock.nclk_intra = nclk_intra; // The packet play time measured in 1us units.
 *      play_noti.data_len = 3;   // The user data length.
 *      play_noti.user_data = user_data; // The user data.
 *      play_noti.bt_local_role = gap_role; // The Bluetooth GAP role.
 *      ret = bt_aws_notify_play(aws_hd, BT_SINK_SRV_AWS_MODULE_ID_A2DP, &play_noti);
 *
 *     // Defines a timer then plays the audio once it's reached.
 *     hal_gpt_start_timer_us(HAL_GPT_1, time_dur, HAL_GPT_TIMER_TYPE_ONE_SHOT);
 *
 *     // Plays the music when the time is up.
 *     //-------------------------------------------------------------------------------------
 *     // Device B
 *
 *     // Starts a timer to wait before playing the music, after receiving the #BT_AWS_NOTIFY_ID_READY_TO_PLAY notification ID.
 *       bt_aws_play_notify_t *play_noti;
 *       time_dur = play_noti->play_time.play_time;
 *       hal_gpt_register_callback(HAL_GPT_1,
 *                              bt_sink_srv_aws_gpt_cb, (void *)dev);
 *       gpt_ret = hal_gpt_start_timer_us(HAL_GPT_1, time_dur, HAL_GPT_TIMER_TYPE_ONE_SHOT);
 *
 *     // Plays the music when the time is up.
 *    @endcode
 *  - 4. Mandatory, disconnect the remote device, as shown in figure titled as "AWS connection release message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *        Disconnect the remote device by passing the connection handle.
 *        The event #BT_AWS_DISCONNECT_IND captured by #bt_app_event_callback() indicates the AWS channel has disconnected.
 *    @code
 *       ret = bt_aws_disconnect(handle);
 *    @endcode
 */



/**
 * @brief The event reported to the user.
 */

#define BT_AWS_CONNECT_IND                       (BT_MODULE_AWS | 0x0001)               /**< An attempt to connect from a remote device with the #bt_aws_connect_ind_t payload. Call #bt_aws_connect_response() to accept or reject the attempt. */
#define BT_AWS_CONNECT_CNF                       (BT_MODULE_AWS | 0x0002)               /**< The result of a connection attempt initiated by a local or remote device is available with the #bt_aws_connect_cnf_t payload. */
#define BT_AWS_DISCONNECT_IND                    (BT_MODULE_AWS | 0x0003)               /**< A disconnect attempt is initiated by a local or remote device or a link loss occurred with the #bt_aws_disconnect_ind_t payload. */
#define BT_AWS_DISCONNECT_CNF                    (BT_MODULE_AWS | 0x0004)               /**< The result of a disconnect attempt initiated by a local device is available with the #bt_aws_disconnect_cnf_t payload.  */
#define BT_AWS_COMMAND_IND                       (BT_MODULE_AWS | 0x000C)               /**< A command request is initiated from a remote device with the #bt_aws_command_ind_t payload. Call #bt_aws_command_response() to accept or reject the request. */
#define BT_AWS_NOTIFY_PLAY_IND                   (BT_MODULE_AWS | 0x000E)               /**< The notify indication is initiated from a remote device with the  #bt_aws_notification_ind_t payload. */
#define BT_AWS_STREAMING_PACKET_RECEIVED_IND     (BT_MODULE_AWS | 0x0013)               /**< A streaming packet is initiated from a remote device with the #bt_aws_streaming_received_ind_t payload.*/
#define BT_AWS_OFFSET_UPDATE_IND                 (BT_MODULE_AWS | 0x0015)               /**< A Bluetooth clock offset updated indication with the #bt_aws_offset_update_ind_t payload.*/
#define BT_AWS_BT_CLOCK_IND                      (BT_MODULE_AWS | 0x0016)               /**< A Bluetooth clock updated indication with the #bt_aws_bt_clock_ind_t payload.*/


BT_EXTERN_C_BEGIN

/**
 * @brief This structure defines the module ID.
 */
typedef uint16_t bt_aws_module_id_t;


/**
 * @brief This structure defines the streaming packet type.
 */
typedef uint8_t bt_aws_streaming_type_t;
#define BT_AWS_STREAMING_TYPE_A2DP              0X01 /**< A2DP type. */
#define BT_AWS_STREAMING_TYPE_CUSTOM            0X02 /**< Custom type. */


/**
 * @brief This structure defines the Bluetooth clock.
 */
typedef struct
{
    uint32_t nclk;          /**< The Bluetooth clock in 312.5us units.*/
    uint32_t nclk_intra;    /**< The Bluetooth clock in microseconds.*/
} bt_aws_clock_t;


/**
 * @brief This structure defines the Bluetooth clock offset.
 */
typedef struct
{
    uint32_t offset;        /**< The Bluetooth clock offset in 312.5us units.*/
    uint32_t offset_intra;  /**< The Bluetooth clock offset in microseconds.*/
} bt_aws_clock_offset_t;


/**
 *  @brief This structure defines the A2DP packet.
 */
typedef struct {
    uint8_t *data_node;                          /**< Media data pointer. */
    uint16_t total_length;                       /**< The total length of a media packet. */
    uint16_t media_offset;                       /**< The offset of media payload from the whole packet. */
} bt_aws_a2dp_packet_t;


/**
 *  @brief This structure defines the custom packet.
 */
typedef struct {
    uint8_t *data;                               /**< Media data pointer. */
    uint16_t data_length;                        /**< The total length of media packet. */
    uint8_t *media_header;                       /**< The media header pointer. */
    uint16_t media_header_len;                   /**< The media header length. */
} bt_aws_custom_packet_t;


/**
 *  @brief This structure defines the AWS streaming packet.
 */
typedef struct {
    bt_aws_streaming_type_t type;                   /**< The streaming type. */
    union
    {
        bt_aws_a2dp_packet_t a2dp_packet;           /**< The A2DP packet. */
        bt_aws_custom_packet_t custom_packet;       /**< The customized packet. */
    };
} bt_aws_streaming_packet_t;


/**
 * @brief This structure defines the AWS play time with a notification ID #BT_AWS_NOTIFY_ID_READY_TO_PLAY.
 */
typedef struct
{
    uint32_t play_time;                 /**< The delay time in microseconds. */
    uint32_t sequence_num;              /**< The packet sequence number. */
    bt_aws_clock_t play_bt_clock;       /**< The Bluetooth clock during play. */
    bt_role_t bt_local_role;            /**< The Bluetooth role. */
    uint32_t data_len;                  /**< The user data length. */
    uint8_t *user_data;                 /**< The user data. */
} bt_aws_play_notify_t;


/**
 *  @brief This structure defines the result of the #BT_AWS_CONNECT_IND event.
 */
typedef struct {
    uint32_t handle;            /**< AWS handle. */
    bt_bd_addr_t *address;      /**< Bluetooth address of a remote device. */
} bt_aws_connect_ind_t;


/**
 *  @brief This structure defines the #BT_AWS_CONNECT_CNF result.
 */
typedef struct {
    uint32_t handle;           /**< AWS handle. */
    bt_status_t status;        /**< Connection status. */
} bt_aws_connect_cnf_t;


/**
 *  @brief This structure defines the #BT_AWS_DISCONNECT_IND result.
 */
typedef struct {
    uint32_t handle;           /**< AWS handle. */
} bt_aws_disconnect_ind_t;


/**
 *  @brief This structure defines the #BT_AWS_DISCONNECT_CNF result.
 */
typedef struct {
    uint32_t handle;           /**< AWS handle. */
    bt_status_t status;        /**< Disconnect status. */
} bt_aws_disconnect_cnf_t;


/**
 *  @brief This structure defines the #BT_AWS_COMMAND_IND result.
 */
typedef struct {
    uint32_t handle;                                /**< AWS handle. */
    bt_aws_module_id_t module_id;                   /**< The module ID. */
    uint8_t *data;                                  /**< The command data. */
    uint16_t data_length;                           /**< The command data length. */
} bt_aws_command_ind_t;


/**
 *  @brief This structure defines the #BT_AWS_OFFSET_UPDATE_IND result.
 */
typedef struct {
    uint32_t handle;                           /**< The AWS handle. */
    bt_aws_clock_offset_t offset;              /**< The Bluetooth clock offset. */
} bt_aws_offset_update_ind_t;



/**
 *  @brief This structure defines the #BT_AWS_BT_CLOCK_IND result.
 */
typedef struct {
    uint32_t handle;                        /**< The AWS handle. */
    bt_aws_clock_t bt_clock;                /**< The current Bluetooth clock offset. */
} bt_aws_bt_clock_ind_t;


/**
 *  @brief This structure defines the #BT_AWS_NOTIFY_PLAY_IND result.
 */
typedef struct {
    uint32_t handle;                                        /**< The AWS handle. */
    bt_aws_module_id_t module_id;                           /**< The module ID. */
    bt_aws_play_notify_t *play_notify;                      /**< The play notification settings. */
} bt_aws_notify_play_ind_t;


/**
 *  @brief This structure defines the #BT_AWS_STREAMING_PACKET_RECEIVED_IND result.
 */
typedef struct {
    uint32_t handle;                              /**< The AWS handle. */
    bt_aws_module_id_t module_id;                 /**< The module ID. */
    bt_aws_streaming_packet_t packet;             /**< The streaming packet. */
} bt_aws_streaming_received_ind_t;



/**
 * @brief                                         This function sends an AWS connection request to the remote device.
 *                                                The event #BT_AWS_CONNECT_CNF returns when the connection request is sent.
 * @param[out] handle                             is the channel handle.
 * @param[in] address                             is the Bluetooth address of a remote device.
 * @return                                        #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                                #BT_STATUS_FAIL, the operation has failed.
 *                                                #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_aws_connect(uint32_t *handle, bt_bd_addr_t *address);


/**
 * @brief                                         This function is a response to the remote device's incoming connection request.
 *                                                Application needs to call this API when getting the event #BT_AWS_CONNECT_IND to accept or reject the connection.
 * @param[in] handle                              is the handle of the current connection.
 * @param[in] accept                              is the operation of accepting or rejecting the current connection request.
 * @return                                        #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                                #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_aws_connect_response(uint32_t handle, bool accept);


/**
 * @brief                                         This function sends an AWS disconnect request to the remote device.
 *                                                The event #BT_AWS_DISCONNECT_CNF returns the current request's result.
 * @param[in] handle                              is the handle of the current connection.
 * @return                                        #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                                #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_aws_disconnect(uint32_t handle);


/**
* @brief                                         This function sends an AWS command to the remote device.
* @param[in] handle                              is the handle of the current connection.
* @param[in] module_id                           is the module ID of the current user.
* @param[in] data                                is the command data to send.
* @param[in] data_length                         is the data length.
* @return                                        #BT_STATUS_SUCCESS, the operation completed successfully.
*                                                #BT_STATUS_FAIL, the operation has failed.
*                                                #BT_STATUS_OUT_OF_MEMORY, out of memory.
*/
bt_status_t bt_aws_send_command(uint32_t handle,
                                bt_aws_module_id_t module_id,
                                const uint8_t *data,
                                uint16_t data_length);


/**
* @brief                                         This function sends an AWS notification to the remote device.
* @param[in] handle                              is the handle of the current connection.
* @param[in] module_id                           is the module ID of the current user.
* @param[in] notificaton                         is the playing notification to notify the remote device.
* @return                                        #BT_STATUS_SUCCESS, the operation completed successfully.
*                                                #BT_STATUS_FAIL, the operation has failed.
*                                                #BT_STATUS_OUT_OF_MEMORY, out of memory.
*/
bt_status_t bt_aws_notify_play(uint32_t handle, bt_aws_module_id_t module_id, bt_aws_play_notify_t *notification);


/**
* @brief                                         This function gets the current native clock.
* @param[out] bt_clock                           is the native clock.
* @return                                        #BT_STATUS_SUCCESS, the operation completed successfully.
*                                                #BT_STATUS_FAIL, the operation has failed.
*/
bt_status_t bt_aws_get_bt_local_time(bt_aws_clock_t *native_clock);


/**
* @brief                                         This function gets the current Bluetooth clock.
* @param[in] role                                is the Bluetooth role of the current AWS connection.
* @param[in] offset                              is the Bluetooth clock offset.
* @param[out] bt_clock                           is the native Bluetooth clock.
* @return                                        #BT_STATUS_SUCCESS, the operation completed successfully.
*                                                #BT_STATUS_FAIL, the operation has failed.
*/
bt_status_t bt_aws_get_curr_bt_time(bt_role_t role, bt_aws_clock_offset_t offset, bt_aws_clock_t *bt_clock);


/**
* @brief                                         This function gets the current Bluetooth clock.
* @param[in] role                                is the Bluetooth role of the current AWS connection.
* @param[in] offset                              is the Bluetooth clock offset.
* @param[in] local_clock                         is the native clock.
* @param[out] bt_clock                           is the native Bluetooth clock.
* @return                                        #BT_STATUS_SUCCESS, the operation completed successfully.
*                                                #BT_STATUS_FAIL, the operation has failed.
*/
bt_status_t bt_aws_get_bt_time_by_local_time(bt_role_t role,
                                             bt_aws_clock_offset_t offset,
                                             bt_aws_clock_t local_clock,
                                             bt_aws_clock_t *bt_clock);


/**
* @brief                                         This function fetches the current Bluetooth clock offset.
* @param[in] handle                              is the handle of the current connection.
* @return                                        #BT_STATUS_SUCCESS, the operation completed successfully.
*                                                #BT_STATUS_FAIL, the operation has failed.
*/
bt_status_t bt_aws_fetch_bt_time_offset(uint32_t handle);


/**
* @brief                                         This function sends the streaming data to the remote device.
* @param[in] handle                              is the handle of the current connection.
* @param[in] module_id                           is the module ID of the current user.
* @param[in] streaming_packet                    is the streaming data to send.
* @return                                        the streamed data length.
*/
uint16_t bt_aws_send_streaming_packet(uint32_t handle, bt_aws_module_id_t module_id, bt_aws_streaming_packet_t *streaming_packet);


/**
* @brief                                         Get the max streaming data length.
* @param[in] handle                              is the handle of the current connection.
* @return                                        the max streaming data length.
*/
uint32_t bt_aws_get_max_streaming_packet_size(uint32_t handle);



BT_EXTERN_C_END


#endif /*__BT_AWS_H__*/
