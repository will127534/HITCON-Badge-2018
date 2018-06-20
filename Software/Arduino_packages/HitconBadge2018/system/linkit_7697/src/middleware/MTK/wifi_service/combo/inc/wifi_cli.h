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
 * @file        wifi_cli.h
 * @brief       WiFi API Example CLI
 *
 * @history     2015/08/18   Initial for 1st draft  (Michael Rong)
 */

/**@addtogroup Wi-Fi_CLI
* @{
* This section introduces the Wi-Fi CLI APIs including terms and acronyms, supported features,
* details on how to use the WiFi-CLI, function groups, enums, structures and functions.
*
*/

/**@defgroup WIFI_COMMAND_CLI_1 WiFi_Configuration_CLI_Usage
* @{
*
* @section WiFiCLI_Terms_Chapter1_1 Wi-Fi Configuration CLI
*  <b>"WiFi Configuration Command" Cli </b>
*  |  Command Format                                   |   Usage Refer and Example                 |  Description |
*  |----------------------------------------------|------------------------------------     --    -|--------------------------------|
*  | <b>wifi config set opmode <mode></b>              |   @sa #wifi_config_set_opmode_ex          |  Set WiFi Operation Mode            |
*  | <b>wifi config get opmode</b>                     |   @sa #wifi_config_get_opmode_ex          |  Get WiFi Operation Mode            |
*  | <b>wifi config set ssid <port> <ssid></b>          |  @sa #wifi_config_set_ssid_ex            |  Set WiFi SSID                      |
*  | <b>wifi config get ssid <port></b>                |   @sa #wifi_config_get_ssid_ex            |  Get WiFi SSID                      |
*  | <b>wifi config set bssid <BSSID></b>              |   @sa #wifi_config_set_bssid_ex           |  Set WiFi BSSID                     |
*  | <b>wifi config get bssid</b>                      |   @sa #wifi_config_get_bssid_ex           |  Get WiFi BSSID                     |
*  | <b>wifi config set sec <port> <auth_mode> <encrypt_type></b> |  @sa #wifi_config_set_security_mode_ex  |  Set the authentication mode and encryption mode    |
*  | <b>wifi config get sec <port></b>                 |   @sa #wifi_config_get_security_mode_ex   |  Get the authentication mode for the specified STA/AP port     |
*  | <b>wifi config set psk <port> <password></b>        |    @sa #wifi_config_set_psk_ex          |  Set the psk for the specified STA/AP port  |
*  | <b>wifi config get psk <port></b>                 |   @sa #wifi_config_get_psk_ex             |  Get the psk for the specified STA/AP port  |
*  | <b>wifi config set pmk <port> <pmk></b>           |   @sa #wifi_config_set_pmk_ex             |  [This function is deprecated!]\n Set PMK for the specified STA/AP port      |
*  | <b>wifi config get pmk <port></b>                 |   @sa #wifi_config_get_pmk_ex             |  [This function is deprecated!]\n Get PMK for the specified STA/AP port      |
*  | <b>wifi config set rxfilter <flag></b>            |   @sa #wifi_config_set_rx_filter_ex       |  Configure rx filter for packets wanted to be received     |
*  | <b>wifi config get rxfilter</b>                   |   @sa #wifi_config_get_rx_filter_ex       |  Get rx filter for packets format wanted to be received     |
*  | <b>wifi config set mtksmart <flag></b>            |   @sa #wifi_config_set_mtk_smartconnection_filter_ex | Set MTK Smart Connection Filter       |
*  | <b>wifi config get mtksmart </b>                  |   @sa #wifi_config_get_mtk_smartconnection_filter_ex | Get MTK Smart Connection Filter  |
*  | <b>wifi config set wep <port> <key_id> <key_string_id></b>  |   @sa #wifi_config_set_wep_key_ex          |Set WiFi WEP Keys         |
*  | <b>wifi config get wep <port> <key_id></b>         |   @sa #wifi_config_get_wep_key_ex        |  Get WiFi WEP Keys                  |
*  | <b>wifi config set ch <port> <ch></b>             |   @sa #wifi_config_set_channel_ex         |  Configure channel for STA/AP wireless port. STA will keep idle and stay in channel specified     |
*  | <b>wifi config get ch <port></b>                  |   @sa #wifi_config_get_channel_ex         |  Get the current channel for STA/AP wireless port     |
*  | <b>wifi config set bw <port> <bw></b>             |   @sa #wifi_config_set_bandwidth_ex       |  Configure bandwidth for STA/AP wireless port |
*  | <b>wifi config get bw <port></b>                  |   @sa #wifi_config_get_bandwidth_ex       |  Get bandwidth for STA/AP wireless port     |
*  | <b>wifi config set wirelessmode <port> <mode></b> |  @sa #wifi_config_set_wireless_mode_ex      |   Set WiFi Wireless Mode            |
*  | <b>wifi config get wirelessmode <port></b>        |   @sa #wifi_config_get_wireless_mode_ex   |  Get WiFi Wireless Mode             |
*  | <b>wifi config set country <band> <region></b>      |   @sa #wifi_config_set_country_region_ex   | [This function is deprecated!]\n Set WiFi Country Region       |
*  | <b>wifi config get country <band></b>             |   @sa #wifi_config_get_country_region_ex  |  [This function is deprecated!]\n Get WiFi Country Region            |
*  | <b>wifi config set country_code <country_code></b>|    @sa #wifi_config_set_country_code_ex     |   Set Country Code               |
*  | <b>wifi config get country_code</b>               |   @sa #wifi_config_get_country_code_ex    |  Get Country Code                   |
*  | <b>wifi config get mac <port></b>                 |   @sa #wifi_config_get_mac_address_ex     |  Get MAC address for STA/AP wireless port   |
*  | <b>wifi config set reload</b>                     |   @sa #wifi_config_set_reload_ex          |  Reload configuration               |
*  | <b>wifi config set radio <on_off></b>             |   @sa #wifi_config_set_radio_on_ex        |  Set WiFi Radio ON/OFF              |
*  | <b>wifi config get radio</b></br>                 |   @sa #wifi_config_get_radio_on_ex        |  Get WiFi Radio ON/OFF              |
*  | <b>wifi config set rxraw <enable></b>             |   @sa #wifi_config_set_rx_raw_pkt_ex      |  Set WiFi Raw Packet Receiver       |
*  | <b>wifi config set txraw <raw_packets></b>        |   @sa #wifi_tx_raw_pkt_ex                 |  Send txraw packets                 |
*  | <b>wifi config set listen <listen_interval></b>       |  @sa #wifi_config_set_listen_interval_ex | Set WiFi Listen Interval             |
*  | <b>wifi config get listen</b>                     |   @sa #wifi_config_get_listen_interval_ex  | Get WiFi Listen Interval           |
*  | <b>wifi config set dtim <dtim_interval></b>        |  @sa #wifi_config_set_dtim_interval_ex   |  Set WiFi DTIM Interval             |
*  | <b>wifi config get dtim</b>                       |   @sa #wifi_config_get_dtim_interval_ex   |  Get WiFi DTIM Interval             |
*
*
* @section WiFiCLI_Terms_Chapter1_2 Wi-Fi Configuration CLI Usage Examples
*  <b>"Wifi Configuration Command" Cli Example</b>
*  | Example  Items                                    |	"wifi Config" Cli Example       |
*  |----------------------------------------------|----------------------------------|
*  |<b>MT7687 Station Mode</b>                         |     |
*  |<b>AP Router :OPEN </b>                            |<b>connect AP with OPEN mode:</b>\n wifi config set opmode 1\n wifi config set ssid 0 MTK_AP\n wifi config set reload |
*  |<b>AP Router :WPA2-PSK(AES)</b>                    |<b>connect AP with WPA2-PSK(AES)mode, password is 12345678:</b>\n wifi config set opmode 1\n wifi config set ssid 0 MTK_AP\n wifi config set psk 0 12345678\n wifi config set reload |
*  |<b>AP Router :WPA2-PSK(TKIP)</b>                   |<b>connect AP with WPA2-PSK(TKIP) mode:</b>\n wifi config set opmode 1\n wifi config set ssid 0 MTK_AP\n wifi config set psk 0 12345678\n wifi config set reload |
*  |<b>AP Router :WPA-PSK(TKIP)</b>                    |<b>connect AP with WPA-PSK(TKIP) mode,password is 12345678:</b>\n wifi config set opmode 1\n wifi config set ssid 0 MTK_AP\n wifi config set psk 0 12345678\n wifi config set reload |
*  |<b>AP Router :WEP(OPEN)(64bit)</b>                 |<b>connect AP with WEP OPEN(64bit) mode,WEP key is 12345:</b>\n wifi config set opmode 1\n wifi config set ssid 0 MTK_AP\n wifi config set wep 0 0 12345\n (use the first key index,the key index start from 0)\n wifi config set reload|
*  |<b>MT7687 SoftAP Mode</b>                          |     |
*  |<b>Soft AP :OPEN</b>                               |<b>config SoftAP as OPEN mode:</b>\n wifi config set opmode 2\n wifi config set ssid 1 MTK_AP\n wifi config set sec 1 0 1\n wifi config set bw 1 1(set the bandwidth of softAP as 40MHZ)\n wifi config set reload|
*  |<b>Soft AP :WPA2-PSK(AES)</b>                      |<b>config SoftAP as WPA2-PSK(AES) mode,Password is 12345678:</b>\n wifi config set opmode 2\n wifi config set ssid 1 MTK_AP\n wifi config set sec 1 7 6\n wifi config set psk 1 12345678\n wifi config set bw 1 1\n wifi config set reload|
*  |<b>Soft AP :WPA2-PSK(TKIP)</b>                     |<b>config SoftAP as WPA2-PSK(TKIP) mode,Password is 12345678:</b>\n wifi config set opmode 2\n wifi config set ssid 1 MTK_AP\n wifi config set sec 1 7 4\n wifi config set psk 1 12345678\n wifi config set bw 1 1\n wifi config set reload|
*  |<b>Soft AP :WPA-PSK(TKIP)</b>                      |<b>config SoftAP as WPA-PSK(TKIP) mode,Password is 12345678:</b>\n wifi config set opmode 2\n wifi config set ssid 1 MTK_AP\n wifi config set sec 1 4 4\n wifi config set psk 1 12345678\n wifi config set bw 1 1\n wifi config set reload|
*  |<b>Soft AP :WEP(OPEN)(64bit)</b>                   |<b>config SoftAP as WEP(OPEN)(64bit)mode,Password is 12345:</b>\n wifi config set opmode 2\n wifi config set ssid 1 MTK_AP\n wifi config set sec 1 0 0\n wifi config set wep 1 0 12345(use the first key index,the key index start from 0)\n wifi config set bw 1 1\n wifi config set reload|
*  |<b>Repeater Mode</b>                               |     |
*  |<b>AP Port :Open Mode\n AP Client Port:Open Mode</b>               |<b>The remote AP Router is in OPEN mode, 40MHz, channel 6:</b>\n wifi config set opmode 3\n wifi config set ssid 0 MTK_AP\n wifi config set ssid 1 MTK_SOFT_AP\n wifi config set sec 1 0 1\n wifi config set ch 1 6\n wifi config set bw 1 1\n wifi config set reload|
*  |<b>AP Port :WPA-PSK(TKIP)\n AP Client Port:WPA2-PSK(AES)</b>      |<b>The remote AP Router is in WPA2-PSK(AES) mode, 40MHz, channel 6:</b>\n wifi config set opmode 3\n wifi config set ssid 0 MTK_AP\n wifi config set psk 0 12345678\n wifi config set ssid 1 MTK_SOFT_AP\n wifi config set sec 1 4 4\n wifi config set psk 1 12345678\n wifi config set ch 1 6\n wifi config set bw 1 1\n wifi config set reload |
*  |<b>AP Port :WEP(OPEN)(64bit)\n AP Client Port: WPA-PSK(TKIP)</b>  |<b>The remote AP Router is in WPA-PSK(TKIP) mode, 40MHz, channel 6:</b>\n wifi config set opmode 3\n wifi config set ssid 0 MTK_AP\n wifi config set psk 0 12345678\n wifi config set ssid 1 MTK_SOFT_AP\n wifi config set sec 1 0 0\n wifi config set wep 1 0 12345(use the first key index,the key index start from 0)\n wifi config set ch 1 6\n wifi config set bw 1 1\n wifi config set reload |
*  |<b>Switch Operation Mode</b>                        |     |
*  |<b>Switch to Station Mode\n Note:\n </b> opmode value:\n STA mode:1\n SoftAP mode:2\n Repeater mode:3 |wifi config get opmode (get current opmode)\n wifi config set opmode 1(switch to station mode)\n wifi config set reload |
*  |<b>Switch to SoftAP Mode</b>                       |wifi config get opmode (get current opmode)\n wifi config set opmode 2(switch to SoftAP mode)|

*/

