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

#ifndef BT_TIMER_EXTERNAL_H
#define BT_TIMER_EXTERNAL_H
#include <stdbool.h>
#include <stdint.h>

#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
#define __BT_TIMER_EXT_SUPPORT__
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BT_TIMER_EXT_INSTANCE_NUM 10
#define BT_TIMER_EXT_EXPIRED 0x80

#define BT_TIMER_EXT_STATUS_SUCCESS  0
#define BT_TIMER_EXT_STATUS_FAIL     -1
#define BT_TIMER_EXT_STATUS_OOM      -2
typedef int32_t bt_timer_ext_status_t;

/* For BT Timer EXT ID Structure. */
/*
 * +---------+-------------+--------+
 * |Module ID| Group Mask  | Timer  |
 * +---------+-------------+--------+
 */

#define BT_MODULE_TIMER_EXT   0x88000000 /**< Prefix of the timer ext module. */

#define BT_TIMER_EXT_GROUP_SINK_MASK  0x00010000
#define BT_TIMER_EXT_GROUP_SOURCE_MASK  0x00020000
#define BT_TIMER_EXT_GROUP_INVALID_MASK 0x00FFFFFF

#define BT_TIMER_EXT_GROUP_SINK(timer) \
        (BT_MODULE_TIMER_EXT | BT_TIMER_EXT_GROUP_SINK_MASK | (timer))

#define BT_TIMER_EXT_GROUP_SOURCE(timer) \
        (BT_MODULE_TIMER_EXT | BT_TIMER_EXT_GROUP_SOURCE_MASK | (timer))


/**
 * @brief                  Timeout callback function prototype
 * @param[in] timer_id     Timer ID
 * @param[in] data         User data saved in timer instance
 * @return                 None
 */
typedef void (*bt_timer_ext_timeout_callback_t) (uint32_t timer_id, uint32_t data);

/**
 * @brief                           Timeout instance structure
 */
typedef struct _bt_timer_ext_t{
    uint32_t timer_id;                /**<  module id + module defined id */
    uint32_t data;                    /**<  user data */
    uint32_t time_tick;               /**<  timer timeout in tick */
    bt_timer_ext_timeout_callback_t cb;       /**<  timer timeout callback function */
} bt_timer_ext_t;


void bt_timer_ext_init(void);

/**
 * @brief                   To start a timer
 * @param[in] timer_id      Timer ID
 * @param[in] data          User data saved in timer instance
 * @param[in] time_ms       timer timeout in ms
 * @param[in] cb            timer timeout callback function
 * @return                  bt_timer_ext_STATUS_SUCCESS if add timer success
                            bt_timer_ext_STATUS_OOM if timer reach max count
                            bt_timer_ext_STATUS_FAIL if double start
 */
bt_timer_ext_status_t bt_timer_ext_start(uint32_t timer_id, uint32_t data, uint32_t time_ms, bt_timer_ext_timeout_callback_t cb);

/**
 * @brief                   Stop a timer
 * @param[in] timer_id      Timer ID
 * @return                  bt_timer_ext_STATUS_SUCCESS if cancel timer success
                            bt_timer_ext_STATUS_FAIL if not found
 */
bt_timer_ext_status_t bt_timer_ext_stop(uint32_t timer_id);

void bt_timer_ext_check_status(void);
void bt_timer_ext_handle_interrupt(void);

/**
 * @brief                   Find a timer
 * @param[in] timer_id      Timer ID
 * @return                  A pointer to the timer instance
 */
bt_timer_ext_t* bt_timer_ext_find(uint32_t timer_id);



#ifdef __cplusplus
}
#endif

#endif /* BT_TIMER_EXTERNAL_H */

