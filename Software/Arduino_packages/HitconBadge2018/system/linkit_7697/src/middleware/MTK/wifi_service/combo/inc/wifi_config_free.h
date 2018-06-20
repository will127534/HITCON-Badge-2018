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

#ifndef __WIFI_CONFIG_FREE_H__
#define __WIFI_CONFIG_FREE_H__

//#include "type_def.h"
#include "stdint.h"
#include "wifi_api.h"
#include "connsys_profile.h"
#include "memory_attribute.h"
#include "wifi_event.h"

//
//  ConfigureFree Demo - parse AP's credential for NVRAM Setting and reboot
//
typedef struct _IOT_CONFIG_FREE_IE {
    uint8_t Nounce[4];
    uint8_t Ssid[32];
    uint8_t SsidLen;
    uint8_t Bssid[6];
    uint8_t AuthMode;
    uint8_t EncrypType;
    uint8_t WpaPsk[64];
    uint8_t WpaPskLen;
    uint8_t Channel;
} IOT_CONFIG_FREE_IE, *P_IOT_CONFIG_FREE_IE;
typedef struct __configure_free_callback
{
	int32_t (*save_credential)(P_IOT_CONFIG_FREE_IE cred);
	int32_t (*save_ready)(uint8_t config_ready);
} CONFIGURE_FREE_CALLBACK;

void register_configure_free_callback(void *save_credential_func, void *save_ready_func);

int wifi_cf_credential_handler(unsigned int inf, unsigned char *body, int len);


/**
* @brief This function get  the device is configured (configfree ready) in the Flash memory or not. \n
* @param[out] config_ready indicates the ConfigFree flow has completed and the device is configured
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | ConfigFree un-configured: default value, ConfigFree Flow will be triggered by default|
* \b 1                          | ConfigFree configured|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_get_configfree(uint8_t *config_ready);

/**
* @brief This function configure the configfree ready in the Flash memory. \n
* The profile is read to initialize the system at boot up.
*
* @param[in] config_ready indicates the ConfigFree flow has completed
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | ConfigFree un-configured: default enable ConfigFree Flow|
* \b 1                          | ConfigFree configured|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note Use this function will reboot the device and boot in concurrent mode (opmode=3)
*/
int32_t wifi_profile_set_configfree(uint8_t config_ready);

#endif /* __WIFI_SCAN_H */