/**
* @}
*/


/**@defgroup WIFI_COMMAND_CLI_2 WiFi_Connection_CLI_Usage
* @{
*
* @section WiFiCLI_Terms_Chapter2_1 Wi-Fi Connection CLI
*  <b>"WiFi Connection Command" Cli </b>
*  |  Command Format                                   |   Usage Refer and Example                |  Description |
*  |---------------------------------------- -----|------------------------------------- |--------------------------------|
*  | <b>wifi connect get linkstatus</b>                |   @sa #wifi_connect_get_link_status_ex   | Get the current STA port link up / link down status of the connection       |
*  | <b>wifi connect get stalist</b>                   |   @sa #wifi_connect_get_station_list_ex  | Get WiFi Associated Station List  |
*  | <b>wifi connect get max_num_sta</b>               |   @sa #wifi_connect_get_max_station_number_ex |Get the max number of the supported stations in ap mode or Repeater Mode.   |
*  | <b>wifi connect get rssi</b>                      |   @sa #wifi_connect_get_rssi_ex          | Get rssi of the connected AP      |
*  | <b>wifi connect set scan <start/stop> <scan_mode> <scan_operation> [ssid] [bssis] </b>  | @sa #wifi_connect_set_scan_ex | Start scan with options    |
*  | <b>wifi connect set connection <link></b>         |   @sa #wifi_connect_set_connection_ex    | Set connect/link up to specifed AP    |
*  | <b>wifi connect set deauth <MAC></b>              |   @sa #wifi_connect_deauth_station_ex    | Deauth some WiFi connection       |
*  | <b>wifi connect set eventcb <enable> <enable_ID></b>|@sa #wifi_connect_set_event_callback_ex |Register WiFi Event Notifier            |
*
*/

/**
* @}
*/


