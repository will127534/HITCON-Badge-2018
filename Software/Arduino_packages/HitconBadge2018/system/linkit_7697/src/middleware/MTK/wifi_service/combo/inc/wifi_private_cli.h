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
/**
    @file       wifi_private_cli.h
    @brief      WiFi - Configure API Examples, these CLI in the wifi_private_api.c
                is only for internal use, it will be removed or modified in future.

    @history    2016/07/27  Initial for 1st draft  (Pengfei Qiu)
 */

#ifndef __WIFI_PRIVATE_CLI_H__
#define __WIFI_PRIVATE_CLI_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "cli.h"


/**
* @brief
* @parameter
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_get_rssi_threshold_ex(uint8_t len, char *param[]);
/**
* @brief
* wifi config set rssi_threshold <enabled> <rssi value>
* @parameter
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_set_rssi_threshold_ex(uint8_t len, char *param[]);

int32_t wifi_frame_type_event_handler(wifi_event_t event, uint8_t *payload, uint32_t length);

/**
* @brief Example of set frame filter for packets format wanted to be received
* wifi config set frame_filter 1 8 221 0 15 172
* @parameter
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_set_frame_filter_ex(uint8_t len, char *param[]);

/**
* @brief Example of set frame filter for packets format wanted to be received
* wifi config set frame_filter 1 8 221 0 15 172
* @parameter
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_set_frame_filter_advanced_ex(uint8_t len, char *param[]);

/**
* @brief Set N9 Debug Level
* wifi config set n9dbg <dbg_level>
* @param [IN]dbg_level
*   0: None
*   1: ERROR
*   2. WARNING
*   3. TRACE
*   4. INFO
*   5. LAUD
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_set_n9_dbg_level(uint8_t len, char *param[]);

/**
* @brief Get N9 Debug Level
* wifi config get n9dbg
* @param [OUT]dbg_level
*   0: None
*   1: ERROR
*   2. WARNING
*   3. TRACE
*   4. INFO
*   5. LAUD
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_get_n9_dbg_level(uint8_t len, char *param[]);

/**
* @brief Example of Start/Stop WiFi Scanning
* wifi connect set scan <start/stop> <mode> <operation> [ssid] [bssid]
* @param [IN]ssid SSID, "NULL" means not specified
* @param [IN]bssid BSSID, "NULL" means not specified
* @param [IN]flag
* @param 0 Active Scan
* @param 1 Passive Scan
*
* @return  =0 means success, >0 means fail
*
* @note When SSID/BSSID specified (not NULL)
* @note 1. ProbeReq carries SSID (for Hidden AP)
* @note 2. Scan Tab always keeps the APs of this specified SSID/BSSID
*/
uint8_t wifi_connect_scan_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi TX Power
* <br><b>
* wifi config get tx_power
* </b></br>
* @param  [OUT]power: 64~190, instead of value range get from N9 is -315 to +315
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_get_tx_power_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi Tx Power
* <br><b>
* wifi config set tx_power <power>
* </b></br>
* @param [IN]interval 64 ~ 190, instead of value range set to N9 is -315 to +315
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_set_tx_power_ex(uint8_t len, char *param[]);

/**
* @brief Example n9log state set
* <br><b>
* n9log set <off/n9/host>
* </b></br>
* @param [IN]
*  off     Disable n9 logs
*  n9     Output n9 logs to uart
*  host  Output n9 logs to host
* @return  =0 means success, >0 means fail
*/
uint8_t n9log_state_set_cli(uint8_t len, char *param[]);

