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


#ifndef __WIFI_INBAND_PUBLIC_H__
#define __WIFI_INBAND_PUBLIC_H__

#include "stdint.h"
#include "type_def.h"

#include "wifi_os_api.h"

#include "syslog.h"
#include "mt_cmd_fmt.h"
#include "wifi_scan.h"
#include "connsys_util.h"
#include "connsys_driver.h"
#include "misc.h"


#include "inband_queue.h"
#include "memory_attribute.h"

#include "task_def.h"


#if (defined(__CC_ARM) || defined(__ICCARM__))
#include <stdio.h>
#endif



#ifdef __cplusplus
extern "C" {
#endif


#if 1

typedef struct {
    inband_handle_func_t function;
    void                 *arg1;
    unsigned char        *arg2;
    unsigned int         arg3;
} xINBAND_JOB_t, *PINBAND_JOB_t;

struct inband_callback_func {
    inband_handle_func_t  handle_func_callback;
};

typedef struct inband_queue_tcb_s {
    xQueueHandle            job_queue;
    CONNSYS_INBAND_REQ_T    queue_entries[ UNIFY_INBAND_QUEUE_LENGTH ];
    os_semaphore_t          tx_buf_semaphore;
    DL_LIST                 wait_list;
    DL_LIST                 idle_list;
    inband_handle_func_t    handler;
    unsigned char           inband_seq;
} inband_queue_tcb_t;
#endif

extern uint32_t g_inband_debug_feature;
extern inband_queue_tcb_t *iq_tcb;



#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
#ifdef MTK_WIFI_ROM_ENABLE
extern void (*net_pkt_free_evt)(void *ptr);
#else
extern void net_pkt_free_evt(void *ptr);
#endif
void inband_cm4_evt_handle(void* pkt_ptr, unsigned char *payload, unsigned int len);
int inband_evt_handle_from_CM4(void *skb, P_INIT_WIFI_EVENT_T  frame);
#endif

void inband_tx_buf_lock(inband_queue_tcb_t *iq_tcb);
void inband_tx_buf_unlock(inband_queue_tcb_t *iq_tcb);
P_CONNSYS_INBAND_REQ_T inband_malloc(inband_queue_tcb_t *iq_tcb);
void inband_mfree(inband_queue_tcb_t *iq_tcb, P_CONNSYS_INBAND_REQ_T req);
void inband_show_wifi_event_format(unsigned char *payload);
void inband_add_to_wait_list(inband_queue_tcb_t *iq_tcb, P_CONNSYS_INBAND_REQ_T req);

int inband_queue_query(unsigned char cmd_id, unsigned char *buf, ssize_t len, unsigned char *ret_buf, ssize_t out_len, unsigned char is_from_isr, unsigned char is_ext_cmd);

int inband_queue_exec(unsigned char cmd_id, unsigned char *buf, ssize_t len, unsigned char is_from_isr, unsigned char is_ext_cmd);

int inband_query_tx_frame(unsigned char cmd_id, unsigned char *buf, ssize_t len, unsigned char is_query, unsigned char is_ext_cmd);

int inband_queue_evt_handler(void *pkt_ptr, unsigned char *payload, unsigned int len);


#ifdef __cplusplus
}
#endif

#endif /* WIFI_INBAND_PUBLIC */