/**@defgroup WIFI_COMMAND_CLI_3 WiFi_WPS_CLI_Usage
* @{
* @section WiFiCLI_Terms_Chapter3_1 Wi-Fi WPS CLI
*  <b>"WPS Configuration Command" Cli </b>
*  |  Command Format                                    |   Usage Refer and Example                |  Description |
*  |---------------------------------------------- |------------------------------------ -|--------------------------------|
*  | <b>wifi wps set device_info <Device_Name> <Manufacturer> <Model_Name> <Model_number> <Serial_number></b> | @sa #wifi_wps_set_device_info_ex | Set wifi wps device info    |
*  | <b>wifi wps get device_info</b>                    |    @sa #wifi_wps_get_device_info_ex      | Get wifi wps device info           |
*  | <b>wifi wps set auto_connection <on_off></b>        |   @sa #wifi_wps_set_auto_connection_ex  |Set wifi wps auto connection        |
*  | <b>wifi wps get auto_connection</b>                |    @sa #wifi_wps_get_auto_connection_ex  | Get wifi wps auto connection       |
*  | <b>wifi wps get pin_code <port></b>                |    @sa #wifi_wps_get_pin_code_ex         | Get wifi wps pin code              | 
*  | <b>wifi wps trigger pbc <port> <BSSID></b>          |   @sa #wifi_wps_connection_by_pbc_ex   | Set wifi wps connection by PBC          |
*  | <b>wifi wps trigger pin <port> <BSSID> <PIN_code></b>|@sa #wifi_wps_connection_by_pin_ex   | set wifi wps connection by PIN            |
*
*
* @section WiFiCLI_Terms_Chapter3_2 Wi-Fi WPS CLI Usage Examples
*  <b>"WPS Configuration Command" Cli Example</b>
*  | Example  Items                                     |	"WPS Config" Cli Example       |
*  |-----------------------------------------------|----------------------------------|
*  |<b>WPS Connection by wifi wps cli</b>               |     |
*  |<b>MT7687 as STA-Enrollee</b>                       |<b>Connect with AP Router by Push Button Connection(PBC)method\n Operation on MT7687 side:</b>\n wifi config set opmode 1(set opmode as station mode)\n wifi config set reload\n wifi wps trigger pbc 0 any(triger WPS PBC connection) <br><b>Operation on Common AP-Registrar side</b></br>\n Push the PBC button on Common AP-Registrar side |
*  |<b>MT7687 as STA-Enrollee</b>                       |<b>Connect with AP Router by Personal Identification Number(PIN) method\n Method 1:(do the WPS Connection without BSSID of the Common AP-Registrar )\n Operation on MT7687 side</b>\n wifi config set opmode 1 (switch to station mode)\n wifi config set reload\n wifi wps get pin_code 0 (get 8-digital PIN number of station enrollee, such as "34794265")\n wifi wps trigger pin 0 any 34794265 (do wps connection with AP-Registrar, AP-Registrar need input STA's PIN) <br><b>Operation on Common AP-Registrar side</b></br>\n 1.Input the STA PIN "34794265" in UI or Web of Common AP-Registrar\n 2.trigger WPS PIN in UI or Web. <br><b>Method2:(do the WPS Connection with BSSID of the Common AP-Registrar )(BSSID=00:0C:43:46:46:31)\n Operation on MT7687 side</b></br>\n wifi config set opmode 1(switch to station mode)\n wifi config set reload\n wifi wps get pin_code 0(get 8-digital PIN number of station enrollee, such as "34794265")\n wifi wps trigger pin 0 00:0C:43:46:46:31 34794265(do wps connection with AP-Registrar, AP-Registrar need input STA's PIN) <br><b>Operation on Common AP-Registrar side</b></br>\n 1.Input the STA PIN "34794265" in UI or Web of Common AP-Registrar\n 2.trigger WPS PIN in UI or Web page.|
*  |<b>MT7687 as AP-Registrar</b>                       |<b>Connect with AP Router by Push Button Connection(PBC)method\n Operation on MT7687 side:</b>\n wifi config set opmode 2(set opmode as SoftAP mode)\n wifi wps trigger pbc 1 any(triger WPS PBC connection) <br><b>Operation on Common STA-Enrollee side</b></br>\n Push the PBC button on Common STA-Enrollee side|
*  |<b>MT7687 as AP-Registrar</b>                       |<b>Connect with AP Router by Personal Identification Number(PIN) method\n Operation on Common STA-Enrollee side</b>\n 1.trigger the WPS PIN connection and we will get STA's 8 ditital PIN, such as "42205597" <br><b>Operation on MT7687 side</b></br>\n wifi config set opmode 2(set opmode as SoftAP mode)\n wifi wps trigger pin 1 any 42205597(input STA's PIN "42205597" and triger WPS PIN connection process).|
*
*/

/**
* @}
*/


/**@defgroup WIFI_COMMAND_CLI_4 WiFi_Smart_CLI_Usage
* @{
* @section WiFiCLI_Terms_Chapter4_1 Wi-Fi Smart CLI
*  <b>"Smart Connection Command" Cli </b>
*  |  Command Format                                    |   Usage Refer and Example                |  Description |
*  |---------------------------------------------- |------------------------------------- |--------------------------------|
*  | <b>smart connect <bm/m/b> <key></b>                |   @sa #smart_config_test_cli            | Set smart connection and start     |
*  | <b>smart stop</b>                                  |   @sa #smart_config_test_cli            | Stop smart connecting              |
*
*/

/**
* @}
*/


/**@defgroup WIFI_CLI_TABLE_1 WIFI_Cli_Tables
* @{
* @section WiFiCLI_Table1_Chapter1_1  Combinations of Auth Mode and Encrypt Type
*  | Combination Type |  Auth  Mode | Encrypt  Type |
*  |---------------------|- --|--------|
*  |OPEN                   |0     |1  |
*  |WPA2PSK(AES)           |7     |6  |
*  |WPA2PSK(TKIP)          |7	  |4  |
*  |WPA2PSK(AES+TKIP)      |7     |8  |
*  |WPAPSK(AES)            |4	  |6  |
*  |WPAPSK(TKIP)           |4	  |4  |
*  |WPAPSK+WPA2PSK(AES+TKIP)|9	|8|
*  |WEP(OPEN)              |0	  |0  | 
*
*/

/**
* @}
*/


#ifndef __WIFI_CLI_H__
#define __WIFI_CLI_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "cli.h"

/**
* @brief Example of Set WiFi Raw Packet Receiver
* @param [IN]enable 0: unregister, 1: register
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set rxraw <enable>
* \n <b>cli_param</b>:     enable: 0: unregister 1: register 
 *\n                       Set RX RAW packet
*
* \n <b>cli_example</b>:   wifi config set rxraw 1  
* \n <b>cli_note</b>:      Enable the pre-registered RX RAW packet handler, the 802.11 packet will be report to the  handler, and print part of raw content. \n This CLI can cooperate with another CLI:  wifi config set rxfilter ***  for the test and verification. 
*
*/
uint8_t wifi_config_set_rx_raw_pkt_ex(uint8_t len, char *param[]);



/**
* @brief send txraw packets
* @param  [IN] packets The 802.11 packets content that includes 802.11 MAC header + Payload, it does not include the FCS.
* @return  =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set txraw <raw_packets>
* \n <b>cli_param</b>:     raw_packets: 802.11 packets content that includes 802.11 MAC header + Payload, it does not include the FCS
*
* \n <b>cli_example</b>:   wifi config set txraw 48113C00D0C7C08200437C1DD97038BFD0C7C08200437001B3015194  
* \n <b>cli_note</b>:      Send a TX RAW packet(A 802.11 NULL packet) with basic rate (CCK, 1Mbps)
*
*/
uint8_t wifi_tx_raw_pkt_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get MAC address for STA/AP wireless port
* @param [IN]port  0: STA/APCLI, 1: AP
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get mac <port>
* \n <b>cli_param</b>:     port: 0: STA/APCLI, 1: AP
*
* \n <b>cli_example</b>:   wifi config get mac 0  
* \n <b>cli_note</b>:      Get the device's STA/APCLI port's mac address, 
* @note                    That there is no corresponding set mac cli because the MAC address cannot be modified dynamically. 
*
*/
uint8_t wifi_config_get_mac_address_ex(uint8_t len, char *param[]);

/**
* @brief Example of Configure bandwidth for STA/AP  wireless port.
* @param [IN]port  0: STA/APCLI, 1: AP
* @param [IN]bw   0: HT20, 1: HT20/40 coexistence
* @return >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set bw <port> <bandwidth>
* \n <b>cli_param</b>:     port: 0: STA/APCLI, 1: AP
* \n                       bandwidth: Bandwidth supported, 0: 20MHz, 1: 20/40MHz
*
* \n <b>cli_example</b>:   wifi config set bw 0 0  
* \n <b>cli_note</b>:      Set the device STA/APCLI port's bandwidth as 20MHz. \n The bandwidth needs to be limited and match the wireless mode settings.
*
*/
uint8_t wifi_config_set_bandwidth_ex(uint8_t len, char *param[]);

