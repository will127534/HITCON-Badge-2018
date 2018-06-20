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

#ifndef __SIE_H__
#define __SIE_H__

#include <stdint.h>

/** @file sie.h contains the API prototype declaration for Secure IE. */

typedef enum {
    SIE_VALIDATE_SUCCESS,
    SIE_VALIDATE_FAILURE
} sie_validate_t;


typedef struct sie_request_s {
    uint8_t nonce[4];
} sie_request_t;


typedef struct sie_respone_s {
    uint8_t     nonce[4];
    uint16_t    payload_len;
    uint8_t     payload[0];
} sie_response_t;


/**
 * Create one SecureIE request payload.
 *
 * @return The pointer of the allocated SecureIE request payload. NULL if
 *         memory allocation failed.
 */
sie_request_t *sie_request_init(void);


/**
 * Destroy one SecureIE request payload.
 *
 * @param request   The pointer to a SecureIE payload previously allocated
 *                  by sie_request_init().
 */
void sie_request_deinit(sie_request_t *request);


/**
 * Create one SecureIE response payload.
 *
 * During the creation of the SecureIE response payload, the nonce from
 * request is used.
 *
 * @return The pointer of the allocated SecureIE response payload. NULL if
 *         memory allocation failed.
 *
 * @note The returned response payload is encrypted.
 */
sie_response_t *sie_response_init(sie_request_t   *request,
                                  uint8_t         *payload,
                                  uint16_t        payload_len);


/**
 * Destroy one SecureIE response payload.
 *
 * @param response      The pointer to a SecureIE payload previously
 *                      allocated by sie_response_init().
 * @param response_len The length of response.
 */
void sie_response_deinit(sie_response_t *response,
                         uint16_t       response_len);


/**
 * Decrypt and validate a SecureIE response payload.
 *
 * @param response      The pointer to a SecureIE payload previously
 *                      allocated by sie_response_init().
 * @param response_len  The length of response.
 * @param request       The pointer to a SecureIE request payload to be
 *                      used to validate response.
 *
 * @retval SIE_VALIDATE_SUCCESS The decryption and validation succeeded.
 * @retval SIE_VALIDATE_FAILURE The decryption and validation failed.
 */
sie_validate_t sie_response_decrypt(sie_response_t  *response,
                                    uint16_t        reponse_len,
                                    sie_request_t   *request);

#endif /* __SIE_H__ */