/**
* @brief Example of Get Wlan statistic
* <br><b>
* wifi config get wlanstat
* <br><b>
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_get_statistic_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set IP filter
* <br><b>
* wifi config set ip_filter <ip_address>
* </b></br>
* @param [IN]ip_address
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_ip_filter_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi ip_mac_port_filter ON/OFF
* <br><b>
* wifi config set ip_mac_port_filter <onoff>
* </b></br>
* @param [IN]onoff  0: OFF, 1: ON
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_ip_mac_port_filter_ex(uint8_t len, char *param[]);

/**
* @brief Example of get WiFi ip_mac_port_filter ON/OFF
* <br><b>
* wifi config get ip_mac_port_filter
* </b></br>
* @param [OUT]onoff 0: OFF, 1: ON
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_ip_mac_port_filter_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi ip_mac_port_entry. When you want to use set specific parameter, you should set the
*  valid bitmap fristly. It can be use fully and partial.
* <br><b>
*  wifi config set ip_mac_port <action> <vaild_bitmap> [src_ip] [src_mac] [src_port] [dst_ip] [dst_mac] [dst_port]
* </b></br>
* @param [IN]action   0 delete, 1 create,
* @param [IN]vaild bitmap   For more details, please refer to #wifi_ip_match_entry_filter_t.
* @param [IN]src_ip
* @param [IN]src_mac
* @param [IN]src_port  0~65535,usually is between 1024 and 5000
* @param [IN]dst_ip
* @param [IN]dst_mac
* @param [IN]dst_port  0~65535,usually is between 1024 and 5000
* @return  >=0 means success, <0 means fail
* @note entry please refer to #ip_match_entry_t
*/
uint8_t wifi_config_set_ip_mac_port_entry_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi ip_mac_port_entry List
* <br><b>
* wifi config get ip_mac_port
* </b></br>
* @param [OUT]entry_list
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_ip_mac_port_entry_ex(uint8_t len, char *param[]);

/**
* @brief Example of sends a raw Wi-Fi packet over the air
* <br><b>
* wifi config set txraw_adv <tx_mode> <mcs> <rate_fixed> <ack_required> <retry_count> <raw_packets>
* </b></br>
* @param [IN]tx_mode
* @param [IN]mcs
* @param [IN]rate_fixed
* @param [IN]ack_required
* @param [IN]retry_count
* @param [IN]raw_packets
* @return  >=0 means success, <0 means fail
* @note entry please refer to #txd_param_setting_t
*/
uint8_t wifi_tx_raw_pkt_advanced_ex(uint8_t len, char *param[]);

/**
* @brief Example of sends a raw Wi-Fi packet over the air
* <br><b>
* wifi config set ps_mode <ps_mode>
* </b></br>
* @param [IN]ps_mode
* @return  >=0 means success, <0 means fail
* @note entry please refer to #txd_param_setting_t
*/
uint8_t wifi_config_set_ps_mode_ex(uint8_t len, char *param[]);

/**
* @brief Example of sends a raw Wi-Fi packet over the air
* <br><b>
* wifi config get ps_mode
* </b></br>
* @param [OUT]ps_mode
* @return  >=0 means success, <0 means fail
* @note entry please refer to #txd_param_setting_t
*/
uint8_t wifi_config_get_ps_mode_ex(uint8_t len, char *param[]);

/**
* @brief Example of calculate pmk value
* <br><b>
* wifi config get calc_pmk <ssid> <passphrase>
* </b></br>
* @param [IN]ssid
* @param [IN]passphrase
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_calculate_pmk_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get Connection Status
* <br><b>
* wifi config get conn_status
* <br><b>
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_get_connection_status_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get Scan List
* <br><b>
* wifi config get scan_list
* <br><b>
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_get_scan_list_ex(uint8_t len, char *param[]);

#ifdef MTK_ANT_DIV_ENABLE
/**
* @brief Example of get RF Diversity antenna status
* <br><b>
* wifi config get ant_status
* <br><b>
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_get_antenna_status_ex(uint8_t len, char *param[]);

/**
* @brief Example of set RF Diversity antenna monitor
* <br><b>
* wifi config get ant_monitor
* <br><b>
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_set_antenna_monitor_ex(uint8_t len, char *param[]);

/**
* @brief Example of set RF Diversity antenna number to 0 or 1
* <br><b>
* wifi config get ant_number 0/1
* <br><b>
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_set_antenna_number_ex(uint8_t len, char *param[]);
#endif

#if defined(MTK_MINICLI_ENABLE)
#define N9_LOG_CLI_ENTRY { "n9log",    "N9 consol log state", n9log_state_set_cli},
#else
#define N9_LOG_CLI_ENTRY
#endif

#if defined(MTK_MINICLI_ENABLE)
extern cmd_t   wifi_private_cli[];
#endif

#if defined(MTK_MINICLI_ENABLE)
#define WIFI_PRIV_CLI_ENTRY     { "wifi", "wifi private cli",   NULL, wifi_private_cli },
#else
#define WIFI_PRIV_CLI_ENTRY
#endif


#ifdef __cplusplus
    }
#endif
#endif /*  __WIFI_PRIVATE_CLI_H__ */

