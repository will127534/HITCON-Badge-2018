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
#ifndef __OUTBAND_API_H__
#define __OUTBAND_API_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * \file outband_api.h.
 *
 * This file lists the backdoors between processors.
 *
 * The backdoors are signaling channels used by processors to communicate with
 * each other. The purpose and usage of each of them is described in this
 * document.
 */


#ifndef WIFI_ADDRESS_REMAP
/**
 * Define this macro before including this header file to remapping addresses
 * to needed bus addresses.
 */
#define WIFI_ADDRESS_REMAP(_x)      (_x)
#endif


/****************************************************************************
 *
 * RAW CONTROL REGISTER DECLARATION
 *
 ****************************************************************************/


/*
 * This is a spare AON CR dedicated to be used for inter-processor
 * communication.
 */
#ifndef EINT0_CON
#define EINT0_CON                               WIFI_ADDRESS_REMAP(0x81040120)
#endif


/*
 * This is a spare AON CR dedicated to be used for inter-processor
 * communication.
 */
#ifndef EINT3_CON
#define EINT3_CON                               WIFI_ADDRESS_REMAP(0x81040150)
#endif


/*
 * This is a spare AON CR dedicated to be used for inter-processor
 * communication.
 */
#ifndef EINT5_CON
#define EINT5_CON                               WIFI_ADDRESS_REMAP(0x81040170)
#endif


/*
 * This is a spare AON CR dedicated to be used for inter-processor
 * communication.
 */
#ifndef AON_TOP_AON_RSV
#define AON_TOP_AON_RSV                         WIFI_ADDRESS_REMAP(0x81021138)
#endif


/*
 * This is a spare AON CR dedicated to be used for inter-processor
 * communication.
 *
 * The bits [31:7] are cr_reserved5.
 *
 * Currently allocated by software
 *     bit 15 - fastboot notification
 */
#ifndef AON_TOP_AON_RSV2
#define AON_TOP_AON_RSV2                        WIFI_ADDRESS_REMAP(0x81021258)
#endif


/*
 * This is a spare AON CR dedicated to be used for inter-processor
 * communication.
 *
 * All 32-bits are cr_reserved3.
 */
#ifndef AON_SW_SYNC0
#define AON_SW_SYNC0                            WIFI_ADDRESS_REMAP(0x81021250)
#endif


/*
 * This is a spare AON CR dedicated to be used for inter-processor
 * communication.
 *
 * All 32-bits are cr_reserved3.
 */
#ifndef AON_SW_SYNC1
#define AON_SW_SYNC1                            WIFI_ADDRESS_REMAP(0x81021254)
#endif


/****************************************************************************
 *
 * COMMUNICATION CONTROL REGISTER DECLARATION/ASSIGNMENTS
 *
 ****************************************************************************/


/* TODO: crystal 40M/26M : CONFG_STRAP_STA_XTAL_26M, this CR is read by N9
 * and set by CM4, software on CM4 decides which sort of XTAL is supported
 * by using compiler options. */


/**
 * CHIP ID and Revision number info.
 *
 * @note This register is reserved for S/W completely. The fields usage in it
 *       is solely defined by software and not awared by chip designers.
 */
#define HOST_TO_MCU_CHIP_ID_CR_1                (EINT0_CON)
#define HOST_TO_MCU_CHIP_ID_CR_2                (EINT3_CON)
#define HOST_TO_MCU_CHIP_ID_CR_3                (EINT5_CON)


/**
 * Wi-Fi Task State
 *
 * @note This register is reserved for S/W completely. The fields usage in it
 *       is solely defined by software and not awared by chip designers.
 */
#ifndef WIFI_SW_SYNC0
#define WIFI_SW_SYNC0                           (AON_SW_SYNC0)
#endif


/**
 * Wi-Fi ROM Initialize done
 *
 * @note This register is reserved for S/W completely. The fields usage in it
 *       is solely defined by software and not awared by chip designers.
 */
#ifndef WIFI_SW_SYNC1
#define WIFI_SW_SYNC1                           (AON_SW_SYNC1)
#endif


#define HOST_TO_MCU_CHIP_ID_CR                  ()


#define MCU_TO_HOST_CR                          (AON_TOP_AON_RSV)
#define HIF_IS_READY_BIT                        (0x8000)


#define HOST_TO_MCU_CR                          (AON_TOP_AON_RSV2)
#define SW_M4_TO_NOTIFY_N9_BIT                  (0x8000)
#define SW_INDICATE_DEFAULT_WIFI_CHANNEL_MASK   (0x7e00)
#define SW_INDICATE_DEFAULT_WIFI_CHANNEL_SHFT   (9)
#define SW_XTAL_COMPILER_OPTION_MASK            (0x007e)
#define SW_XTAL_COMPILER_OPTION_SHFT            (1)
#define SW_TOP_CFG_SW_SYNC_RESTORE_BIT          (1)


/****************************************************************************
 *
 * About Chip ID helpers...
 *
 ****************************************************************************/


/**
 * Get the chip ID
 */
#define ob_chip_id_get()        ( \
                                    ((HAL_REG_32(HOST_TO_MCU_CHIP_ID_CR_1) & 0xFF) << 8) | \
				    ((HAL_REG_32(HOST_TO_MCU_CHIP_ID_CR_2) & 0xFF)     )   \
                                )


