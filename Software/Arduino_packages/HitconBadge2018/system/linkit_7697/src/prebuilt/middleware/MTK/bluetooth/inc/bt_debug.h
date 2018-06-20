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

#ifndef __BT_DEBUG_H__
#define __BT_DEBUG_H__

/**
 * This header file describes the debug apis used by Bluetooth stack.
 */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "bt_type.h"
#include "bt_os_layer_api.h"

BT_EXTERN_C_BEGIN

/**
 * Assert function.
 */
#define BT_ASSERT(...) assert(__VA_ARGS__)
#define BT_FW_ASSERT(...) assert(__VA_ARGS__)

/**
 * Log function. Example usage: BT_LOGD("HCI", "test%d", 123);
 */
void bt_debug_log(const char *format, ...);
#define BT_LOGI(module_name, format, ...)   \
    bt_debug_log("[I][" module_name "] " format, ## __VA_ARGS__)

#define BT_LOGD(module_name, format, ...)   \
    bt_debug_log("[D][" module_name "] " format, ## __VA_ARGS__)

#define BT_LOGW(module_name, format, ...)   \
    bt_debug_log("[W][" module_name "] " format, ## __VA_ARGS__)

#define BT_LOGE(module_name, format, ...)   \
    bt_debug_log("[E][" module_name "] " format, ## __VA_ARGS__)

/**
 * Data print function.
 */
#define BT_PRINT_16_BYTE_BIG_ENDIAN(p) {register int i; for(i=0;i<16;i++) fprintf(stderr, "%02x ", (p)[i]); fprintf(stderr, "\n");}
#define BT_PRINT_8_BYTE_BIG_ENDIAN(p) {register int i; for(i=0;i<8;i++) fprintf(stderr, "%02x ", (p)[i]); fprintf(stderr, "\n");}

/**
 * For address debug printing.
 */

/**
 * @brief     Convert address to string.
 * @param[in] addr is address using bt_bd_addr_t.
 * @return    An addrss string with format: 11-22-33-44-55-66.
 */
const char *bt_debug_bd_addr2str(const bt_bd_addr_t addr);

/**
 * @brief     Convert address to string.
 * @param[in] addr is address using bt_bd_addr_t.
 * @return    An addrss string with format: LAP: 11-22-33, UAP: 44, NAP: 55-66.
 */
const char *bt_debug_bd_addr2str2(const bt_bd_addr_t addr);

/**
 * @brief     Convert address to string.
 * @param[in] addr is address using bt_bd_addr_t.
 * @return    An addrss string with format: [PUBLIC]11-22-33-44-55-66.
 */
const char *bt_debug_addr2str(const bt_addr_t *p);

/**
 * @brief     Convert address to string.
 * @param[in] addr is address using bt_bd_addr_t.
 * @return    An addrss string with format: [PUBLIC]LAP: 11-22-33, UAP: 44, NAP: 55-66.
 */
const char *bt_debug_addr2str2(const bt_addr_t *p);

BT_EXTERN_C_END

#endif /* __BT_DEBUG_H__ */