/**
* @brief Example of get bandwidth for STA/AP wireless port.
* @param [IN]port  0: STA/APCLI, 1: AP
* @return >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get bw <port>
* \n <b>cli_param</b>:     port: 0: STA/APCLI, 1: AP
* \n <b>cli_output</b>:    The current bandwidth
*
* \n <b>cli_example</b>:   wifi config get bw 0  
* \n <b>cli_note</b>:      Get the device STA/APCLI port's current bandwidth
*
*/
uint8_t wifi_config_get_bandwidth_ex(uint8_t len, char *param[]);

/**
* @brief Example of Configure bandwidth for STA/AP  wireless port.
*
* wifi config set bw_extended <port>  <0:HT20/1:HT40> <below_above_ch>
* @parameter
* @return >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set bw_extended <port> <bandwidth> <below_above_ch>
* \n <b>cli_param</b>:     port: 0: STA/APCLI, 1: AP
* \n                       bandwidth: Bandwidth supported, 0: 20MHz, 1: 20/40MHz
* \n                       below_above_ch:
*
* \n <b>cli_example</b>:   
* \n <b>cli_note</b>:      
*
*/
uint8_t wifi_config_set_bandwidth_extended_ex(uint8_t len, char *param[]);

/**
* @brief Example of get bandwidth for STA/AP wireless port.
*  wifi config get bw_extended <port>
* @parameter
* @return >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get bw_extended <port>
* \n <b>cli_param</b>:     port: 0: STA/APCLI, 1: AP
* \n <b>cli_output</b>:    The current externed bandwidth
*
* \n <b>cli_example</b>:   wifi config get bw_extended 0  
* \n <b>cli_note</b>:      Get the device STA/APCLI port's current externed bandwidth
*
*/
uint8_t wifi_config_get_bandwidth_extended_ex(uint8_t len, char *param[]);

/**
* @brief Example of Configure channel for STA/AP wireless port. STA will keep idle and stay in channel specified
* @param [IN]port  0: STA/APCLI, 1: AP
* @param [IN]ch  1~14 are supported for 2.4G only product, channel 36~165 for 5G only
* @return >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set ch <port> <ch>
* \n <b>cli_param</b>:     port: 0: STA/APCLI, 1: AP
* \n                       ch: 2.4G: channel 1~14, 5G: channel 36~165. please refer to #wifi_config_set_country_region 
*
* \n <b>cli_example</b>:   wifi config set ch 0 1  
* \n <b>cli_note</b>:      Set the device STA/APCLI port's current channel as 1. \n The channel needs to be limited and match the country region/country code/wireless mode setting.
*
*/
uint8_t wifi_config_set_channel_ex(uint8_t len, char *param[]);

/**
* @brief Example of get the current channel for STA/AP wireless port.
* @param [IN]port  0: STA/APCLI, 1: AP
* @return >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get ch <port>
* \n <b>cli_param</b>:     port: 0: STA/APCLI, 1: AP
* \n <b>cli_output</b>:    The current channel, please refer to #wifi_config_set_country_region
*
* \n <b>cli_example</b>:   wifi config get ch 0  
* \n <b>cli_note</b>:      Get the device STA/APCLI port's current channel.
*
*/
uint8_t wifi_config_get_channel_ex(uint8_t len, char *param[]);

/**
* @brief Example of configure rx filter for packets wanted to be received
* @param [IN]flag defined in  #wifi_rx_filter_t
* @return >=0 means success, <0 means fail
* @note Default value will be WIFI_DEFAULT_IOT_RX_FILTER
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set rxfilter <flag>
* \n <b>cli_param</b>:     flag: Configure packet format which to be receivedm, flag defined in #wifi_rx_filter_t of wifi api reference
*
* \n <b>cli_example</b>:   wifi config set rxfilter 0x1de00a  
* \n <b>cli_note</b>:      Set the filter to drop specific packets.\n bit 1 Drops the FCS error frames.\n bit 3 Drops the version field of Frame Control field.\n  More @sa Please refer #wifi_rx_filter_t.
*
*/
uint8_t wifi_config_set_rx_filter_ex(uint8_t len, char *param[]);

/**
* @brief Example of get rx filter for packets format wanted to be received
* @param [OUT]flag defined in  #wifi_rx_filter_t
* @return >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get rxfilter
* \n <b>cli_output</b>:    Flag defined in  #wifi_rx_filter_t of wifi api reference
*
* \n <b>cli_example</b>:   wifi config get rxfilter  
* \n <b>cli_note</b>:      Get current rx filter settings.
*
*/
uint8_t wifi_config_get_rx_filter_ex(uint8_t len, char *param[]);

/**
* @brief Example of set the authentication mode and encryption mode for the specified STA/AP port
* @param [IN]authmode
*           1 WPAPSK
*           2 WPA2PSK
*           3 WPA1PSKWPA2PSK
* @param  [IN] encryption method index:
*           1 AES
*           2 TKIP
*           3 TKIPAES
*
* @return =0 means success, >0 means fail
* 
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set sec <port> <auth_mode> <encrypt_type>
* \n <b>cli_param</b>:     port: 0: STA/APCLI;  1: AP
* \n                       auth_mode:
* \n                       encrypt_type:
* \n                       Detail authentication mode and encryption mode info please refer to section table <b>Combinations of Auth Mode and Encrypt Type</b>. @sa Refer to #wifi_auth_mode_t ,  #wifi_encrypt_type_t 
*
* \n <b>cli_example 1</b>: wifi config set sec 1 0 1 
* \n <b>cli_note</b>:      Set the device AP port's security as Open mode.
* \n <b>cli_example 2</b>: wifi config set sec 0 7 6 
* \n <b>cli_note</b>:      Set the device STA/APCLI port's security setting as WPA2-PSK AES mode. The device must connect the AP with same security setting.
*
*/
uint8_t wifi_config_set_security_mode_ex(uint8_t len, char *param[]);

/**
* @brief Example of get the authentication mode for the specified STA/AP port
* @param [OUT]authmode method index:
*           1 WPAPSK
*           2 WPA2PSK
*           3 WPA1PSKWPA2PSK
* @param [OUT] encryption method index:
*           1 AES
*           2 TKIP
*           3 TKIPAES
*
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get sec <port>
* \n <b>cli_param</b>:     port: 0: STA/APCLI  1: AP
* \n <b>cli_output</b>:    Get the authentication mode and encryption mode. Detail authentication mode and encryption mode info please refer to section table <b>Combinations of Auth Mode and Encrypt Type</b>  @sa Refer to #wifi_auth_mode_t ,  #wifi_encrypt_type_t
*
* \n <b>cli_example</b>:   wifi config get sec 0  
* \n <b>cli_note</b>:      Get the device STA/APCLI's current security setting. 
*
*/
uint8_t wifi_config_get_security_mode_ex(uint8_t len, char *param[]);

/**
* @brief Example of set the psk for the specified STA/AP port
* @param [IN]passphrase 8 ~ 63 bytes ASCII or 64 bytes Hex
*
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set psk <port> <password>
* \n <b>cli_param</b>:     port: 0: STA/APCLI;  1: AP
* \n                       password: 8 ~ 63 bytes ASCII or 64 bytes Hex
*
* \n <b>cli_example</b>:   wifi config set psk 0 12345678 
* \n <b>cli_note</b>:      Set the device STA/APCLI port's password as "12345678", the device will use this password to connect target AP. \n If the password has 64 character, it will be treated as PMK which is the 64 HEX value.
*
*/
uint8_t wifi_config_set_psk_ex(uint8_t len, char *param[]);

