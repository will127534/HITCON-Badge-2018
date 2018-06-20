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

#ifndef __WIFI_SCAN_H__
#define __WIFI_SCAN_H__

//#include "type_def.h"
#include "stdint.h"
#include "wifi_api.h"
#include "connsys_profile.h"
#include "memory_attribute.h"
#include "wifi_event.h"
#include "wifi_config_free.h"

/**
 * @brief temperary definition of the error code, this should be redesigned in the future
 *
 * @note currently all errors are defined as -1
 */
#define WIFI_SUCC  (0)
#define WIFI_FAIL  (-1)
#define WIFI_ERR_PARA_INVALID (-1)
#define WIFI_ERR_NOT_SUPPORT  (-1)


#if defined(MTK_MINISUPP_ENABLE) || (defined(MTK_WIFI_REPEATER_ENABLE) && defined(MTK_WIFI_ROM_ENABLE))
#define MTK_CM4_SCAN_TABLE_ENABLE
#endif

extern unsigned char g_scan_table_size;
extern const unsigned char WPA_OUI[];
extern const unsigned char RSN_OUI[];
extern const unsigned char WPS_OUI[];
extern const unsigned char ZeroMAC[];

typedef long __time_t;

struct __time {
    __time_t sec;
    __time_t usec;
};


#define hex_isdigit(c)      (('0' <= (c) && (c) <= '9') || ('a' <= (c) && (c) <= 'f') || ('A' <= (c) && (c) <= 'F'))

 //#define PACKED  __attribute__ ((packed))
#define PACKED ATTR_PACKED

 #ifdef MTK_MINISUPP_ENABLE
 typedef struct __wpa_supplicant_api_callback {
    int (*set_security)(uint8_t port, uint8_t auth_mode, uint8_t encrypt_type);
    int (*get_security)(uint8_t port, uint8_t *auth_mode, uint8_t *encrypt_type);

    int (*set_ssid)(uint8_t port, uint8_t *ssid, uint8_t ssid_length);
    int (*get_ssid)(uint8_t port, uint8_t *ssid, uint8_t *ssid_length);

    int (*set_bssid)(uint8_t *bssid);

    int (*set_pmk)(uint8_t port, uint8_t *pmk);
    int (*get_pmk)(uint8_t port, uint8_t *pmk);
    int (*cal_pmk)(uint8_t *passphrase, uint8_t *ssid, uint8_t ssid_length, uint8_t *psk);

    int (*set_wpapsk)(uint8_t port, uint8_t *passphrase, uint8_t passphrase_length);
    int (*get_wpapsk)(uint8_t port, uint8_t *passphrase, uint8_t *passphrase_length);

    int (*set_wep_key)(uint8_t port, wifi_wep_key_t *wep_keys);
    int (*get_wep_key)(uint8_t port, wifi_wep_key_t *wep_keys);

    int (*wpa_supplicant_entry_iface_change)(unsigned char op_mode);
    void (*wpa_supplicant_entry_op_mode_get)(unsigned char *op_mode);
    void (*wpa_supplicant_entry_stop_scan)(void);
    void (*wpa_supplicant_entry_start_scan)(void);
    void (*wpa_supplicant_entry_radio_onoff)(uint8_t radio_on);

    int (*do_wps_pbc)(char *cmd);
    int (*do_wps_pin)(char *cmd, char *buf, int buflen);
    int (*do_wps_reg_pin)(char *cmd);
    int (*wps_ap_pin)(char *cmd, char *buf, int buflen);
    void (*wpa_supplicant_check_dpid_from_wps_ap)(void *ie_buf, uint16_t *device_passwd_id);
    void (*wpa_supplicant_check_wps_element_from_ap)(void *ie_buf, wifi_wps_element_t *wps_element);
    int32_t (*wpa_supplicant_query_connect_status)(uint8_t *link_status);
    int (*wps_generate_pin)(void);
    int (*wps_get_pin)(char *cmd, char *buf, int buflen);

    int (*get_ap_sta)(char *mac_addr);
    int (*get_sta_qos_bit)(int32_t inf_num, char *mac_addr);   /* return value - 1: WMM, 0: non-WMM, <0: station doesn't exist */

    void (*show_ifaces)(void);
    void (*wpa_supplicant_disable_trigger_scan)(void);
    void (*wpa_supplicant_enable_trigger_scan)(void);

    int (*get_bssid)(uint8_t *bssid);
    int (*set_ssid_bssid)(unsigned char *ssid, unsigned int ssidlen, uint8_t *bssid);
} wpa_supplicant_api_callback;
 extern wpa_supplicant_api_callback __g_wpa_supplicant_api;
