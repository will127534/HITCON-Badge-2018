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

#ifndef __WIFI_PRIVATE_H__
#define __WIFI_PRIVATE_H__

#include "stdint.h"
#include "type_def.h"
#include "wifi_api.h"
#include "wifi_inband.h"
#include "connsys_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_IP_MODE_STATIC   0
#define WIFI_IP_MODE_DHCP     1

/** @brief Indicates the wlan statistic information.
*/
typedef struct {
    uint32_t Current_Temperature;  /**< Temperature of the device.*/
    uint32_t Tx_Success_Count;     /**< No of TX success, value is reset upon read.*/
    uint32_t Tx_Retry_Count;       /**< No of packet retries, value is reset upon read.*/
    uint32_t Tx_Fail_Count;        /**< No of TX Fails, value is reset upon read.*/
    uint32_t Rx_Success_Count;     /**< No of RX success, value is reset upon read.*/
    uint32_t Rx_with_CRC;          /**< No of FCS Errors, value is reset upon read.*/
    uint32_t Rx_Drop_due_to_out_of_resources;/**< No of RX drops due to FIFO Full. Value is reset upon read.*/
    uint32_t MIC_Error_Count;      /**< No of MIC Errors, value is reset upon read.*/
    uint32_t Cipher_Error_Count;   /**< No of Cipher errors, value is reset upon read.*/
    int8_t   Rssi;                 /**< Average RSSI of Data Packets.*/
    uint32_t Tx_AGG_Range_1;       /**< A-MPDU count with Aggregate Size in the range 1. Value is reset upon read.*/
    uint32_t Tx_AGG_Range_2;       /**< A-MPDU count with Aggregate Size in the range 2. Value is reset upon read.*/
    uint32_t Tx_AGG_Range_3;       /**< A-MPDU count with Aggregate Size in the range 3. Value is reset upon read.*/
    uint32_t Tx_AGG_Range_4;       /**< A-MPDU count with Aggregate Size in the range 4. Value is reset upon read.*/
    uint32_t AMPDU_Tx_Success;     /**< No of AMPDU TX Success, value is reset upon read.*/
    uint32_t AMPDU_Tx_Fail;        /**< No of AMPDU TX Failure, value is reset upon read.*/
    int32_t  SNR;                  /**< Average SNR from Data packets.*/
    uint16_t MAX_PHY_Rate;         /**< Max_phy_rate, value is reset upon read*/
    uint16_t REAL_TX_PHY_Rate;     /**< Real_Tx_phy_rate, value is reset upon read*/
    uint16_t REAL_RX_PHY_Rate;     /**< Real_Rx_phy_rate */
    uint8_t  BBPCurrentBW;         /**< BW_20=  0  , BW_40=  1*/
    uint8_t  REAL_TX_PHY_Mode;     /**< TX Mode , indicate the transmission mode 0: Legacy CCK, 1: Legacy OFDM, 2: HT Mixed mode, 3: HT Green field mode */
    uint8_t  REAL_RX_PHY_Mode;     /**< TX Mode , indicate the transmission mode 0: Legacy CCK, 1: Legacy OFDM, 2: HT Mixed mode, 3: HT Green field mode */
    uint8_t  REAL_TX_ShortGI;      /**< 0:Normal GI (800) , 1:Short GI (400) */
    uint8_t  REAL_RX_ShortGI;      /**< 0:Normal GI (800) , 1:Short GI (400) */
    uint8_t  REAL_TX_MCS;          /**< MCS 0 to 7 */
    uint8_t  REAL_RX_MCS;          /**< MCS 0 to 7 */
} wifi_statistic_t;


typedef struct {
    uint8_t ack_required;                   /* 0/1: Disable/Enable ACK */
    uint8_t rate_fixed;                     /* 0/1: Disable/Enable Fixed rate */
    uint8_t tx_mode;                        /* 0: MODE_CCK, 1: MODE_OFDM, 2: MODE_HTMIX, 3: MODE_HTGREENFIELD, 4: MODE_VHT  */
    uint8_t mcs;                            /* Note 1 */
    uint8_t retry_count;                    /* 0: No Retry, 1-30: Retry count, 31: Unlimited Retry count  */
} txd_param_setting_t;

typedef struct {
    int8_t  rssi;                           /* The RSSI of the recieved data */
    uint32_t data_rate;                     /* The data rate of the recieved data */
    uint8_t  *mac_header;                    /* The MAC header of the recieved data  */
    uint8_t  mac_header_len;                 /* The MAC header length of the recieved data */
    uint8_t  *packet_payload;                /* The 802.11 packet payload of the recieved data */
    uint16_t packet_payload_len;            /* The 802.11 packet payload length of the recieved data */
} wifi_data_parse_t;