#if 0 //Add new cli to set authentication mode and psk separately.
/**
* @brief Example of Set the authentication mode for the specified STA/AP port
* <br><b>
* wifi config set psk <port> <auth> <encrypt> <passphrase>
* </b></br>
*
* @param [IN]port  0: STA/APCLI, 1: AP
* @param [IN]auth authentication mode
*           1: WPAPSK,
*           2: WPA2PSK,
*           3: WPA1PSKWPA2PSK
* @param  [IN]encrypt encryption method index:
*           1: AES,
*           2: TKIP,
*           3: TKIPAES
* @param [IN]passphrase 8 ~ 63 bytes ASCII or 64 bytes Hex
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
uint8_t wifi_config_set_psk_ex(uint8_t len, char *param[]);
#endif

/**
* @deprecated This function is deprecated!It can be replaced by #wifi_config_set_psk_ex. Example of Set PMK for the specified STA/AP port
* @param [IN]port  0: STA/APCLI / AP Client, 1: AP
* @param [IN]PMK (in hex)
*       00, 05, 30, ......(size 32)
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set pmk <port> <PMK>
* \n <b>cli_param</b>:     port: 0: STA/APCLI;  1: AP
* \n                       PMK: Hex:  00, 05, 03,...(size 32)
*
* \n <b>cli_note</b>:      Set PMK for the specified STA/AP port. It is replaced by psk API/CLI.
*
*/
uint8_t wifi_config_set_pmk_ex(uint8_t len, char *param[]);

#if 0 //Add new cli to get authentication mode and psk separately.
/**
* @brief Example of Get the authentication mode for the specified STA/AP port
* <br><b>
* wifi config get psk <port>
* </b></br>
* @param [IN]port  0: STA/APCLI / AP Client, 1: AP
* @param [OUT]authmode
*           1: WPAPSK,
*           2: WPA2PSK,
*           3: WPA1PSKWPA2PSK
* @param  [OUT] encryption method index:
*           1: AES,
*           2: TKIP,
*           3: TKIPAES
* @param [OUT]passphrase 8 ~ 63 bytes ASCII or 64 bytes Hex
* @param [OUT]passphrase_len 8 ~ 64
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
uint8_t wifi_config_get_psk_ex(uint8_t len, char *param[]);
#endif

/**
* @brief Example of get the psk for the specified STA/AP port
@param [IN]passphrase 8 ~ 63 bytes ASCII or 64 bytes Hex
*
* @return =0 means success, >0 means fail
* 
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get psk <port>
* \n <b>cli_param</b>:     port: 0: STA/APCLI/AP Client, 1: AP
* \n <b>cli_output</b>:    Get the current password.
*
* \n <b>cli_example</b>:   wifi config get psk 0 
* \n <b>cli_note</b>:      Get the device STA/APCLI port's current password. 
*
*/
uint8_t wifi_config_get_psk_ex(uint8_t len, char *param[]);


/**
* @deprecated This function is deprecated!It can be replaced by #wifi_config_get_psk_ex. Example of Get PMK for the specified STA/AP port
* @param [IN]port  0: STA/APCLI / AP Client, 1: AP
* @param [OUT]PMK (in hex)
*       00, 05, 30, ......(size 32)
* @return >=0 means success, <0 means fail
* @note Default to OPEN
* 
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get pmk <port>
* \n <b>cli_param</b>:     port: 0: STA/APCLI/AP Client, 1: AP
* \n <b>cli_output</b>     pmk Hex:  00, 05, 03 ,...(size 32)
*
* \n <b>cli_example</b>:   wifi config get pmk 0 
* \n <b>cli_note</b>:      Get the PMK information of the specified STA/AP port. It is replaced by psk API/CLI. 
*
*/
uint8_t wifi_config_get_pmk_ex(uint8_t len, char *param[]);

/**
* @brief Example of get WiFi WEP Keys
* @param [IN]port  0: STA/APCLI / AP Client, 1: AP
* @param [OUT]wifi_wep_key_t
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get wep <port> <key_id>
* \n <b>cli_param</b>:     port: 0: STA/APCLI;  1: AP
* \n                       key_id: 0~3 
* \n <b>cli_output</b>:    Get the key string of specified key index
*
* \n <b>cli_example</b>:   wifi config get wep 0 0
* \n <b>cli_note</b>:      Get the key string of the key index 0 for the device's STA/APCLI port.
*
*/
uint8_t wifi_config_get_wep_key_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi WEP Keys
* @param [IN]port  0: STA/APCLI / AP Client, 1: AP
* @param [IN]key_id 0~3
* @param [IN]key_string 0~26 (ASCII length: 5 or 13; Hex length:10 or 26)
* @return >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set wep <port> <key_id> <key_string_id>
* \n <b>cli_param</b>:     port: 0: STA/APCLI;  1: AP
* \n                       key_id: 0~3 
* \n                       key_string_id: The key string of specified id
*
* \n <b>cli_example 1</b>: wifi config set wep 0 0 1234567890
* \n <b>cli_note</b>:      Set the device STA/APCLI port's WEP key index 0's value as "0x12 0x34 0x56 0x78 0x90".
* \n <b>cli_example 2</b>: wifi config set wep 0 0 12345
* \n <b>cli_note</b>:      Set the device STA/APCLI port's WEP key index 's value as "0x31 0x32 0x33 0x34 0x35.
* \n                       The wep key string length should be 5 or 10 or 13 or 26. Key string with 5 or 13 length be treated as ASCLL, Key string with 10 or 26 length be treated as Hex.
*
*/
uint8_t wifi_config_set_wep_key_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi Operation Mode.
* @param [OUT]mode
*        1: WIFI_MODE_STA_ONLY,
*        2: WIFI_MODE_AP_ONLY,
*        3: WIFI_MODE_REPEATER
*        4: WIFI_MODE_MONITOR
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get opmode
* \n <b>cli_output</b>:    1: STA  mode;  2: AP mode;  3: Repeater mode 4: Monitor mode
*
* \n <b>cli_example</b>:   wifi config get opmode
* \n <b>cli_note</b>:      Get the current operation mode.
*
*/
uint8_t wifi_config_get_opmode_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi Operation Mode.
* @param [IN]mode
*       1: WIFI_MODE_STA_ONLY,
*       2: WIFI_MODE_AP_ONLY,
*       3: WIFI_MODE_REPEATER
*       4: WIFI_MODE_MONITOR
* @return  >=0 means success, <0 means fail
* @note Set WiFi Operation Mode will RESET all the configuration set by previous WIFI-CONFIG APIs
* 
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set opmode <mode>
* \n <b>cli_param</b>:     mode:  1: STA mode;  2: AP mode;  3: Repeater mode 4: Monitor mode
*
* \n <b>cli_example</b>:   wifi config set opmode 1 
* \n <b>cli_note</b>:      Set the device's current operation mode as STA mode.
*
*/
uint8_t wifi_config_set_opmode_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi SSID.
* @param [IN]port  0: STA/APCLI/AP Client,  1: AP
* @param [OUT]ssid SSID
* @param [OUT]ssid_len Length of SSID
* @return  >=0 means success, <0 means fail
* 
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get ssid <port>
* \n <b>cli_param</b>:     port: 0: STA/APCLI  
* \n <b>cli_output</b>:    The ssid for the specific port
*
* \n <b>cli_example 1</b>: wifi config get ssid 0  
* \n <b>cli_note</b>:      Get the device's STA or APCLI port's  current SSID
* \n <b>cli_example 2</b>: wifi config get ssid 1
* \n <b>cli_note</b>:      Get current SSID for AP port
*
*/
uint8_t wifi_config_get_ssid_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi SSID.
* @param [IN]port  0: STA/APCLI / AP Client, 1: AP
* @param [IN]ssid SSID
* @param [IN]ssid_len Length of SSID
* @return  >=0 means success, <0 means fail
* 
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set ssid <port> <ssid>
* \n <b>cli_param</b>:     port: 0: STA/APCLI;  1: AP
* \n                       ssid: The ssid of target AP
*
* \n <b>cli_example</b>:   wifi config set ssid 0 APRouter 
* \n <b>cli_note</b>:      Set the device STA/APCLI port's ssid named as "APRouter". The device will connect the AP which has the same SSID,  when the device is doing wifi connection process. 
*
*/
uint8_t wifi_config_set_ssid_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi BSSID.
* @param [OUT]bssid BSSID
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get bssid
* \n <b>output</b>:        Target AP's Wi-Fi BSSID
*
* \n <b>cli_example</b>:   wifi config get bssid 
* \n <b>cli_note</b>:      Get the current Wi-Fi BSSID which the device used to connect to AP, or the AP's BSSID which the device is connecting.
*
*/
uint8_t wifi_config_get_bssid_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi SSID.
* @param [IN]bssid BSSID
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set bssid <BSSID>
* \n <b>cli_param</b>:     BSSID: target AP's Wi-Fi BSSID
*
* \n <b>cli_example</b>:   wifi config set bssid 00:0c:45:56:78:90 
* \n <b>cli_note</b>:      Set the target AP's BSSID when device is STA mode, the device will connect the AP which has the same BSSID, when the device doing wifi connection process. 
*
*/
uint8_t wifi_config_set_bssid_ex(uint8_t len, char *param[]);