/**
 * Get the chip revision
 */
#define ob_chip_ver_get()       (HAL_REG_32(HOST_TO_MCU_CHIP_ID_CR_3) & 0xFF) 


/**
 * Set the chip ID
 */
#define ob_chip_id_set(id)      do ( \
                                    HAL_REG_32(HOST_TO_MCU_CHIP_ID_CR_1) = (HAL_REG_32(HOST_TO_MCU_CHIP_ID_CR_1) & (~0xFF) | ((id >> 8) & 0xFF); \
                                    HAL_REG_32(HOST_TO_MCU_CHIP_ID_CR_2) = (HAL_REG_32(HOST_TO_MCU_CHIP_ID_CR_2) & (~0xFF) | ((id     ) & 0xFF); \
                                ) while (0)
/**
 * Set the chip revision
 */
#define ob_chip_ver_set(ver)    do { \
                                    HAL_REG_32(HOST_TO_MCU_CHIP_ID_CR_3) = (HAL_REG_32(HOST_TO_MCU_CHIP_ID_CR_3) & (~0xFF)) | (ver & 0xFF); \
				} while (0) 


/****************************************************************************
 *
 * About HIF readiness helpers...
 *
 * During F/W initialization phase, PSE is resetted, host needs to know when
 * the PSE is ready for writing inband commands into it. This flag is used
 * by MCU to notify host the readiness of HIF interface.
 *
 ****************************************************************************/


/**
 * check whether HIF is ready
 */
#define ob_hif_ready_check()    (HAL_REG_32(HOST_TO_MCU_CR) & HIF_IS_READY_BIT)


/**
 * notify that HIF is ready for access
 */
#define ob_hif_ready_set()      do { HAL_REG_32(MCU_TO_HOST_CR) |= HIF_IS_READY_BIT; } while (0)


/**
 * notify that HIF is not ready for access
 */
#define ob_hif_not_ready()      do { HAL_REG_32(MCU_TO_HOST_CR) &= (~HIF_IS_READY_BIT); } while (0)


/****************************************************************************
 *
 * About firmware FASTBOOT...
 *
 * Skip time consuming power-on calibrations in firmware during boot up.
 * After HIF is ready (check using ob_hif_is_ready()), host is responsible for
 * trigger full calibration using inband command.
 *
 * When system boots up, host set this flag before execute firmware on MCU.
 * Firmware reads this flag to decide whether the FASTBOOT is used.
 *
 * Example Usage:
 *
 *  1. power on connsys.
 *  2. download Wi-Fi firmware.
 *  3. ob_hif_ready_set(0).
 *  4. ob_fastboot_enable_set(1).
 *  5. start firmware.
 *  6. polling on ob_hif_ready_check() until it is true - F/W is ready.
 *  7. ob_fastboot_enable_set(0) - to clear the request.
 *  8. first-packet transmission.
 *  9. when possible, restart firmware initialization to get best performance.
 *
 ****************************************************************************/


/**
 * check whether FASTBOOT is enabled
 */
#define ob_fastboot_enable_check() (HAL_REG_32(HOST_TO_MCU_CR) & SW_M4_TO_NOTIFY_N9_BIT)


/**
 * notify that FASTBOOT is required
 */
#define ob_fastboot_enable_set()   do { HAL_REG_32(HOST_TO_MCU_CR) |= SW_M4_TO_NOTIFY_N9_BIT; } while (0)


/**
 * notify that FASTBOOT is not required
 */
#define ob_fastboot_enable_clear() do { HAL_REG_32(HOST_TO_MCU_CR) &= (~SW_M4_TO_NOTIFY_N9_BIT); } while (0)


/****************************************************************************
 *
 * About default channel
 *
 ****************************************************************************/


/**
 * Set default channel
 *
 * An application/protocol may choose a different default channel after Wi-Fi
 * firmware power-on.
 *
 * Normally, after power on calibration, firmware will lock the RF hardware at
 * channel 7. If an application wants the firmware to switch to a different
 * channel after power-on calibration. It can do so using this macro.
 *
 * @note This will not be effective if used after firmware is running but
 *       before MCU reset or firmware mode switching.
 */
#define ob_reset_channel_set(ch)    do { HAL_REG_32(HOST_TO_MCU_CR) = (HAL_REG_32(HOST_TO_MCU_CR) & (~SW_INDICATE_DEFAULT_WIFI_CHANNEL_MASK)) | ((ch << SW_INDICATE_DEFAULT_WIFI_CHANNEL_SHFT) & SW_INDICATE_DEFAULT_WIFI_CHANNEL_MASK); } while (0)


/**
 * Get default channel
 *
 * The default channel is usually hardcoded in Wi-Fi firmware as channel 7.
 * If no one configures the default channel, the return value of this API
 * should be 0.
 */
#define ob_reset_channel_get()      ((HAL_REG_32(HOST_TO_MCU_CR) & SW_INDICATE_DEFAULT_WIFI_CHANNEL_MASK) >> SW_INDICATE_DEFAULT_WIFI_CHANNEL_SHFT)


#ifdef __cplusplus
}
#endif

#endif /* __OUTBAND_API_H__ */