// Pengfei.Qiu  20160504  Only for internal use
// Need do committee and TC/TW review if put them into common SDK
/**
* @brief This function sets the bandwidth configuration that the WIFI driver uses for a specific wireless port.
* This operation takes effect immediately.
*
* @param[in] port indicates the WIFI port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  bandwidth is the wireless bandwidth.
*
* Value                                                |Definition                                                                 |
* -----------------------------------------------------|-------------------------------------------------|
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ          | 20MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ          | 40MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_2040MHZ        | 20 or 40MHz |
*
* @param[out]  below_above_ch is the primarychannel of bandwidth.
*
* Value                                     |Definition                                                                 |
* ------------------------------------------|-------------------------------------------------|
* \b 0                                      | Extended channel None  |
* \b 1                                      | Extended Channel Above |
* \b 3                                      | Extended channel Below |
*
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_bandwidth_extended(uint8_t port, uint8_t *bandwidth, uint8_t *below_above_ch);

/**
* @brief This function sets the bandwidth configuration that the WIFI driver uses for a specific wireless port.
* This operation takes effect immediately.
*
* @param[in] port indicates the WIFI port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in]  bandwidth is the wireless bandwidth.
*
* Value                                                |Definition                                                                 |
* -----------------------------------------------------|-------------------------------------------------|
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ          | 20MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ          | 40MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_2040MHZ        | 20 or 40MHz |
*
* @param[in]  below_above_ch is the primarychannel of bandwidth.
*
* Value                                     |Definition                                                                 |
* ------------------------------------------|-------------------------------------------------|
* \b 0                                      | Extended channel None  |
* \b 1                                      | Extended Channel Above |
* \b 3                                      | Extended channel Below |
*
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_bandwidth_extended(uint8_t port, uint8_t bandwidth, uint8_t below_above_ch);

/**
* @brief Set frame filter for packets format wanted to be received.
*
* @param [in]filter_switch enable/disable filter_switch
* @param [in]frame_type
* @param [in]vender_ie vender IE information,refer to #extra_vender_ie_t
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_config_set_frame_filter(uint8_t filter_switch, uint16_t frame_type, extra_vender_ie_t *vender_ie);

/**
* @brief Set frame filter for packets format wanted to be received. The RX descriptor will be recieved
*
* @param [in]filter_switch enable/disable filter_switch
* @param [in]frame_type
* @param [in]vender_ie vender IE information,refer to #extra_vender_ie_t
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_config_set_frame_filter_advanced(uint8_t filter_switch, uint16_t frame_type, extra_vender_ie_t *vender_ie);

/**
* @brief Get the rssi threshold.
*
* @param [out]enable enable/disable rssi threshold
* @param [out]rssi The rssi threshold will be returned
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_config_get_rssi_threshold(uint8_t *enable, int8_t *rssi);

/**
* @brief Set the rssi threshold.
*
* @param [in]enable enable/disable rssi threshold
* @param [in]rssi The rssi threshold will be set
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_config_set_rssi_threshold(uint8_t enable, int8_t rssi);

/**
* @brief  The objective of the  API  is to keep CM4 in low power and not be wake up
*         except get a specific IP packet which match the entry of the ip mac port list.
*
*         used to enable or disable the ip mac port filter feature. When the filter is enabled,  cm4 will
*         be wake up if receive the IP packet which match the entry set by #wifi_config_set_ip_mac_port_list_entry
*
* @param [in] flag
*                1  enable
*                0  disable
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_config_set_ip_mac_port_filter(uint8_t flag);

/**
* @brief get the status of the ip mac port filter
*
* @param [out] flag
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_config_get_ip_mac_port_filter(uint8_t *flag);


/**
* @brief add or delete an entry of the ip mac port filter, cm4 will be wake up if receive the IP packet which match the entry
*
* @param [in] action
*               1 create
*               0 delete
* @param [in]  entry please refer to #ip_match_entry_t
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_config_set_ip_mac_port_list_entry(uint8_t action,ip_match_entry_t *entry);


/**
* @brief get the number of entry and the entry itself
*
* @param [out] entry_num
* @param [out] entry
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_config_get_ip_mac_port_list_entry(uint8_t *entry_num ,ip_match_entry_t *entry);

/**
* @brief This function gets the tx power from hardware configuration.
*           The hardware configuration tx power value read back is in a range from 64 to 190.
*
*@param[out]  power is in a range from 64 to 190 which is the hardware configuration. which means set -31.5dbm ~ +31.5dbm to the chip.
*
* @note   The user set power value(power) and really value set to hardware(dbm) have this Calculation formula: dbm = (power-127)/2
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_tx_power(uint8_t *power);

/**
* @brief This function sets the tx power to hardware configuration. 
*           If the power set is over MAX/below MIN value, it will use MAX value/MIN value as workable setting. 
*           Input 127 means set 0 dbm to chip.
*
* @param[in]  power is in a range from 64 to 190 that means chip configured -31.5dbm ~ +31.5dbm
*
* @note   The user set power value(power) and really value set to hardware(dbm) have this Calculation formula: dbm = (power-127)/2
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_tx_power(uint8_t power);

typedef int32_t (*N9LOG_HANDLER)(unsigned char *body, int len);

/**
* @brief Register n9log handler
*
* @param[in] handler N9LOG_HANDLER handler,refer to #N9LOG_HANDLER
*
* @return
*/
int32_t wifi_config_register_n9log_handler(N9LOG_HANDLER handler);

