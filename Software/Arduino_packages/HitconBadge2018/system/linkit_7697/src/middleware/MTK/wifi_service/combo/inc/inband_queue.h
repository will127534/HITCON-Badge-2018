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

#ifndef __INBAND_QUEUE_H__
#define __INBAND_QUEUE_H__

#include <stdint.h>
#include "type_def.h"
#include "memory_attribute.h"
#include "connsys_adapter.h"
#include "mtk_ll.h"




#define CFG_INBAND_CMD_RSP_WAIT_TIME_VAL    5000 /* unit is ms */
#define CFG_INBAND_CMD_RSP_WAIT_PERIOD_VAL    10 /* unit is ms */
#define CFG_INBAND_CMD_RSP_FROM_N9_WAIT_TIME_VAL    3000 /* unit is ms */

#define CFG_INBAND_CMD_IS_QUERY_SET            0
#define CFG_INBAND_CMD_IS_QUERY_QUERY          1
#define CFG_INBAND_CMD_IS_QUERY_NEED_STATUS    2

/* CMD ACK BIT of ucExtCmdOption field */
#define EXT_CMD_OPT_BIT_0_ACK		BIT(0)

#define DBG_INBAND(FEATURE, _Fmt) \
        {                                       \
            if (g_inband_debug_feature & FEATURE)   \
            {               \
                printf _Fmt; \
            }   \
        }

#define INBAND_DBG_MAIN   (1<<0)


typedef enum _ENUM_INBNAD_HANDLE_T {
    INBAND_HANDLE_NON_HANDLE = 0,
    INBAND_HANDLE_RSP,
    INBAND_HANDLE_EVENT_HANDLER
} ENUM_INBNAD_HANDLE_T;

typedef enum _ENUM_INBNAD_RSP_STATUS_T {
    INBAND_NOT_RSP    = 0,
    INBAND_RSP_OK     = 1,
    INBAND_RSP_NO_BUF = 2,
    INBAND_RSP_SHORT  = 3,
    INBAND_RSP_LONG   = 4
} ENUM_INBNAD_RSP_STATUS_T;


typedef int (*P_INBAND_EVT_FUNC)(unsigned int inf, unsigned char *body, int len);

typedef struct _inband_evt_item_t {
    unsigned char         evt_id;
    P_INBAND_EVT_FUNC     func;
} inband_evt_item_t;

typedef void (*inband_handle_func_t)(void *pkt_ptr, unsigned char *payload, unsigned int len);

/*
 * forward declaraion to let P_INBAND_RSP_HANDLER able to recognize
 * P_CONNSYS_INBAND_REQ_T.
 */
typedef struct __CONNSYS_INBAND_REQ *P_CONNSYS_INBAND_REQ_T;

/*
 * inband command response handler function prototype.
 */
typedef int (*P_INBAND_RSP_HANDLER)(P_CONNSYS_INBAND_REQ_T req, unsigned int, unsigned int);

typedef struct __CONNSYS_INBAND_REQ {
    DL_LIST                 list;       /* used for rsp queue */
    uint8_t                   cmd_id;
    uint8_t                   seq;
    uint16_t                  in_len;
    uint8_t                   *in_buf;
    uint16_t                  out_len;
    uint8_t                   *out_buf;
    void                    *wait_sema; /* if not null => need wait */
    P_INBAND_RSP_HANDLER    rsp_handler;
    uint8_t                   need_rsp;
    uint8_t                   is_head;    /* used for DL_LIST head */
    uint8_t                   ret_status; /* record return status */
    uint8_t                   is_query;   /* used for query cmd, in-band header: ucReserved = 1 to indicate query */
    uint8_t                   completed;  /* used for busy looping case, instead of wait_semaphore */
    uint8_t                  is_ext_cmd;  /* for EXT in-band-cmd */
} CONNSYS_INBAND_REQ_T;

typedef struct _WIFI_CMD_FW_T
{
    /* DW#0 */
    // uint16_t     u2Length;
    uint16_t     u2TxByteCount;
    uint16_t     u2PQ_ID;            /* Must be 0x8000 (Port1, Queue 0) */

    /* DW#1 */
    uint8_t      ucCID;
    uint8_t      ucPktTypeID;        /* Must be 0xA0 (CMD Packet by long format) */
   // uint8_t      ucSetQuery;
    uint8_t	 ucReserved;
    uint8_t      ucSeqNum;

    /* DW#2 */
    uint8_t      ucD2B0Rev;          /* padding fields, hw may auto modify this field */
    uint8_t      ucExtenCID;         /* Extend CID for MT7603 */
    uint8_t      ucD2B2Rev;          /* padding fields, hw may auto modify this field */
    uint8_t      ucExtCmdOption;     /* Extend CID option */

    /* DW#3-7 */
    uint32_t     au4D3toD7Rev[5];    /* padding fields */

    uint8_t      aucBuffer[];
} wifi_cmd_fw_t, WIFI_CMD_FW_T, *P_WIFI_CMD_FW_T;




/**
 * Initialize inband queue modules.
 *
 * @retval 0 succeeded.
 * @retval 1 already initialized.
 * @retval 2 out-of-memory.
 * @retval 3 other initilization failure.
 */
uint8_t inband_queue_init(void);

int inband_queue_exec_ex(unsigned char  cmd_id,unsigned char  *buf,ssize_t len, unsigned char *ret_buf,ssize_t out_len, unsigned char  is_from_isr,unsigned char   is_ext_cmd);

int inband_queue_evt_handler(void *pkt_ptr, unsigned char *payload, unsigned int len);

int inband_default_rsp_handler(P_CONNSYS_INBAND_REQ_T req, unsigned int arg2, unsigned int arg3);

void inband_queue_register_callback(inband_handle_func_t func);

void inband_queue_handler(void *pkt_ptr, unsigned char *payload, unsigned int len);

uint32_t inband_get_mq_spaces(void);

int inband_enqueue_cmd_config(unsigned char cmd_id, unsigned char *buf, ssize_t len, unsigned char  *status,
            unsigned char  *ret_buf, ssize_t out_len, P_INBAND_RSP_HANDLER rsp_handler, unsigned char need_rsp,
            unsigned char need_wait, unsigned char is_query, unsigned char is_ext_cmd);

int inband_enqueue_cmd_config_to_wifi_task(unsigned char cmd_id, unsigned char *buf, ssize_t len, unsigned char  *status,
            unsigned char  *ret_buf, ssize_t out_len, P_INBAND_RSP_HANDLER rsp_handler, unsigned char need_rsp,
            unsigned char need_wait, unsigned char is_query, unsigned char is_ext_cmd);

#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
void inband_evt_rescan(uint8_t retry);
#endif
os_port_base_type inband_job_add(
    inband_handle_func_t    function,
    void                    *arg1,
    unsigned char           *arg2,
    unsigned int            arg3);
const inband_evt_item_t *inband_evt_get_handler(unsigned char evt_id);

PKT_HANDLE_RESULT_T connsys_deliver_inband(void *pkt, uint8_t *payload, uint32_t len);

#define EXT_CMD_ID_PM_STATE_CTRL			0x7

#endif /* INBAND_QUEUE */