/**
* @brief Example of Reload configuration
* @return  >=0 means success, <0 means fail
* 
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set reload
* \n <b>cli_output</b>:    None.  Reload configuration
*
* \n <b>cli_example</b>:   wifi config set reload 
* \n <b>cli_note</b>:      Reload the wifi configuration, and make it take effect, such as: ssid, psk, sec, pmk, wep and so on.
*
*/
uint8_t wifi_config_set_reload_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get MTK Smart Connection Filter
* @param [OUT]flag
* @return  >=0 means success, <0 means fail
* 
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get mtksmart 
* \n <b>cli_output</b>:    The value of mtksmart flag
*
* \n <b>cli_example</b>:   wifi config get mtksmart 
* \n <b>cli_note</b>:      Get the current mtk smart connection  filter status value..
*
*/
uint8_t wifi_config_get_mtk_smartconnection_filter_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set MTK Smart Connection Filter
* @param [IN]flag  0: Disable, 1: Enable
* @return  >=0 means success, <0 means fail
* 
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set mtksmart <flag>
* \n <b>cli_param</b>:     flag: 0: disable;  1: enable
* \n <b>cli_output</b>:    Set MTK Smart Connection Filter
*
* \n <b>cli_example</b>:   wifi config set mtksmart 1 
* \n <b>cli_note</b>:      Enable mtk smart connection Filter.
*
*/
uint8_t wifi_config_set_mtk_smartconnection_filter_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi Radio ON/OFF
* @param [IN]onoff  0: OFF, 1: ON
* @return  >=0 means success, <0 means fail
* @note in MODE_Dual, both WiFi interface radio will be turn on/off at the same time
* 
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set radio <on_off>
* \n <b>cli_param</b>:     on_off:  0: OFF;  1: ON
*
* \n <b>cli_example</b>:   wifi config set radio 1 
* \n <b>cli_note</b>:      WIFI interface radio will be turn on.
* @note                    In dual mode, both WIFI interface radio will be turn on/off at the same time.
*
*/
uint8_t wifi_config_set_radio_on_ex(uint8_t len, char *param[]);

/**
* @brief Example of get WiFi Radio ON/OFF
* @param [OUT]onoff 0: OFF, 1: ON
* @return  >=0 means success, <0 means fail
* @note in MODE_Dual, both WiFi interface radio will be turn on/off at the same time
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get radio
* \n <b>output</b>         0: OFF;  1: ON 
* 
* \n <b>cli_example</b>:   wifi config get radio
* \n <b>cli_note</b>:      Get the current status of WIFI interface radio status.
*
*/
uint8_t wifi_config_get_radio_on_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi Wireless Mode
* @param [OUT] wirelessmode, @sa Refer to #wifi_phy_mode_t
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get wirelessmode <port>
* \n <b>cli_param</b>:     port: 0: STA/APCLI;  1: AP
* \n <b>output</b>         The current wirelessmode ,@sa Refer to #wifi_phy_mode_t
* 
* \n <b>cli_example</b>:   wifi config get wirelessmode 0
* \n <b>cli_note</b>:      Get the device STA/APCLI port's wireless mode.
*
*/
uint8_t wifi_config_get_wireless_mode_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi Wireless Mode
* @param [IN]wirelessmode, @sa Refer to #wifi_phy_mode_t
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set wirelessmode <port> <mode>
* \n <b>cli_param</b>:     port: 0: STA/APCLI;  1: AP
* \n                       mode: 0~11  @sa Refer to #wifi_phy_mode_t  and table <b>WiFi Physical Mode Enumeration</b>
* 
* \n <b>cli_example</b>:   wifi config set wirelessmode 0 0
* \n <b>cli_note</b>:      Set the current wirless mode as 11BG mixed mode.
*
*/
uint8_t wifi_config_set_wireless_mode_ex(uint8_t len, char *param[]);

/**
* @deprecated This function is deprecated!It can be replaced by #wifi_config_get_country_code_ex. Example of Get WiFi Country Region
* @param [IN]band  0: 2.4G, 1: 5G
* @param [OUT]region  @sa #wifi_config_set_country_region
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get country <band>
* \n <b>cli_param</b>:     band: 0: 2.4G;  1: 5G
* \n <b>output</b>:        The country region info. @sa Refer to #wifi_config_get_country_region
* 
* \n <b>cli_example</b>:   wifi config get country 0
* \n <b>cli_note</b>:      Get 2.4G Band country region value.
* \n <b>cli_example</b>:   wifi config get country 1 
* \n <b>cli_note</b>:      Get 5G Band country region value.
*
*/
uint8_t wifi_config_get_country_region_ex(uint8_t len, char *param[]);

/**
* @deprecated This function is deprecated!It can be replaced by #wifi_config_set_country_code_ex. Example of Set WiFi Country Region
* @param [IN]band  0: 2.4G, 1: 5G
* @param [IN]region band=0, region range 0-7,31-33; \n band=1,region range0-22,30-37,  @sa #wifi_config_set_country_region
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set country <band> <region>
* \n <b>cli_param</b>:     band: 0: 2.4G;  1: 5G
* \n                       region: the region value ,Refer to @sa #wifi_config_set_country_region 
* 
* \n <b>cli_example</b>:   wifi config set country 0 1
* \n <b>cli_note</b>:      Set 2.4G band country Region as 1. 
* \n                       Details of country region and channel range of 2.4G band or 5G band, Refer to @sa #wifi_config_set_country_region.
*
*/
uint8_t wifi_config_set_country_region_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set Country Code
* @param [IN]country_code 2 chars country code
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set country_code <country_code>
* \n <b>cli_param</b>:     country_code: The country code that the driver uses. For more information please refer to wifi_country_code_t in Wi-Fi API reference guide.
* 
* \n <b>cli_example</b>:   wifi config set country_code CN
* \n <b>cli_note</b>:      Only support country code: CN, US, UK for reference design 
*
*/
uint8_t wifi_config_set_country_code_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get Country Code
* @param  [OUT]country_code country code and list
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get country_code
* \n <b>cli_output</b>:    The code region info.
* 
* \n <b>cli_example</b>:   wifi config get country_code
* \n <b>cli_note</b>:      Get the country code information. 
*
*/
uint8_t wifi_config_get_country_code_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi DTIM Interval
* @param  [OUT]interval: 1~255
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get dtim
* \n <b>cli_output</b>:    DTIM interval(range is 1~255)
* 
* \n <b>cli_example</b>:   wifi config get dtim
* \n <b>cli_note</b>:      Get device's current wifi DTIM interval value..
*
*/
uint8_t wifi_config_get_dtim_interval_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi DTIM Interval
* @param [IN]interval 1 ~ 255
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set dtim <dtim_interval>
* \n <b>cli_param</b>:     dtim_interval: 1~255
* 
* \n <b>cli_example</b>:   wifi config set dtim 1
* \n <b>cli_note</b>:      set device's wifi DTIM interval value as 1, only used when device is AP mode.
*
*/
uint8_t wifi_config_set_dtim_interval_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi Listen Interval
* @param  [OUT]interval: 1~255
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config get listen
* \n <b>cli_output</b>:    listen interval(range is 1~255).
* 
* \n <b>cli_example</b>:   wifi config get listen
* \n <b>cli_note</b>:      Get device's current wifi listen interval value. 
*
*/
uint8_t wifi_config_get_listen_interval_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi Listen Interval
* @param [IN]interval 1 ~ 255
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi config set listen <listen_interval>
* \n <b>cli_param</b>:     listen_interval: 1~255.
* 
* \n <b>cli_example</b>:   wifi config set listen 2
* \n <b>cli_note</b>:      Set device's wifi listen interval value as 2, only used when device is STA mode. 
*
*/
uint8_t wifi_config_set_listen_interval_ex(uint8_t len, char *param[]);