/**
* @brief Get n9log handler
*
* @param[in] body
* @param[in] len
*
* @return n9log handler
*/
N9LOG_HANDLER wifi_config_get_n9log_handler(unsigned char *body, int len);

/**
* @brief This function is called to inform the lower layer that the IP address is ready.
*           This is a private API , which used to inform IP is ready to wifi driver
*           In present, WiFi Driver will do some operation when this API is invoked, such as:
*           Do WiFi&BLE Coexstence relative behavior if BLE is enabled and do Power Saving Status change.
*           This API will be improved, user may need to use new API to replace it in future
** @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_connection_inform_ip_ready(void);

#if (PRODUCT_VERSION == 5932) || (PRODUCT_VERSION == 7682) || (PRODUCT_VERSION == 7686)

/**
* @brief This function get mac adddress from register, it only supported in mt5932/ mt7682/mt7686 currently.
*
* @param [in]port
* @param [out]mac_addr The mac address should be returned
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_config_get_mac_from_register(uint8_t port, uint8_t *mac_addr);
#endif

/**
* @brief This function get scan channel number, it inlcude a band and bg band.
*
* @return  0 means failed, >0 means scan channel number value
*/
uint32_t wifi_config_get_scan_channel_number(void);

/**
* @brief Get wlan statistic info, include Temperature, Tx_Success_Count/Retry_Count/Fail_Count, Rx_Success_Count/Rx_with_CRC/Rx_Drop,
* also more detail please refer to #wifi_statistic_t
*
* @param [in]wifi_statistic
*
* @return  >=0 means success, <0 means fail
*
* @note only used in STA mode. If it connected to an AP router, PHY rate also will be supported.
*/
int32_t wifi_config_get_statistic(wifi_statistic_t *wifi_statistic);

