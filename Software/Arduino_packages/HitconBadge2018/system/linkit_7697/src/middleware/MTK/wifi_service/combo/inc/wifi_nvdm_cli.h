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
 * @file        wifi_nvdm_cli.h
 * @brief       WiFi NVDM Example CLI
 *
 * @history     2017/08/16   Initial for 1st draft  
 */

/**@addtogroup Wi-Fi_Nvdm_CLI
* @{
* This section introduces the Wi-Fi NVDM CLIs  including terms and acronyms, supported features,
* details on how to use the WiFi NVDM CLI, function groups, enums, structures and functions.
* @section WiFiNvdmCLI_Terms_Chapter1 Wi-Fi NVDM CLI
*  <b>"NVDM Configuration Command" Cli</b>
*  |        Command Format          |           Parameter  Value        |Usage Example|
*  |-------------------------------------------------------------|--------------------|--------------|
*  |<b>config read <group_name> <data_item_name></b>                    |Read the data from NVDM with specified group name and data item name. <br><b><group_name></b></br>: \n the group name of the data item, such as:\n common; STA; AP;config_free <br><b><data_item_name></b></br>: \n the name of the data item, such as:\n IpMode, MacAddr, IpAddr, OpMode, Ssid, SsidLen, BW, AuthMode, EncrypType, WpaPsk, WpaPskLen, DefaultKeyId \n  ListenInterval(range from 1 to 255), \n SharedKey\n (aaaaaaaaaaaaa,bbbbbbbbbbbbb,\n ccccccccccccc,ddddddddddddd),\n SharedKeyLen\n (the key index is 0~4, and the length of each key should be 5,10,13 or 26),\n PSMode(0 means disable, 1 means enable), \n KeepAlivePeriod(it is not used now), \n BeaconLostTime | <b><usage_example></b>  config read STA AuthMode  <br><b><note></b></br> Read authmode of STA from NVDM |
*  |<b>config write <group_name> <data_item_name> <item_value></b> |Write value of specified group name and data item name to NVDM. <br><b><group_name></b></br>: \n the group name of the data item, such as:\n common; STA; AP;config_free <br><b><data_item_name></b></br>: \n The same with config read cli. <br><b><item_value></b></br>: \n The value depend on the specified group name and data item name. | <b><usage_example></b>  :\n 1.config write STA MacAddr 00:00:4c:76:87:01\n (Write the MAC address of STA to NVDM)\n 2. config write STA SsidLen 6\n (the maximum length of SSID is 32 bytes)\n 3. config write STA BW 1\n (BW:0 IS 20MHz, 1 is 40MHz)\n 4. config write STA WirelessMode 9\n (set the wireless mode of station as 11bgn mixed mode, detail wireless mode refer to "wifi config set wirelessmode <port> <mode>" )\n 5. config write config_free IpMode dhcp\n (set the IpMode of station as dhcp)\n 6. config write STA BeaconLostTime 2\n (if the station does not receive the beacon after 2s,then the station will disconnect from the AP)  |
*  |<b>config reset <group_name></b>                                    |<b><group_name></b>: \n the group name of the data item, such as:\n common; STA; AP;config_free   | Recover default value of the group specified  or not specified \n <b>Example1</b>:\n config reset STA\n Recover the default value of the group STA. <br><b>Example2</b></br>: \n config reset\n Recover the default value of the group not specified. |
*  |<b>config show <group_name></b>                                     |<b><group_name></b>: \n the group name of the data item, such as:\n common; STA; AP;config_free   | Show content of the group specified or not specified \n <b><usage_example></b>  :\n config show STA  <br><b><note></b></br> Show content of the group STA.\n  <br><b><usage_example></b></br>  : config show  <br><b><note></b></br> Show content of the group not specified. |
*
*
* @section WiFiNvdmCLI_Terms_Chapter2 NVDM Config Group and Items
*  <b>Group_Name "common"</b>
*  |Data_item_name                   | Item_value                       |      Description                    |
*  |----------------------------- |------------------------------ |---------------------------------|
*  |<b>OpMode</b>                    | Range: 1~4, default 1            | 1: STA  2: AP  3:Repeater  4:Monitor  | 
*  |<b>CountryRegion</b>             | Range:0~7, 30~33, default 5      | <b><note></b> current the Country Region is not used in wifi task. @sa Refer to #wifi_config_get_country_region 2.4G Band | 
*  |<b>CountryCode</b>               | 2 characters,\n  Example: TW, US, EU,CN,... defeult "TW"  | WiFi task will contain the Country Code strings in Beacon IE if device is AP mode.  <br><b><note></b></br>: It is not affect the channel list or power setting. | 
*  |<b>CountryRegionABand</b>        | Range:0~22,  30~37               | <b><note></b>: current the Country Region is not used in wifi task. @sa Refer to #wifi_config_get_country_region 5G Band | 
*  |<b>RadioOff</b>                  | Range: 0,1, default 0            | <b><note></b>: Internal Use.        | 
*  |<b>DbgLevel</b>                  | Range:0~3, default 3             | <b><note></b>: Internal Use.        | 
*  |<b>RTSThreshold</b>              | Default 2347                     | <b><note></b>: Internal Use.       | 
*  |<b>FragThreshold</b>             | Default 2346                     | <b><note></b>: Internal Use.       | 
*  |<b>BGChannelTable</b>            |<b>Format:</b> <First_Channel>,<Total_Channel_Number>,<Scan_Type>\| | <b><First_Channel></b> The first channel <br><b><Total_Channel_Number></b></br> Total channel number <br><b><Scan_Type></b></br> the scan type for the channel,  0:Active Scan,  1: Passive Scan. <br><b><usage_example 1></b></br> : 1,14,0\|  \n It means the channel 1~ channel 14 with active scan  <br><b><usage_example 2></b></br> : 1,11,0\|12,2,1\|  \n It means the channel 1~ channel 11  with active scan, the channel 12~ channel14 with passive scan. | 
*  |<b>AChannelTable</b>             |<b>Format:</b> <First_Channel>,<Total_Channel_Number>,<Scan_Type>\| | <b><First_Channel></b> The first channel <br><b><Total_Channel_Number></b></br> Total channel number <br><b><Scan_Type></b></br> the scan type for the channel,  0:Active Scan,  1: Passive Scan. <br><b><usage_example></b></br> : 52,3,1\|149,4,0\| \n It means: The channel 52, 56, 60 with passive scan. The channel 149, 153,157, 161 with active scan.  | 
*  |<b>syslog_filters</b>            |                                  | <b><note></b>: Internal Use.        | 
*  |<b>StaFastLin</b>                | Default 0                        | <b><note></b>: Internal Use.       | 
*  |<b>WiFiPrivilegeEnable</b>       |                                  | <b><note></b>: Internal Use.       | 
*
*
*  <b>Group_Name "STA"</b>
*  |Data_item_name                   | Item_value                       |      Description                    |
*  |----------------------------- |------------------------------ |---------------------------------|
*  |<b>LocalAdminMAC</b>             | Range 0~1,default 1              | <b><note></b>: WiFi Task usage , please keep it as 1. | 
*  |<b>MacAddr</b>                   | Default 00:0c:43:76:87:22        | <b><note></b>: The Device only use API: #wifi_config_get_mac_address to  read MAC from Efuse first, if Efuse no MAC, read it from NVDM. If NVDM no MAC, or NVDM is disabled, return error. | 
*  |<b>Ssid</b>                      | The SSID string. Default "MTK_SOFT_AP"  |The Device will try to connect the target AP with the matched SSID, after system power on and act as STA mode. | 
*  |<b>SsidLen</b>                   | The SSID string length,Default 11  |The max size is 32 byte.            | 
*  |<b>BssType</b>                   | Default 1                        |<b><note></b>: WiFi Task usage , please keep it as 1. | 
*  |<b>Channel</b>                   | The working channel number,2.4G: 1~14, 5G: 36~165, default 1  |The channel number for 2.4G Band or 5G band,  it should be limited by BGChannelTable,  AChannelTable and WirelessMode in NVDM, channel @sa Refer to #wifi_config_get_country_region | 
*  |<b>BW</b>                        | Range:0: 20MHz, 1:20/40 MHz,default 0   | The working bandwidth when boot up in STA mode. | 
*  |<b>WirelessMode</b>              | Range:0~11,default 9             | Initialize the wireless mode in STA mode. @sa Refer to #wifi_phy_mode_t. | 
*  |<b>BADecline</b>                 | Default 0                        | <b><note></b>: WiFi Task usage , please keep it as 0. | 
*  |<b>AutoBA</b>                    | Default 1                        | <b><note></b>: WiFi Task usage , please keep it as 1. | 
*  |<b>HT_MCS</b>                    | Default 33                       | <b><note></b>: WiFi Task usage , please keep it as 33. | 
*  |<b>HT_BAWinSize</b>              | Default mt7682/mt7686/mt5932: 4 \n mt7687/mt7697: 64 | <b><note></b>: WiFi Task usage , please keep it as 64. | 
*  |<b>HT_GI</b>                     | Default 1                        | <b><note></b>: WiFi Task usage , please keep it as 1. | 
*  |<b>HT_PROTECT</b>                | Default 1                        | <b><note></b>: WiFi Task usage , please keep it as 1. | 
*  |<b>HT_EXTCHA</b>                 | Default 1                        | <b><note></b>: WiFi Task usage , please keep it as 1. | 
*  |<b>WmmCapable</b>                | Default 1                        | <b><note></b>: WiFi Task usage , please keep it as 1. | 
*  |<b>ListenInterval</b>            | Range 1~255, default 1           | <b><note></b>: WiFi Task usage , please keep it as 1. | 
*  |<b>AuthMode</b>                  | Range:0,4,7,9, default 0         | Initialize the auth mode in STA mode, @sa Refer to #wifi_auth_mode_t and table <b>Combinations of Auth Mode and Encrypt Type</b> | 
*  |<b>EncrypType</b>                | Range:0,1,4,6,8, default 1       | Initialize the encrypt type in STA mode, @sa Refer to #wifi_encrypt_type_t and table <b>Combinations of Auth Mode and Encrypt Type</b>. | 
*  |<b>WpaPsk</b>                    | WPA Key String,default "12345678" | Initialize the PSK in STA mode, the MAX length is 64. If the length is 64, it will be regarded as PMK.. | 
*  |<b>WpaPskLen</b>                 |The length of WPA Key String (8~64),default 8 |0~63:  the WPA Key string will be treated as ASCLL character.\n 64:  the WPA Key string will be treated as PMK HEX character. | 
*  |<b>Password</b>                  |                                  | <b><note></b>: Internal Use.         | 
*  |<b>PMK</b>                       |                                  | <b><note></b>: Not used.             | 
*  |<b>PMK_INFO</b>                  |                                  | <b><note></b>: A temporary method to store PMK info, include "SSID, Password, PMK". It is not a official method. | 
*  |<b>PairCipher</b>                |                                  | <b><note></b>: Not used.             | 
*  |<b>GroupCipher</b>               |                                  | <b><note></b>: Not used.             | 
*  |<b>DefaultKeyId</b>              | Range:0~3,default 0              | It used for indicating the index of WEP key. | 
*  |<b>SharedKey</b>                 | Example: aaaaa,bbbbb,ccccc,ddddd | WEP Key string for index0~index3, use "," as delimiter. \n The Key of index0 is :aaaaa \nThe Key of index1 is :bbbbb \nThe Key of index2 is :ccccc \nThe Key of index3 is :ddddd. | 
*  |<b>SharedKeyLen</b>              | Range: 5, 10, 13, 26             | 5, 13: the key with this length will be treated as ASCLL character in wifi driver. \n10,  26: the key with this length will be treated as HEX character in wifi driver. | 
*  |<b>PSMode</b>                    | Range:0~2,default 0              | 0: CAM(Constantly Awake Mode) \n 1: Legacy Power Save Mode \n 2: Fast Power Save Mode. | 
*  |<b>BeaconLostTime</b>            | Range: 1~255 (unit: second)      | If Device is connected with target AP, Device will disconnect if it not receive any beacon or data frames from the target AP after the BeaconLostTime timeout. | 
*  |<b>ApcliBWAutoUpBelow</b>        | 1                                | <b><note></b>: WiFi Task usage , please keep it as 1. | 
*
*
*  <b>Group_Name "AP"</b>
*  |Data_item_name                   | Item_value                       |      Description                    |
*  |----------------------------- |-------------------------------|---------------------------------|
*  |<b>LocalAdminMAC</b>             | Default 1                        | <b><note></b>: WiFi Task usage , please keep it as 1. | 
*  |<b>MacAddr</b>                   | 6 Decimal Number, Default: 00:0c:43:76:87:12  | The Device only use API: #wifi_config_get_mac_address to read MAC from Efuse first, if Efuse no MAC, read it from NVDM. If NVDM no MAC, or NVDM is disabled, return error. | 
*  |<b>Ssid</b>                      | The SSID string, MAX length is 32. default "MTK_SOFT_AP"  | The Device will send beacon with this  SSID, after system power on and act as AP mode. | 
*  |<b>SsidLen</b>                   | The SSID string length, default 11 | The max size is 32 byte.        | 
*  |<b>Channel</b>                   | The working channel number,2.4G: 1~14, 5G: 36~165, default 1           |The channel number for 2.4G Band or 5G band,  it should be limited by BGChannelTable,  AChannelTable and WirelessMode in NVDM.Channel @sa Refer to #wifi_config_get_country_region | 
*  |<b>BW</b>                        | Range:  0: 20MHz, 1:20/40 MHz, default 0  |   The working bandwidth when boot up in AP mode . | 
*  |<b>WirelessMode</b>              | Range:0~11,default 9             | Initialize the wireless mode in AP mode, @sa Refer to #wifi_phy_mode_t. | 
*  |<b>AutoBA</b>                    | Default 1                        | <b><note></b>: WiFi Task usage , please keep it as 1. | 
*  |<b>HT_MCS</b>                    | Default 33                       | <b><note></b>: WiFi Task usage , please keep it as 33. | 
*  |<b>HT_BAWinSize</b>              | Default 64                       | <b><note></b>: WiFi Task usage , please keep it as 64. | 
*  |<b>HT_GI</b>                     | Default 1                        | <b><note></b>: WiFi Task usage , please keep it as 1. | 
*  |<b>HT_PROTECT</b>                | Default 1                        | <b><note></b>: WiFi Task usage , please keep it as 1. | 
*  |<b>HT_EXTCHA</b>                 | Default 1                        | <b><note></b>: WiFi Task usage , please keep it as 1. | 
*  |<b>WmmCapable</b>                | Default 1                        | <b><note></b>: WiFi Task usage , please keep it as 1. | 
*  |<b>DtimPeriod</b>                | Range 1~255, default 1           | The number of DTIM interval in PS mode | 
*  |<b>AuthMode</b>                  | Range: 0,4,7,9 ,default 0        |Initialize the auth mode in AP mode, @sa Refer to #wifi_auth_mode_t and table <b>Combinations of Auth Mode and Encrypt Type</b>. | 
*  |<b>EncrypType</b>                | Range: 0,1,4,6,8 ,default 1      |Initialize the encrypt type in AP mode, @sa Refer to #wifi_encrypt_type_t and table <b>Combinations of Auth Mode and Encrypt Type</b>. | 
*  |<b>WpaPsk</b>                    | WPA Key String,default "12345678" | The Key for WPA, WPA2 auth mode.   | 
*  |<b>WpaPskLen</b>                 | The length of WPA Key String (8~64) | 8~63:  the WPA Key string will be treated as ASCLL character. \n 64:  the WPA Key string will be treated as PMK HEX character. | 
*  |<b>Password</b>                  |                                  | <b><note></b>: Not used.            | 
*  |<b>PMK</b>                       |                                  | <b><note></b>: Not used.            | 
*  |<b>PairCipher</b>                |                                  | <b><note></b>: Not used.            | 
*  |<b>GroupCipher</b>               |                                  | <b><note></b>: Not used.            | 
*  |<b>DefaultKeyId</b>              | Range: 0~3,default 0             | It used for indicating the index of WEP key. | 
*  |<b>SharedKey</b>                 | Example: aaaaa,bbbbb,ccccc,ddddd | WEP Key string for index0~index3, use "," as delimiter \n The Key of index0 is :aaaaa \n The Key of index1 is :bbbbb \n The Key of index2 is :ccccc \n The Key of index3 is :ddddd . | 
*  |<b>SharedKeyLen</b>              | Range: 5, 10, 13, 26             | 5, 13: the key with this length will be treated as ASCLL character in wifi driver \n 10, 26: the key with this length will be treated as HEX character in wifi driver. | 
*  |<b>HideSSID</b>                  | Range: 0: disable, 1:enable,default 0  |                               | 
*  |<b>RekeyInterval</b>             | 3600                             |                                     | 
*  |<b>BcnDisEn</b>                  | 0                                |                                     | 
*
*
*  <b>Group_Name "config_free"</b>
*  |Data_item_name                   | Item_value                       |      Description                    |
*  |----------------------------- |-------------------------------|---------------------------------|
*  |<b>IpAddr</b>                    | Default:192.168.0.1              |           | 
*  |<b>IpNetmask</b>                 | Default:255.255.255.0            |           | 
*  |<b>IpGateway</b>                 | Default:192.168.0.1              | If OpMode=1, and IpMode=static in NVDM, this is for Static IP setting in STA Mode. Device will not trigger DHCP process to do IP hand shark with target AP.\n If OpMode=2 in NVDM,this is for Static IP setting in AP Mode | 
*  |<b>ConfigFree_Ready</b>          |                                  | <b><note></b>: Internal Use.       | 
*  |<b>ConfigFree_Enable</b>         |                                  | <b><note></b>: Internal Use.       | 
*  |<b>IpMode</b>                    | Range: dhcp , static             | dhcp: device will use DHCP to get IP. \n static:device will use static IP address which defined in the data items of IpAddr, IpNetmask, IpGateway in NVDM. | 
*
*
*
* @section WiFiNvdmCLI_Terms_Chapter3 Wi-Fi NVDM CLI Usage Examples
*  <b>"NVDM Configuration Command" Cli Example</b>
*  | Example  Items                    |	      "Config" Cli Example       |
*  |-------------------------------|----------------------------------|
*  |<b>MT7687 Station Mode</b>            |  |
*  |<b>AP Router :OPEN </b>               |<b>connect AP with OPEN mode:</b>\n config write common OpMode 1 \n config write STA Ssid MTK_AP\n config write STA SsidLen 6 \n config write STA AuthMode 0\n config write STA EncrypType 1\n reboot |
*  |<b>AP Router :WPA2-PSK(AES)</b>       |<b>connect AP with WPA2-PSK(AES)mode, password is 12345678:</b>\n config write common OpMode 1\n config write STA Ssid MTK_AP \n config write STA SsidLen 6 \n config write STA AuthMode 7\n config write STA EncrypType 6\n config write STA WpaPsk 12345678 \n config write STA WpaPskLen 8 \n reboot |
*  |<b>AP Router :WPA2-PSK(TKIP)</b>      |<b>connect AP with WPA2-PSK(TKIP) mode:</b>\n config write common OpMode 1 1\n config write STA Ssid MTK_AP \n config write STA SsidLen 6 \n config write STA AuthMode 7\n config write STA EncrypType 4\n config write STA WpaPsk 12345678 \n config write STA WpaPskLen 8 \n reboot |
*  |<b>AP Router :WPA-PSK(TKIP)</b>       |<b>connect AP with WPA-PSK(TKIP) mode,password is 12345678:</b>\n config write common OpMode 1 \n config write STA Ssid MTK_AP\n config write STA SsidLen 6\n config write STA SsidLen 6 \n config write STA AuthMode 4\n config write STA EncrypType 4\n config write STA WpaPsk 12345678 \n config write STA WpaPskLen 8 \n reboot |
*  |<b>AP Router :WEP(OPEN)(64bit)</b>    |<b>connect AP with WEP OPEN(64bit) mode,WEP key is 12345:</b>\n config write common OpMode 1\n config write STA Ssid MTK_AP\n config write STA SsidLen 6\n config write STA AuthMode 0\n config write STA EncrypType 0\n config write STA SharedKey 12345,0,0,0\n config write STA SharedKeyLen 5,0,0,0\n config write STA DefaultKeyId 0(use the first key index,the key index start from 0)\n reboot |
*  |<b>MT7687 SoftAP Mode</b>             |  |
*  |<b>AP Router :OPEN </b>               |<b>connect AP with OPEN mode:</b>\n config write common OpMode 1 \n config write AP Ssid MTK_AP\n config write AP SsidLen 6 \n config write AP AuthMode 0\n config write AP EncrypType 1\n config write AP BW 1\n reboot |
*  |<b>AP Router :WPA2-PSK(AES)</b>       |<b>connect AP with WPA2-PSK(AES)mode, password is 12345678:</b>\n config write common OpMode 1\n config write AP Ssid MTK_AP \n config write AP SsidLen 6 \n config write AP AuthMode 7\n config write AP EncrypType 6\n config write AP WpaPsk 12345678 \n config write AP WpaPskLen 8 \n reboot |
*  |<b>AP Router :WPA2-PSK(TKIP)</b>      |<b>connect AP with WPA2-PSK(TKIP) mode:</b>\n config write common OpMode 1 1\n config write AP Ssid MTK_AP \n config write AP SsidLen 6 \n config write AP AuthMode 7\n config write AP EncrypType 4\n config write AP WpaPsk 12345678 \n config write AP WpaPskLen 8 \n reboot |
*  |<b>AP Router :WPA-PSK(TKIP)</b>       |<b>connect AP with WPA-PSK(TKIP) mode,password is 12345678:</b>\n config write common OpMode 1 \n config write AP Ssid MTK_AP\n config write AP SsidLen 6\n config write AP SsidLen 6 \n config write AP AuthMode 4\n config write AP EncrypType 4\n config write AP WpaPsk 12345678 \n config write AP WpaPskLen 8 \n reboot |
*  |<b>AP Router :WEP(OPEN)(64bit)</b>    |<b>connect AP with WEP OPEN(64bit) mode,WEP key is 12345:</b>\n config write common OpMode 1\n config write AP Ssid MTK_AP\n config write AP SsidLen 6\n config write AP AuthMode 0\n config write AP EncrypType 0\n config write AP SharedKey 12345,0,0,0\n config write AP SharedKeyLen 5,0,0,0\n config write AP DefaultKeyId 0(use the first key index,the key index start from 0)\n reboot |
*  |<b>Repeater Mode</b>                  |   |
*  |<b>AP Port :Open Mode\n AP Client Port:Open Mode</b>             |<b>The remote AP Router is in OPEN mode, 40MHz, channel 6:</b>\n config write common OpMode 3\n config write STA Ssid MTK_AP\n config write STA SsidLen 6\n config write STA AuthMode 0\n config write STA EncrypType 1\n config write STA Channel 6\n config write STA BW 1\n config write AP Ssid MTK_SOFT_AP\n config write AP SsidLen 11\n config write AP AuthMode 0\n config write AP EncrypType 1\n config write AP Channel 6\n config write AP BW 1\n reboot|
*  |<b>AP Port :WPA-PSK(TKIP)\n AP Client Port:WPA2-PSK(AES)</b>    |<b>The remote AP Router is in WPA2-PSK(AES) mode, 40MHz, channel 6:</b>\n config write common OpMode 3\n config write STA Ssid MTK_AP\n config write STA SsidLen 6\n config write STA AuthMode 7\n config write STA EncrypType 6\n config write STA WpaPskLen 8\n config write STA WpaPsk 12345678\n config write STA Channel 6\n config write STA BW 1\n config write AP Ssid MTK_SOFT_AP\n config write AP SsidLen 11\n config write AP AuthMode 4\n config write AP EncrypType 4\n config write AP WpaPskLen 8\n config write AP WpaPsk 12345678\n config write AP Channel 6\n config write AP BW 1\n reboot|
*  |<b>AP Port :WEP(OPEN)(64bit)\n AP Client Port: WPA-PSK(TKIP)</b>|<b>The remote AP Router is in WPA-PSK(TKIP) mode, 40MHz, channel 6:</b>\n config write common OpMode 3\n config write STA Ssid MTK_AP\n config write STA SsidLen 6\n config write STA AuthMode 4\n config write STA EncrypType 4\n config write STA WpaPskLen 8\n config write STA WpaPsk 12345678\n config write STA Channel 6\n config write STA BW 1\n config write AP Ssid MTK_SOFT_AP\n config write AP SsidLen 11\n config write AP AuthMode 0\n config write AP EncrypType 0\n config write AP SharedKey 12345,0,0,0\n config write AP SharedKeyLen 5,0,0,0\n config write AP DefaultKeyId 0\n config write AP Channel 6\n config write AP BW 1\n reboot |
*  |<b>Switch Operation Mode</b>          |   |
*  |<b>Switch to Station Mode\n Note:\n </b> opmode value:\n STA mode:1\n SoftAP mode:2\n Repeater mode:3 | config read  common OpMode (get current opmode)\n config write common OpMode 1(switch to station mode)\n reboot |
*
*
*
*/

/**
*@}
*/


#ifndef __WIFI_NVDM_CLI_H__
#define __WIFI_NVDM_CLI_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /*  __WIFI_NVDM_CLI_H__ */