/* ---------------------- Connect Ex ----------------------------*/




/**
* @brief Example of  connect / link up to specifed AP
* @param [IN] link  0: link down, 1: link up
* @return >=0 means success, <0 means fail
* @note Be sure to configure security setting before connect to AP
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi connect set connection <linkup/down>
* \n <b>cli_param</b>:     link: 0: link down (wifi connection disconnect ap).  1: link up (wifi config reload setting).
* 
* \n <b>cli_example</b>:   wifi connect set connection 0 
* \n <b>cli_note</b>:      Disconnect to the AP , only used in STA mode.
*
*/
uint8_t wifi_connect_set_connection_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get the current STA port link up / link down status of the connection
* @param [out] link  0: link down, 1: link up
* @return >=0 means success, <0 means fail
* @note WIFI_STATUS_LINK_DISCONNECTED indicates STA may in IDLE/ SCAN/ CONNECTING state
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi connect get linkstatus
* \n <b>cli_output</b>:    0: disconnected 1: connected.
* 
* \n <b>cli_example</b>:   wifi connect get linkstatus
* \n <b>cli_note</b>:      Return the link status , only for STA mode. 
*
*/
uint8_t wifi_connect_get_link_status_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi Associated Station List
* @param [OUT]station_list
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi connect get stalist
* \n <b>cli_output</b>:    Station list information
* 
* \n <b>cli_example</b>:   wifi connect get stalist
* \n <b>cli_note</b>:      Return the number of associated stations and the stations information. Only for AP mode.
*
*/
uint8_t wifi_connect_get_station_list_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get the max number of the supported stations in ap mode or Repeater Mode.
* @param [OUT] number The max number of supported stations will be returned
*
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi connect get max_num_sta
* \n <b>cli_output</b>:    Max station link supported
* 
* \n <b>cli_example</b>:   wifi connect get max_num_sta 
* \n <b>cli_note</b>:      Get the maximum number of supported stations in AP mode or Repeater mode.
*
*/
uint8_t wifi_connect_get_max_station_number_ex(uint8_t len, char *param[]);

/**
* @brief Example of deauth some WiFi connection
* @param [IN]addr STA MAC Address
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi connect set deauth <MAC>
* \n <b>cli_param</b>:     MAC: the station's mac address want to disconnect
* 
* \n <b>cli_example</b>:   wifi connect set deauth 00:0c:ae:80:03:01
* \n <b>cli_note</b>:      Disconnect the station with this specify MacAdderss, only used in AP mode.
*
*/
uint8_t wifi_connect_deauth_station_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get rssi of the connected AP
* @param [OUT]rssi
* @return >=0 means success, the status will be:
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi connect get rssi
* \n <b>cli_output</b>:    Get rssi of the connected AP
* 
* \n <b>cli_example</b>:   wifi connect get rssi 
* \n <b>cli_note</b>:      Get rssi of the connected AP. Only used for STA mode and the station has connected to the AP.
*
*/
uint8_t wifi_connect_get_rssi_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get inband queue spaces
* @param [OUT]inband queue spaces
* @return >=0 means success, the status will be:
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi mq get iq
* \n <b>cli_output</b>:    Get inband queue spaces
* 
* \n <b>cli_example</b>:    
* \n <b>cli_note</b>:      
*
*/
uint8_t wifi_mq_get_iq_ex(uint8_t len, char *param[]);

/**
* @brief Example of Register WiFi Event Notifier
* @param [IN]enable 0: unregister, 1: register
* @param [IN]event_id Event ID. More Event ID @sa #wifi_event_t
* @return  >=0 means success, <0 means fail
*
* \n Example of cli usage [Internal Use]
* \n <b>cli_format</b>:    wifi connect set eventcb <enable> <enable_ID>
* \n <b>cli_param</b>:     enable: 0:register 1:unregister
* \n                       enable_ID: 0: link up event 1:scan complete event .More Event ID @sa #wifi_event_t.
* 
* \n <b>cli_example</b>:   wifi connect set eventcb 1 0 
* \n <b>cli_note</b>:      Register the example handler for connected event, when the event is reported,the handler will be triggered. 
*
*/
uint8_t wifi_connect_set_event_callback_ex(uint8_t len, char *param[]);

/**
* @brief start scan with options
* @param [IN] start/stop  0: stop, 1: start
* @param [IN] scan mode  0: full, 1: partial
* @param [IN] scan_option  0: active, 1: passive in all channel, 2:force active
* @param [IN] ssid   "NULL" means not specified
* @param [IN] bssid   "NULL" means not specified
*
* @return =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format 1</b>:   wifi connect set scan <start> <scan mode> <scan_option> [ssid] [bssid]
* \n <b>cli_format 2</b>:   wifi connect set scan <stop>
* \n <b>cli_param</b>:     start/stop: 0:stop,  1:start 
* \n                       scan_mode:  0:full; 1:partial
* \n                       scan_option:  0: active(passive in regulatory channel) ,1: passive in all channel 2: force active(active in all channel)
* \n                       ssid: "NULL" means not specified
* \n                       bssid: "NULL" means not specified 
* 
* \n <b>cli_example</b>:  wifi connect set scan 1 0 0
* \n <b>cli_note</b>:     Start do scan. it is better to set CLI:\n wifi connect set eventcb 1 1 \n to register scan complete event handler,  then the scan result shall be display after running above two CLIs. 
* @note                   when ssid/bssid is specified(not NULL) \n 1) ProbeReq carries ssid(for hidden AP)\n 2) Scan table always keeps the APs of this specified ssid/bssid.  
*
*/
uint8_t wifi_connect_set_scan_ex(uint8_t len, char *param[]);


/*WIFI CLI ENTRY definiation*/

#define WIFI_PUB_CLI_ENTRY          { "wifi",    "wifi api",     NULL,   wifi_pub_cli },

#if defined(MTK_SMTCN_V5_ENABLE) || defined(MTK_SMTCN_V4_ENABLE)
#define WIFI_PUB_CLI_SMNT_ENTRY     { "smart",   "smart connection", smart_config_test_cli },
#else
#define WIFI_PUB_CLI_SMNT_ENTRY
#endif


/*extern function*/
#if defined(MTK_SMTCN_V5_ENABLE) || defined(MTK_SMTCN_V4_ENABLE)