/**
* @brief This function set ip filter.
*
* @param[in] ip is the ip_filter. Please refer to #wifi_ip_filter_t
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_ip_filter(wifi_ip_filter_t ip);

/**
* @brief This function sends a raw Wi-Fi packet over the air. It can config send options.
*
* @param[in] raw_packet is a pointer to the raw packet.Please refer to #wifi_tx_raw_pkt_advanced_ex
* @param[in] length is the length of the raw packet.
* @param[in] txd_param please refer to #txd_param_setting_t
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_connection_send_raw_packet_advanced(uint8_t *payload, uint32_t length, txd_param_setting_t *txd_param);

/**
* @brief This function calculate pmk.
*
* @param[in]  ssid
* @param[in]  ssid_len
* @param[in]  passphrase
* @param[in]  passphrase_len
* @param[out]  pmk pmk is a pointer to the PMK buffer with a length of #WIFI_LENGTH_PMK in bytes.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_calculate_pmk(uint8_t *ssid, uint8_t ssid_length, uint8_t *passphrase, uint8_t passphrase_length, uint8_t *pmk);

/**
* @brief This functions set pwoer save mode,this only support in mt7682/mt7686 currently.
*
* @param[in] power_save_mode indicates three power save mode below.
*
* Value                       |Definition                                                                |
* ----------------------------|--------------------------------------------------------------------------|
* \b 0                        | CAM: CAM (Constantly Awake Mode) is a power save mode that keeps the radio powered up continuously to ensure there is a minimal lag in response time. This power save setting consumes the most power but offers the highest throughput.|
* \b 1                        | LEGACY_POWERSAVE: the access point buffers incoming messages for the radio. The radio occasionally 'wakes up' to determine if any buffered messages are waiting and then returns to sleep mode after it requests each message. This setting conserves the most power but also provides the lowest throughput. It is recommended for radios in which power consumption is the most important (such as small battery-operating devices).|
* \b 2                        | FAST_POWERSAVE: Fast is a power save mode that switches between power saving and CAM modes, depending on the network traffic. For example, it switches to CAM when receiving a large number of packets and switches back to PS mode after the packets have been retrieved. Fast is recommended when power consumption and throughput are a concern.|
*
** @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_power_save_mode(uint8_t ps_mode);

/**
* @brief This functions get pwoer save mode,this only support in mt7682/mt7686 currently.
*
* @param[out] power_save_mode indicates three power save mode. Detail description refered to #wifi_config_set_power_save_mode
*
** @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_power_save_mode(uint8_t *ps_mode);

#ifdef MTK_SINGLE_SKU_SUPPORT

/**
* @brief This function sets the 2G single SKU power.
*
* @param[in]  sku_table_2g is power table of 14 channel in 2G.
*
*For each channel power item format
*| -------|----------|----------|----------- |------------|-------- |---------|---------|---------- |---------- |-------- |-------- |-------- | --------|---------- |---------- |-------- |-------- |-------- |
*|CCK1/2M|CCK5.5/11M|OFDM_6/9M|OFDM_12/18M|OFDM_24/36M|OFDM_48M|OFDM_54M| HT20MCS0|HT20MCS1/2|HT20MCS3/4|HT20MCS5|HT20MCS6|HT20MCS7|HT40MCS0|HT40MCS1/2|HT40MCS3/4|HT40MCS5|HT40MCS6|HT40MCS7|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_single_sku_2G(singleSKU2G_t* sku_table_2g);

/**
* @brief This function gets the 2G single SKU power.
*
* @param[out]  sku_table_2g is power table of 14 channel in 2G.
*
*For each channel power item format
*| -------|----------|----------|----------- |------------|-------- |---------|---------|---------- |---------- |-------- |-------- |-------- | --------|---------- |---------- |-------- |-------- |-------- |
*|CCK1/2M|CCK5.5/11M|OFDM_6/9M|OFDM_12/18M|OFDM_24/36M|OFDM_48M|OFDM_54M| HT20MCS0|HT20MCS1/2|HT20MCS3/4|HT20MCS5|HT20MCS6|HT20MCS7|HT40MCS0|HT40MCS1/2|HT40MCS3/4|HT40MCS5|HT40MCS6|HT40MCS7|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_single_sku_2G(singleSKU2G_t* sku_table_2g);

/**
* @brief This function sets the 5G single SKU power.
*
* @param[out]  sku_table_5g is power table of 42 channel in 5G.
*
*For each channel power item format
*|----------|----------- |------------|-------- |---------|---------|---------- |---------- |-------- |-------- |-------- | --------|---------- |---------- |-------- |-------- |-------- |
*|OFDM_6/9M|OFDM_12/18M|OFDM_24/36M|OFDM_48M|OFDM_54M| HT20MCS0|HT20MCS1/2|HT20MCS3/4|HT20MCS5|HT20MCS6|HT20MCS7|HT40MCS0|HT40MCS1/2|HT40MCS3/4|HT40MCS5|HT40MCS6|HT40MCS7|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_single_sku_5G(singleSKU5G_t* sku_table_5g);

/**
* @brief This function gets the 5G single SKU power.
*
* @param[out]  sku_table_5g is power table of 42 channel in 5G.
*
*For each channel power item format
*|----------|----------- |------------|-------- |---------|---------|---------- |---------- |-------- |-------- |-------- | --------|---------- |---------- |-------- |-------- |-------- |
*|OFDM_6/9M|OFDM_12/18M|OFDM_24/36M|OFDM_48M|OFDM_54M| HT20MCS0|HT20MCS1/2|HT20MCS3/4|HT20MCS5|HT20MCS6|HT20MCS7|HT40MCS0|HT40MCS1/2|HT40MCS3/4|HT40MCS5|HT40MCS6|HT40MCS7|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_single_sku_5G(singleSKU5G_t* sku_table_5g);

#endif


/**
* @brief This function parses the data descriptor in raw packet.
*
* @param[in]  payload is the raw date that be recieved from RX filter handler.
* @param[out] data is the descriptor information parsed from payload, please refer to #wifi_data_parse_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_connection_parse_data_descriptor(uint8_t *payload, wifi_data_parse_t *data);

/**
* @brief This function sets the IP mode to Wi-Fi driver, this is for internal use.
*
* @param[in]  ip_mode is the IP mode that be set. It only supports #WIFI_IP_MODE_STATIC and #WIFI_IP_MODE_DHCP.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_ip_mode(uint8_t ip_mode);

/**
* @brief This function gets the IP mode of Wi-Fi driver, this is for internal use.
*
* @param[out]  ip_mode is the IP mode that Wi-Fi driver is using. It only supports #WIFI_IP_MODE_STATIC and #WIFI_IP_MODE_DHCP.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_ip_mode(uint8_t *ip_mode);


#ifdef __cplusplus
}
#endif

#endif /* WIFI_API */
