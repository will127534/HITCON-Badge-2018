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
 * @file uart.h
 *
 *  This header file exposes the API for UART.
 *
 */

#ifndef __UART_H__
#define __UART_H__

#include "stdint.h"
#include "dma_sw.h"

#ifdef __cplusplus
extern "C" {
#endif

//WLS
#define   UART_WLS_8                      0x0003
#define   UART_WLS_7                      0x0002
#define   UART_WLS_6                      0x0001
#define   UART_WLS_5                      0x0000

//Parity
#define   UART_NONE_PARITY                0x0000
#define   UART_ODD_PARITY                 0x0008
#define   UART_EVEN_PARITY                0x0018
#define   UART_MARK_PARITY                0x0028
#define   UART_SPACE_PARITY               0x0038

//Stop bits
#define   UART_1_STOP                     0x0000
#define   UART_1_5_STOP                   0x0004  // Only valid for 5 data bits
#define   UART_2_STOP                     0x0004


typedef enum {
    UART_PORT0 = 0,
    UART_PORT1,
    UART_MAX_PORT
} UART_PORT;


void UART_HWInit(unsigned long uart_base);

/**
 * @brief  Put the UART data synchronously
 *
 * @param  c [IN] the character to be sent to host
 *
 * @return void
 *
 * @note Send character to peer device
 */
void uart_output_char(UART_PORT port_no, unsigned char c);         /* no LN->CRLN translation */


/**
 * @brief  Get the UART0 data asynchronously
 *
 * @param  void
 *
 * @return The value "-1" means failure while other values mean valid character
 *
 * @note No-waiting, or non-blocking, operation
 */
int getc_nowait(void);


/**
 * @brief  Get the UART data synchronously
 *
 * @param  void
 *
 * @return The character received from host
 *
 * @note Get character from peer device
 */
uint8_t uart_input_char(UART_PORT port_no);


/**
 * @brief  Get the UART data synchronously
 *
 * @param  u_port [IN] UART_PORT0 or UART_PORT1
 *
 * @return The character received from host
 *
 * @note Get character from peer device
 */
uint32_t uart_input_char_unblocking(UART_PORT port_no);


/**
 * @brief  Get the UART1 data asynchronously
 *
 * @param  void
 *
 * @return The value "-1" means failure while other values mean valid character
 *
 * @note No-waiting, or non-blocking, operation
 */
int uart1_getc_nowait(void);


/**
 * @brief  Initialize UART hardware
 *
 * @param  u_port [IN] UART_PORT0 or UART_PORT1
 *
 * @return void
 *
 * @note Initialize the UART before operations.
 */
void halUART_HWInit(UART_PORT u_port);

/**
 * @brief  Set baudrate and format for UART
 *
 * @param  u_port [IN] UART_PORT0 or UART_PORT1
 * @param  baudrate [IN] E.g., 115200, 921600
 * @param  databit [IN] UART_WLS_8, UART_WLS_7, UART_WLS_6, or UART_WLS_5
 * @param  parity [IN] UART_NONE_PARITY, UART_ODD_PARITY, UART_EVEN_PARITY, UART_MARK_PARITY, or UART_SPACE_PARITY
 * @param  stopbit [IN] UART_1_STOP, UART_1_5_STOP, or UART_2_STOP
 *
 * @return void
 *
 * @note Set the baudrate and format of UART before operations.
 */
void halUART_SetFormat(UART_PORT u_port, uint32_t baudrate, uint16_t databit, uint16_t parity, uint16_t stopbit);

/**
 * @brief  Get the DMA VFIFO length of UART Rx
 *
 * @param  u_port [IN] UART_PORT0 or UART_PORT1
 * @param  is_rx [IN] transmit channel or receive channel
 * @param  length [OUT] Length of the VFIFO in byte
 *
 * @return void
 *
 * @note Get the VFIFO length.
 */
void halUART_Get_VFIFO_Length(UART_PORT u_port, int32_t is_rx, uint32_t *length);

/**
 * @brief  Read data from UART Rx DMA VFIFO
 *
 * @param  u_port [IN] UART_PORT0 or UART_PORT1
 * @param  data [OUT] Buffer to store the UART data
 * @param  length [IN] Length to retrieve
 *
 * @return void
 *
 * @note Get the VFIFO data.
 */
void halUART_Read_VFIFO_Data(UART_PORT u_port, uint8_t *data, uint32_t length);

/**
 * @brief  Write data to UART Tx DMA VFIFO
 *
 * @param  u_port [IN] UART_PORT0 or UART_PORT1
 * @param  data [IN] Data buffer to be sent out
 * @param  length [IN] Length to send
 *
 * @return void
 *
 * @note Put the data to VFIFO.
 */
void halUART_Write_VFIFO_Data(UART_PORT u_port, const uint8_t *data, uint32_t length);

/**
 * @brief  Initialize the UART Tx DMA
 *
 * @param  u_port [IN] UART_PORT0 or UART_PORT1
 * @param  vfifo_buffer [IN] Memory address pointing to a buffer for DMA VFIFO working
 * @param  buffer_length [IN] Length of buffer in byte
 *
 * @return void
 *
 * @note Initialize of DMA VFIFO for UART Tx.
 */
void halUART_Tx_VFIFO_Initialize(UART_PORT u_port, uint8_t *vfifo_buffer, uint32_t buffer_length, uint32_t threshold);

/**
 * @brief  Initialize the UART Rx DMA
 *
 * @param  u_port [IN] UART_PORT0 or UART_PORT1
 * @param  vfifo_buffer [IN] Memory address pointing to a buffer for DMA VFIFO working
 * @param  buffer_length [IN] Length of buffer in byte
 * @param  alert_length [IN] Alert length
 * @param  threshold [IN] Threshold for DMA to interrupt MCU
 * @param  timeout [IN] Timeout value for DMA to interrupt MCU
 *
 * @return void
 *
 * @note Initialize of DMA VFIFO for UART Rx.
 */
void halUART_Rx_VFIFO_Initialize(UART_PORT u_port, uint8_t *vfifo_buffer, uint32_t buffer_length, uint32_t alert_length, uint32_t threshold, uint32_t timeout);

/**
 * @brief  Register callback function for receiving the incoming data event
 *
 * @param  u_port [IN] UART_PORT0 or UART_PORT1
 * @param  func [IN] function for receving the notification of incoming data
 *
 * @return void
 *
 * @note Register the callback to receive the event of data buffer exceeding threshold
 */
void halUART_VDMA_Rx_Register_Callback(UART_PORT u_port, VOID_FUNC func);

/**
 * @brief  Register callback function for receiving the DMA timeout event
 *
 * @param  u_port [IN] UART_PORT0 or UART_PORT1
 * @param  func [IN] function for receving the notification of timeout
 *
 * @return void
 *
 * @note Register the callback to receive the event of remaining data timeout
 */
void halUART_VDMA_Rx_Register_TO_Callback(UART_PORT u_port, VOID_FUNC func);

void uart_set_hardware_flowcontrol(UART_PORT u_port);
void uart_set_software_flowcontrol(UART_PORT u_port, uint8_t xon, uint8_t xoff, uint8_t escape_character);
void uart_disable_flowcontrol(UART_PORT u_port);

#ifdef HAL_SLEEP_MANAGER_ENABLED
void uart_set_sleep_mode(UART_PORT u_port);
void uart_unmask_send_interrupt(UART_PORT u_port);
void uart_mask_send_interrupt(UART_PORT u_port);
void uart_send_handler(UART_PORT u_port);
void uart_interrupt_handler(hal_nvic_irq_t irq_number);
#endif

int getc_nowait(void);
void uart_reset_default_value(UART_PORT u_port);
void uart_query_empty(UART_PORT u_port);

#ifdef __cplusplus
}
#endif

#endif