/**
* @brief Example of smart connect config
* @param [IN] cmd
* @param [IN] option
* @return  =0 means success, >0 means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    smart connect <bm/m/b> <key>
* \n <b>cli_param</b>:     type: b: broadcast (V5), m:multicast (V4), bm:V4 and V5
* \n                       key: 16 bytes key string. This parameter is optional. User can use it to encrypt the smart connection data.
* 
* \n <b>cli_example 1</b>: smart connect bm 
* \n <b>cli_note</b>:      start smart connection both V5, V4 protocol. 
* \n <b>cli_example 2</b>: smart connect b 
* \n <b>cli_note</b>:      start smart connection V5 protocol only. 
* \n <b>cli_example 3</b>: smart connect m
* \n <b>cli_note</b>:      start smart connection V4 protocol only.. 
* \n <b>cli_example 4</b>: smart connect
* \n <b>cli_note</b>:      start smart connection both V5, V4 protocol. 
*
* \n Example of cli usage
* \n <b>cli_format</b>:    smart stop
* \n <b>cli_output</b>:    Stop smart conenction
* 
* \n <b>cli_example</b>:   smart stop 
* \n <b>cli_note</b>:      stop smart connection. 
*
* \n Example of cli usage
* \n <b>cli_format</b>:    smart debug  <0/1>
* \n <b>cli_param</b>:     0: stop, 1: start
* 
* \n <b>cli_example</b>:   smart debug 0 
* \n <b>cli_note</b>:      stop smart connect debug. 
*
*/
uint8_t smart_config_test_cli(uint8_t len, char *param[]);
#endif
extern cmd_t   wifi_pub_cli[];
extern cmd_t   wifi_mq_get_cli[];


#ifdef MTK_WIFI_PROFILE_ENABLE
extern cmd_t   wifi_profile_set_cli[];
extern cmd_t   wifi_profile_get_cli[];
uint8_t wifi_profile_reset_ex(uint8_t len, char *param[]);
#endif


#if defined(MTK_WIFI_WPS_ENABLE)

/**
* @brief Example of Set default WPS device information, config default value inside the function
* @return =0 means success, else means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi wps set device_info <Device_Name> <Manufacturer> <Model_Name> <Model_number> <Serial_number>
* \n <b>cli_param</b>:     Device_Name: The name of the device
* \n <b>cli_param</b>:     Manufacturer: Manufacturer of the device
* \n <b>cli_param</b>:     Model_Name: Module name of the device
* \n <b>cli_param</b>:     Model_number: Module number of the device
* \n <b>cli_param</b>:     Serial_number: Serial number of the device
* 
* \n <b>cli_example</b>:   wifi wps set device_info "MTK IoT Demo" "MTK Demo" "MTK Wireless Device Demo" "MTK7687 Demo" "123456789"
* \n <b>cli_note</b>:        
*
*/
uint8_t wifi_wps_set_device_info_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set auto connection when Enrollee got WPS credential information from the target AP.
* @param [IN]onoff on: enable auto connection to target AP after WPS done,   off: disable auto connection.
* @return =0 means success, else means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi wps set auto_connection <on_off>
* \n <b>cli_param</b>:     on_off: on: enable , off: disable
* 
* \n <b>cli_example</b>:   wifi wps set auto_connection on
* \n <b>cli_note</b>:       The enrollee will automatically connect to the target AP obtained from the WPS credential information after WPS process is complete. 
*
*/
uint8_t wifi_wps_set_auto_connection_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WPS device information
* @return =0 means success, else means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi wps get device_info
* \n <b>cli_output</b>:    Get the WPS device information
* 
* \n <b>cli_example</b>:   wifi wps get device_info 
* \n <b>cli_note</b>:      Get the WPS device information. 
*
*/
uint8_t wifi_wps_get_device_info_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WPS done auto connection control status.
* @return =0 means success, else means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi wps get auto_connection
* \n <b>cli_output</b>:    1: on  0: off
* 
* \n <b>cli_example</b>:   wifi wps get auto_connection
* \n <b>cli_note</b>:      Get the current status of WPS auto connection
*
*/
uint8_t wifi_wps_get_auto_connection_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get 8-digital PIN code of AP or STA.
* @param [IN]port  0: STA/APCLI,   1:AP.
* @return =0 means success, else means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi wps get pin_code <port>
* \n <b>cli_output</b>:    port: 0: STA/APCLI;   1: AP
* 
* \n <b>cli_example</b>:   wifi wps get pin_code 0
* \n <b>cli_note</b>:      Get 8-digital PIN code of STA.
*
*/
uint8_t wifi_wps_get_pin_code_ex(uint8_t len, char *param[]);

/**
* @brief Example of trigger WPS PBC.
* @param [IN]port  0: STA/APCLI,   1:AP.
* @param [IN]BSSID any: no special BSSID,   xx:xx:xx:xx:xx:xx :special BSSID.
* @return =0 means success, else means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi wps trigger pbc <port> <BSSID>
* \n <b>cli_param</b>:     port: 0: STA/APCLI;   1: AP
* \n                       BSSID: any: no special AP BSSID ,\n xx:xx:xx:xx:xx:xx :48 bit AP BSSID address
* 
* \n <b>cli_example</b>:   Example_STA
* \n <b>cli_example</b>:   (1) wifi wps trigger pbc 0 any \n Trigger STA WPS PBC with any triggered WPS PBC AP
* \n <b>cli_example</b>:   (2) wifi wps trigger pbc 0 01:02:03:04:05:06 \n STA WPS PBC with BSSID=01:02:03:04:05:06 WPS PBC AP
* \n <b>cli_example</b>:   Example_AP
* \n <b>cli_example</b>:   (1) wifi wps trigger pbc 1 any \n Ttrigger AP WPS PBC.  AP will ingore BSSID, so please only input "any"
*
*/
uint8_t wifi_wps_connection_by_pbc_ex(uint8_t len, char *param[]);


/**
* @brief Example of trigger WPS PIN.
* @param [IN]port  0: STA/APCLI,   1:AP.
* @param [IN]BSSID any: no special BSSID,   xx:xx:xx:xx:xx:xx :special BSSID.
* @param [IN]PIN_code:   8-digital PIN code.(for example: PIN code can be got from wifi_wps_connection_by_pin_ex())
* @return =0 means success, else means fail
*
* \n Example of cli usage
* \n <b>cli_format</b>:    wifi wps trigger pin <port> <BSSID> <PIN_code>
* \n <b>cli_param</b>:     port: 0: STA/APCLI;   1: AP
* \n <b>cli_param</b>:     BSSID: any: no special AP BSSID, xx:xx:xx:xx:xx:xx :48 bit AP BSSID address
* \n <b>cli_param</b>:     PIN_code:Support both Static PIN code and Dynamic PIN code input. Dynamic PIN code is generated by cli command "wifi wps get pin_code".
* 
* \n <b>cli_example</b>:   Example_STA
* \n <b>cli_example</b>:   (1) wifi wps trigger pin 0 any  XXXXXXXX:\n "XXXXXXXX" instead a 8-digital PIN code got from CLI "wifi wps get pin_code", The  STA do WPS PIN connection with any triggered WPS PIN AP. On AP side, we need input this PIN  in it.
* \n <b>cli_example</b>    (2) wifi wps trigger pin 0 x:xx:xx:xx:xx:xx XXXXXXXX:\n STA do WPS PIN use "XXXXXXXX" PIN code with BSSID="xx:xx:xx:xx:xx:xx" AP.
* \n <b>cli_example</b>:   Example_AP
* \n <b>cli_example</b>:   (1) wifi wps trigger pin 1 any XXXXXXXX:\n AP will ignore BSSID, so please only input "any". Then AP will do WPS PIN connection with the STA's PIN code="XXXXXXXX".
*
*/
uint8_t wifi_wps_connection_by_pin_ex(uint8_t len, char *param[]);

#endif/*MTK_WIFI_WPS_ENABLE*/


#ifdef MTK_WIFI_MIB_ENABLE
/**
 * @brief Show Wi-Fi MIB counters.
 * @return 0 means success, else means fail
 *
 * \n Example of cli usage [Internal Use]
 * \n <b>cli_format</b>:    wifi mib
 * \n                      Show Wi-Fi MIB counters.
 * 
 * \n <b>cli_example</b>:  wifi mib  
 * \n <b>cli_note</b>:     Show Wi-Fi MIB counters. 
 *
 */
uint8_t wifi_mib_show(uint8_t len, char *param[]);
#endif //MTK_WIFI_MIB_ENABLE


#if (PRODUCT_VERSION == 5932)
#define MTK_CLI_SLIM_FOR_32
#endif

#ifdef __cplusplus
}
#endif

/**
*@}
*/
#endif /*  __WIFI_CLI_H__ */
