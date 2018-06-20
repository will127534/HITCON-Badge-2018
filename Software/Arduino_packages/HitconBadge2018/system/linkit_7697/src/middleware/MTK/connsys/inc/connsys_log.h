/* Copyright Statement:
 *
 * (C) 2017-2017  MediaTek Inc. All rights reserved.
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

/*
    Module Name:
    connsys_log.h

    Abstract:
    Provide CONNSYS module logging wrapping interface to unify the print mechanisms.

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
*/

#ifndef __CONNSYS_LOG_H__
#define __CONNSYS_LOG_H__


#include <stdint.h>


#ifdef OVERRIDE_LOG
#define ERR(mod, format...)                                 \
    do {                                                    \
        if (connsys_util_uart_slim_status())                \
                connsys_util_uart_slim_printf("! " format); \
            else                                            \
                LOG_E(mod, format);                         \
        } while (0)
#else
#define ERR(mod, format...)                                 \
                LOG_E(mod, format);
#endif


#ifdef OVERRIDE_LOG
#define INFO(mod, format...)                                \
    do {                                                    \
        if (connsys_util_uart_slim_status())                \
                connsys_util_uart_slim_printf("  " format); \
            else                                            \
                LOG_I(mod, format);                         \
        } while (0)
#else
#define INFO(mod, format...)                                \
                LOG_I(mod, format);
#endif


#ifdef OVERRIDE_LOG
#define CONNSYS_D(mod, msg)                                 \
    do {                                                    \
        if (connsys_util_uart_slim_status())                \
                connsys_util_uart_slim_printf msg;          \
            else                                            \
                DBG_CONNSYS(mod, msg);                      \
        } while (0)
#else
#define CONNSYS_D(mod, msg)                                 \
                DBG_CONNSYS(mod, msg);
#endif


#ifdef OVERRIDE_LOG
#define CONNSYS_DUMP_E(mod, msg, p, s)                                      \
    do {                                                                    \
        if (connsys_util_uart_slim_status())                                \
                connsys_util_uart_slim_dump('E', msg, (const char *)p, s);  \
            else                                                            \
                LOG_HEXDUMP_E(mod, msg, p, s);                              \
        } while (0)
#else
#define CONNSYS_DUMP_E(mod, msg, p, s)                                      \
                LOG_HEXDUMP_E(mod, msg, p, s);
#endif


#ifdef OVERRIDE_LOG
#define CONNSYS_DUMP_I(mod, msg, p, s)                                      \
    do {                                                                    \
        if (connsys_util_uart_slim_status())                                \
                connsys_util_uart_slim_dump('I', msg, (const char *)p, s);  \
            else                                                            \
                LOG_HEXDUMP_I(mod, msg, p, s);                              \
        } while (0)
#else
#define CONNSYS_DUMP_I(mod, msg, p, s)                                      \
                LOG_HEXDUMP_I(mod, msg, p, s);
#endif


int connsys_util_uart_slim_printf(
    const char *format,
    ...
    );


int connsys_util_uart_slim_dump(
    const char      level,
    const char      *msg,
    const char      *p,
    size_t          s
    );


void connsys_util_uart_slim_enable(
    uint8_t         enable
    );


uint8_t connsys_util_uart_slim_status(
    void
    );


#endif /* __CONNSYS_LOG_H__ */