#endif /* MTK_MINISUPP_ENABLE */

#define MAX_LEN_OF_SUPPORTED_RATES      12    // 1, 2, 5.5, 11, 6, 9, 12, 18, 24, 36, 48, 54
#define MAX_VIE_LEN                     512 //384   // New for WPA cipher suite variable IE sizes.
typedef union _LARGE_INTEGER {
    struct {
        uint32_t  LowPart;
        int32_t   HighPart;
    } u;
    int64_t       QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;
typedef struct {
    uint8_t     bValid;         // 1: variable contains valid value
    uint8_t       CfpCount;
    uint8_t       CfpPeriod;
    uint16_t      CfpMaxDuration;
    uint16_t      CfpDurRemaining;
} PACKED CF_PARM, *PCF_PARM;

// EDCA configuration from AP's BEACON/ProbeRsp
typedef struct {
    uint8_t     bValid;         // 1: variable contains valid value
    uint8_t     bAdd;         // 1: variable contains valid value
    uint8_t     bQAck;
    uint8_t     bQueueRequest;
    uint8_t     bTxopRequest;
    uint8_t     bAPSDCapable;
//  uint8_t     bMoreDataAck;
    uint8_t       EdcaUpdateCount;
    uint8_t       Aifsn[4];       // 0:AC_BK, 1:AC_BE, 2:AC_VI, 3:AC_VO
    uint8_t       Cwmin[4];
    uint8_t       Cwmax[4];
    uint16_t      Txop[4];      // in unit of 32-us
    uint8_t     bACM[4];      // 1: Admission Control of AC_BK is mandattory
} EDCA_PARM, *PEDCA_PARM;
// QBSS LOAD information from QAP's BEACON/ProbeRsp
typedef struct {
    uint8_t     bValid;                     // 1: variable contains valid value
    uint16_t      StaNum;
    uint8_t       ChannelUtilization;
    uint16_t      RemainingAdmissionControl;  // in unit of 32-us
} QBSS_LOAD_PARM, *PQBSS_LOAD_PARM;

// QOS Capability reported in QAP's BEACON/ProbeRsp
// QOS Capability sent out in QSTA's AssociateReq/ReAssociateReq
typedef struct {
    uint8_t     bValid;                     // 1: variable contains valid value
    uint8_t     bQAck;
    uint8_t     bQueueRequest;
    uint8_t     bTxopRequest;
//  uint8_t     bMoreDataAck;
    uint8_t       EdcaUpdateCount;
} QOS_CAPABILITY_PARM, *PQOS_CAPABILITY_PARM;

//  HT Capability INFO field in HT Cap IE .
typedef struct  {
    uint16_t    AdvCoding: 1;
    uint16_t    ChannelWidth: 1;
    uint16_t    MimoPs: 2; //momi power safe
    uint16_t    GF: 1;  //green field
    uint16_t    ShortGIfor20: 1;
    uint16_t    ShortGIfor40: 1; //for40MHz
    uint16_t    TxSTBC: 1;
    uint16_t    RxSTBC: 2;
    uint16_t    DelayedBA: 1;   //rt2860c not support
    uint16_t    AMsduSize: 1;   // only support as zero
    uint16_t    CCKmodein40: 1;
    uint16_t    PSMP: 1;
    uint16_t    Forty_Mhz_Intolerant: 1;
    uint16_t    LSIGTxopProSup: 1;
} PACKED HT_CAP_INFO, *PHT_CAP_INFO;

//  HT Capability INFO field in HT Cap IE .
typedef struct  {
    uint8_t MaxRAmpduFactor: 2;
    uint8_t MpduDensity: 3;
    uint8_t rsv: 3; //momi power safe
} PACKED HT_CAP_PARM, *PHT_CAP_PARM;

//  HT Capability INFO field in HT Cap IE .
typedef struct  {
    uint16_t    Pco: 1;
    uint16_t    TranTime: 2;
    uint16_t    rsv: 5; //momi power safe
    uint16_t    MCSFeedback: 2; //0:no MCS feedback, 2:unsolicited MCS feedback, 3:Full MCS feedback,  1:rsv.
    uint16_t    PlusHTC: 1; //+HTC control field support
    uint16_t    RDGSupport: 1;  //reverse Direction Grant  support
    uint16_t    rsv2: 4;
} PACKED EXT_HT_CAP_INFO, *PEXT_HT_CAP_INFO;

// The structure for HT Capability IE.
typedef struct  {
    HT_CAP_INFO     HtCapInfo;
    HT_CAP_PARM     HtCapParm;
    uint8_t         MCSSet[16];
    EXT_HT_CAP_INFO ExtHtCapInfo;
    uint8_t         TxBFCap[4]; // beamforming cap. rt2860c not support beamforming.
    uint8_t         ASCap;  //antenna selection.
} PACKED HT_CAPABILITY_IE, *PHT_CAPABILITY_IE;

// 7.3.27 20/40 Bss Coexistence Mgmt capability used in extended capabilities information IE( ID = 127 = IE_EXT_CAPABILITY).
//  This is the first octet and was defined in 802.11n D3.03 and 802.11yD9.0
typedef struct  _EXT_CAP_INFO_ELEMENT {
    uint32_t    BssCoexistMgmtSupport: 1;
    uint32_t    rsv: 1;
    uint32_t    ExtendChannelSwitch: 1;
    uint32_t    rsv2: 7;
    uint32_t    FMSSupport: 1; /*bit 11*/
    uint32_t    rsv3: 4;
    uint32_t    TFSSupport: 1; /*bit 16*/
    uint32_t    WNMSleepSupport: 1; /*bit 17*/
    uint32_t    rsv4: 1;
    uint32_t    BssTransitionManmt: 1;
    uint32_t    rsv5: 6;
    uint32_t    DMSSupport: 1;
    uint32_t    rsv6: 6;
} PACKED EXT_CAP_INFO_ELEMENT, *PEXT_CAP_INFO_ELEMENT;

//   field in Addtional HT Information IE .
typedef struct  {
    uint8_t ExtChanOffset: 2;
    uint8_t RecomWidth: 1;
    uint8_t RifsMode: 1;
    uint8_t CntlAccOnly: 1; //controlled access only
    uint8_t SerInterGranu: 3;    //service interval granularity
} PACKED ADD_HTINFO, *PADD_HTINFO;

typedef struct  {
    uint16_t    OperaionMode: 2;
    uint16_t    NonGfPresent: 1;
    uint16_t    TransmitBustLimit: 1;
    uint16_t    OBSSNonHT: 1;
    uint16_t    rsv: 11;
} PACKED ADD_HTINFO2, *PADD_HTINFO2;

typedef struct  {
    uint16_t    StbcMcs: 7;
    uint16_t    DualStbcProtect: 1;
    uint16_t    SecondBeacon: 1;
    uint16_t    LsigTxopProtFull: 1; // L-SIG TXOP protection full support
    uint16_t    PcoActive: 1;
    uint16_t    PcoPhase: 1;
    uint16_t    rsv: 4;
} PACKED ADD_HTINFO3, *PADD_HTINFO3;

#define SIZE_ADD_HT_INFO_IE     22
typedef struct   {
    uint8_t             ControlChan;
    ADD_HTINFO          AddHtInfo;
    ADD_HTINFO2         AddHtInfo2;
    ADD_HTINFO3         AddHtInfo3;
    uint8_t             MCSSet[16];
} PACKED ADD_HT_INFO_IE, *PADD_HT_INFO_IE;

typedef struct __BeaconProbeRspStr {
    uint8_t                   Addr2[WIFI_MAC_ADDRESS_LENGTH];
    uint8_t                   Bssid[WIFI_MAC_ADDRESS_LENGTH];
    int8_t                    Ssid[WIFI_MAX_LENGTH_OF_SSID];
    uint8_t                   SsidLen;
    uint8_t                   BssType;
    uint16_t                  BeaconPeriod;
    LARGE_INTEGER           Timestamp;
    uint16_t                  CapabilityInfo;
    ADD_HT_INFO_IE       AddHtInfo;
    HT_CAPABILITY_IE     HtCapability;
    uint16_t                  LengthVIE;
    uint8_t              VarIE[MAX_VIE_LEN];
    uint8_t              AddHtInfoLen;
    uint8_t              HtCapabilityLen;
    uint8_t              Channel;
    uint8_t              CentralChannel;
    uint8_t              wps;
    int8_t                 rssi;
    unsigned char        auth_mode;
    unsigned char        encrypt_mode;
    unsigned char        hidden_ssid;
    wifi_wps_element_t   wps_element;
} BeaconProbeRspStr_t;

typedef struct  {
    uint16_t      TID: 4;
    uint16_t      __EOSP: 1;
    uint16_t      AckPolicy: 2; //0: normal ACK 1:No ACK 2:scheduled under MTBA/PSMP  3: BA
    uint16_t      AMsduPresent: 1;
    uint16_t      Txop_QueueSize: 8;
} PACKED QOS_CONTROL, *PQOS_CONTROL;

// 2-byte Frame control field
typedef struct   {
    uint16_t      Ver: 2;             // Protocol version
    uint16_t      Type: 2;            // MSDU type
    uint16_t      SubType: 4;         // MSDU subtype
    uint16_t      ToDs: 1;            // To DS indication
    uint16_t      FrDs: 1;            // From DS indication
    uint16_t      MoreFrag: 1;        // More fragment bit
    uint16_t      Retry: 1;           // Retry status bit
    uint16_t      PwrMgmt: 1;         // Power management bit
    uint16_t      MoreData: 1;        // More data bit
    uint16_t      Wep: 1;             // Wep data
    uint16_t      Order: 1;           // Strict order expected
} PACKED FRAME_CONTROL, *PFRAME_CONTROL;

typedef struct   _HEADER_802_11   {
    FRAME_CONTROL   FC;
    uint16_t          Duration;
    uint8_t           Addr1[WIFI_MAC_ADDRESS_LENGTH];
    uint8_t           Addr2[WIFI_MAC_ADDRESS_LENGTH];
    uint8_t           Addr3[WIFI_MAC_ADDRESS_LENGTH];
    uint16_t          Frag: 4;
    uint16_t          Sequence: 12;
#ifndef __CC_ARM
    uint8_t           Octet[0];
#endif
} PACKED  HEADER_802_11, *PHEADER_802_11;

typedef struct  _FRAME_802_11 {
    HEADER_802_11   Hdr;
    uint8_t            Octet[1];
} PACKED FRAME_802_11, *PFRAME_802_11;

typedef struct  {
    uint8_t   Eid;
    uint8_t   Len;
    int8_t   Octet[1];
} PACKED EID_STRUCT, *PEID_STRUCT, BEACON_EID_STRUCT, *PBEACON_EID_STRUCT;

typedef struct _NDIS_802_11_VARIABLE_IEs {
    uint8_t ElementID;
    uint8_t Length;    // Number of bytes in data field
    uint8_t data[1];
} NDIS_802_11_VARIABLE_IEs, *PNDIS_802_11_VARIABLE_IEs;

#define COPY_MAC_ADDR(Addr1, Addr2)   os_memcpy((Addr1), (Addr2), WIFI_MAC_ADDRESS_LENGTH)
#define CAP_IS_ESS_ON(x)                 (((x) & 0x0001) != 0)
#define CAP_IS_IBSS_ON(x)                (((x) & 0x0002) != 0)
#define CAP_IS_CF_POLLABLE_ON(x)         (((x) & 0x0004) != 0)
#define CAP_IS_CF_POLL_REQ_ON(x)         (((x) & 0x0008) != 0)
#define CAP_IS_PRIVACY_ON(x)             (((x) & 0x0010) != 0)
#define CAP_IS_SHORT_PREAMBLE_ON(x)      (((x) & 0x0020) != 0)
#define CAP_IS_PBCC_ON(x)                (((x) & 0x0040) != 0)
#define CAP_IS_AGILITY_ON(x)             (((x) & 0x0080) != 0)
#define CAP_IS_SPECTRUM_MGMT(x)          (((x) & 0x0100) != 0)  // 802.11e d9
#define CAP_IS_QOS(x)                    (((x) & 0x0200) != 0)  // 802.11e d9
#define CAP_IS_SHORT_SLOT(x)             (((x) & 0x0400) != 0)
#define CAP_IS_APSD(x)                   (((x) & 0x0800) != 0)  // 802.11e d9
#define CAP_IS_IMMED_BA(x)               (((x) & 0x1000) != 0)  // 802.11e d9
#define CAP_IS_DSSS_OFDM(x)              (((x) & 0x2000) != 0)
#define CAP_IS_DELAY_BA(x)               (((x) & 0x4000) != 0)  // 802.11e d9

#define CAP_GENERATE(ess,ibss,priv,s_pre,s_slot,spectrum)  (((ess) ? 0x0001 : 0x0000) | ((ibss) ? 0x0002 : 0x0000) | ((priv) ? 0x0010 : 0x0000) | ((s_pre) ? 0x0020 : 0x0000) | ((s_slot) ? 0x0400 : 0x0000) | ((spectrum) ? 0x0100 : 0x0000))

#define ERP_IS_NON_ERP_PRESENT(x)        (((x) & 0x01) != 0)    // 802.11g
#define ERP_IS_USE_PROTECTION(x)         (((x) & 0x02) != 0)    // 802.11g
#define ERP_IS_USE_BARKER_PREAMBLE(x)    (((x) & 0x04) != 0)    // 802.11g

// BSS Type definitions
#define BSS_ADHOC                       0  // = Ndis802_11IBSS
#define BSS_INFRA                       1  // = Ndis802_11Infrastructure
#define BSS_ANY                         2  // = Ndis802_11AutoUnknown
#define BSS_MONITOR                     3  // = Ndis802_11Monitor
#define SIZE_HT_CAP_IE                  26

// IE code
#define IE_SSID                         0
#define IE_SUPP_RATES                   1
#define IE_FH_PARM                      2
#define IE_DS_PARM                      3
#define IE_CF_PARM                      4
#define IE_TIM                          5
#define IE_IBSS_PARM                    6
#define IE_COUNTRY                      7     // 802.11d
#define IE_802_11D_REQUEST              10    // 802.11d
#define IE_QBSS_LOAD                    11    // 802.11e d9
#define IE_EDCA_PARAMETER               12    // 802.11e d9
#define IE_TSPEC                        13    // 802.11e d9
#define IE_TCLAS                        14    // 802.11e d9
#define IE_SCHEDULE                     15    // 802.11e d9
#define IE_CHALLENGE_TEXT               16
#define IE_POWER_CONSTRAINT             32    // 802.11h d3.3
#define IE_POWER_CAPABILITY             33    // 802.11h d3.3
#define IE_TPC_REQUEST                  34    // 802.11h d3.3
#define IE_TPC_REPORT                   35    // 802.11h d3.3
#define IE_SUPP_CHANNELS                36    // 802.11h d3.3
#define IE_CHANNEL_SWITCH_ANNOUNCEMENT  37    // 802.11h d3.3
#define IE_MEASUREMENT_REQUEST          38    // 802.11h d3.3
#define IE_MEASUREMENT_REPORT           39    // 802.11h d3.3
#define IE_QUIET                        40    // 802.11h d3.3
#define IE_IBSS_DFS                     41    // 802.11h d3.3
#define IE_ERP                          42    // 802.11g
#define IE_TS_DELAY                     43    // 802.11e d9
#define IE_TCLAS_PROCESSING             44    // 802.11e d9
#define IE_QOS_CAPABILITY               46    // 802.11e d6
#define IE_HT_CAP                       45    // 802.11n d1. HT CAPABILITY. ELEMENT ID TBD
#define IE_AP_CHANNEL_REPORT            51    // 802.11k d6
#define IE_HT_CAP2                         52    // 802.11n d1. HT CAPABILITY. ELEMENT ID TBD
#define IE_RSN                          48    // 802.11i d3.0
#define IE_WPA2                         48    // WPA2

#define IE_EXT_SUPP_RATES               50    // 802.11g
#define IE_SUPP_REG_CLASS               59    // 802.11y. Supported regulatory classes.
#define IE_EXT_CHANNEL_SWITCH_ANNOUNCEMENT  60  // 802.11n
#define IE_ADD_HT                         61    // 802.11n d1. ADDITIONAL HT CAPABILITY. ELEMENT ID TBD
#define IE_ADD_HT2                        53    // 802.11n d1. ADDITIONAL HT CAPABILITY. ELEMENT ID TBD
#define IE_NEW_EXT_CHA_OFFSET                         62    // 802.11n d1. New extension channel offset elemet
#define IE_EXT_CAPABILITY               127
#define IE_WPA                          221   // WPA
#define IE_VENDOR_SPECIFIC              221   // Wifi WMM (WME)
#define IE_WFA_WSC          221
#define IE_WIFI_DIRECT                  221
/* new types for Media Specific Indications */
/* Extension channel offset */
#define EXTCHA_NONE         0
#define EXTCHA_ABOVE        0x1
#define EXTCHA_BELOW        0x3

/* BW */
#define BAND_WIDTH_20       0
#define BAND_WIDTH_40       1
#define BAND_WIDTH_80       2
#define BAND_WIDTH_BOTH     3
#define BAND_WIDTH_10       4   /* 802.11j has 10MHz. This definition is for internal usage. doesn't fill in the IE or other field. */

/* BW */
#define BW_20       BAND_WIDTH_20
#define BW_40       BAND_WIDTH_40
#define BW_80       BAND_WIDTH_80
#define BW_10       BAND_WIDTH_10   /* 802.11j has 10MHz. This definition is for internal usage. doesn't fill in the IE or other field. */


#define MAX_SCAN_ARRAY_ITEM 4
#define MAX_SCAN_TABLE_SIZE MAX_SCAN_ARRAY_ITEM

extern uint8_t          g_scan_ssid[];
extern uint32_t         g_scan_ssid_len;
extern unsigned char    target_bssid[];
extern unsigned char    g_scanning;
extern unsigned char    g_scan_by_supplicant;
extern unsigned char    g_scan_by_app;
extern unsigned char    g_scan_pbc_ap;

typedef struct {
    struct __time       last_update;
    unsigned char       valid;
    unsigned char       scan_seq_number;
    unsigned char       is_config_ssid;
    BeaconProbeRspStr_t beacon; /* internal for mini-supplicant */
} SCAN_ARRAY_ITEM, *P_SCAN_ARRAY_ITEM;

typedef struct _FULL_SCAN_LIST {
    BeaconProbeRspStr_t     beacon;
    struct _FULL_SCAN_LIST  *next;
} FULL_SCAN_LIST, *P_FULL_SCAN_LIST;

typedef struct _CMD_IOT_START_SCAN_S {
    unsigned char partial;    // 0: full, 1: partial
    unsigned char scan_mode;   // 0: active (passive in regulatory channel), 1: passive in all channel, 2: force_active (active in all channel)
    unsigned char partial_bcn_cnt;
    unsigned char periodic_bcn_cnt; // 0: scan for once, x: auto re-scan after x beacon interval for background scan

    unsigned char enable_bssid; //0:disable, 1:enable
    unsigned char BSSID[WIFI_MAC_ADDRESS_LENGTH];
    unsigned char enable_ssid; // bit7 0:disable, 1:enable, bit[5:0] is length of SSID
    unsigned char SSID[32];

    ch_list_t channel_list;
} CMD_IOT_START_SCAN_T, *P_CMD_IOT_START_SCAN_T;

/**
*  Get the AP full information list of the last scan operation (for mini_supplicant)
*
* @parameter None
* @return Number of AP scaned and a BEACON data structure will be returned
* @note If no wifi_conn_start_scan() has been performed, an empty data structure is returned
*/
int32_t wifi_scan_get_full_list(FULL_SCAN_LIST *scan_list);
int wifi_scan_list_handler(unsigned int inf, unsigned char *body, int len);
void wifi_scan_reset(void);
BeaconProbeRspStr_t *wifi_scan_get_item(int i);
SCAN_ARRAY_ITEM *wifi_scan_table_array(void);
void sort_scan_array_by_rssi(void);

void BssCipherParse(
    uint16_t CapabilityInfo,
    unsigned char *auth_mode,
    unsigned char *encrypt_mode,
    uint16_t LengthVIE,
    uint8_t *VarIE);

int32_t wifi_config_get_scan_table_size(uint8_t *size);
int32_t wifi_config_set_scan_table_size(uint8_t size);
int wifi_scan_set_bssid(uint8_t *bssid);
int wifi_scan_ap_rescan(unsigned int inf, unsigned char *body, int len);
uint8_t wifi_beacon_probe_rsp_parser(
    void *Msg,
    uint32_t MsgLen,
    BeaconProbeRspStr_t *pRsp);
int32_t insert_list_by_rssi(wifi_scan_list_item_t data_to_insert);
int wifi_scan_add_scan_bssid(unsigned char *pbssid);
uint8_t get_cent_ch_by_htinfo(
    ADD_HT_INFO_IE *ht_op,
    HT_CAPABILITY_IE *ht_cap);

/**
*  Get the the scan param and fill channel list in the struct scan param, 
*
* @parameter[out] param_length  
* @return scan param pointer
* @note return NULL means failed, donot forget to free memory when use this function
*/
P_CMD_IOT_START_SCAN_T wifi_get_scan_param_and_channel_list(int32_t* param_length);

#endif /* __WIFI_SCAN_H */


