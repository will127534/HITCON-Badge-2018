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

#ifndef __BT_SYSTEM_H__
#define __BT_SYSTEM_H__

#include "bt_platform.h"
#include "bt_type.h"

BT_EXTERN_C_BEGIN

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothCommon Common
 * @{
 * @addtogroup BluetoothCommonSYS_MEM Memory management
 * @{
 * This section defines the macros and API prototypes related to memory configuration. Apply memory configuration after the system boots up.
 *
 * @section memory_conifguration_usage How to use this module
 *
 * - Call the function #bt_memory_init_packet() to assign buffers for the Bluetooth stack. There are two buffers.
 *   One is the #BT_MEMORY_TX_BUFFER buffer to send commands and application data to the controller with a recommended size from 256 to 512 bytes.
 *   The other is the #BT_MEMORY_RX_BUFFER buffer to receive events and application data from the controller with a recommended size from 512 to 1024 bytes.
 * - Call the function #bt_memory_init_control_block() to assign buffers with fixed block size for the Bluetooth stack. There are two buffers.
 *   One is the #BT_MEMORY_CONTROL_BLOCK_TIMER buffer for the software timer usage in the Bluetooth stack.
 *   The other is the #BT_MEMORY_CONTROL_BLOCK_LE_CONNECTION buffer to store connection information in the stack.
 *
 *  - Sample code:
 *      @code
 *          #define BT_CONNECTION_MAX   5
 *          #define BT_TIMER_NUM 10
 *          #define BT_TX_BUF_SIZE 512
 *          #define BT_RX_BUF_SIZE 1024
 *          #define BT_TIMER_BUF_SIZE (BT_TIMER_NUM * BT_CONTROL_BLOCK_SIZE_OF_TIMER) // For more details, please refer to BT_CONTROL_BLOCK_SIZE_OF_TIMER.
 *          #define BT_CONNECTION_BUF_SIZE (BT_CONNECTION_MAX* BT_CONTROL_BLOCK_SIZE_OF_LE_CONNECTION) // For more details, please refer to BT_CONTROL_BLOCK_SIZE_OF_LE_CONNECTION.
 *          static char timer_cb_buf[BT_TIMER_BUF_SIZE];
 *          static char connection_cb_buf[BT_CONNECTION_BUF_SIZE];
 *          static char tx_buf[BT_TX_BUF_SIZE];
 *          static char rx_buf[BT_RX_BUF_SIZE];
 *          bt_bd_addr_t local_public_addr = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66}; // Initialize the public_addr according to your requirement based on application.
 *
 *          // Call memory initialization and task creation in the bt_create_task() only once.
 *          static void bt_create_task(void)
 *          {
 *               //Configurate the radio transmission power level for bt and le connections. the transmission power settings is not mandatory. 
 *               bt_config_tx_power_t tx_config_data = {.bt_init_tx_power_level = 5,
 *                                                      .bt_max_tx_power_level = 6,
 *                                                      .le_init_tx_power_level = 6};
 *               bt_config_tx_power_level(&tx_power_info);
 *               
 *               //Configurate the memory blocks.
 *               bt_memory_init_packet(BT_MEMORY_TX_BUFFER, tx_buf, BT_TX_BUF_SIZE);
 *               bt_memory_init_packet(BT_MEMORY_RX_BUFFER, rx_buf, BT_RX_BUF_SIZE);
 *               bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_TIMER, timer_cb_buf, BT_TIMER_BUF_SIZE);
 *               bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_LE_CONNECTION, connection_cb_buf,
 *                                 BT_CONNECTION_BUF_SIZE);
 *
 *               // Create a Bluetooth task. The Bluetooth will power on automatically after the task is created.
 *               if (pdPASS != xTaskCreate(bt_task, "bt_task", 2048, (void *)local_public_addr, 5, NULL)) {
 *                   printf("Cannot create a bt_task.");
 *               }
 *          }
 *      @endcode
 *
 */


/**
 * @defgroup BluetoothCommonhbif_memory_define Define
 * @{
 */


/* -------------- The buffer size of a single control block. --------------------- */
#define BT_CONTROL_BLOCK_SIZE_OF_TIMER            (20)     /**< The size of a control block for Timer. */
#define BT_CONTROL_BLOCK_SIZE_OF_LE_CONNECTION    (100)    /**< The size of a control block for Bluetooth LE connection. */
#define BT_CONTROL_BLOCK_SIZE_OF_EDR_CONNECTION   (56)     /**< The size of a control block for BR/EDR connection. */
#define BT_CONTROL_BLOCK_SIZE_OF_RFCOMM           (48)     /**< The size of a control block for RFCOMM. */
#define BT_CONTROL_BLOCK_SIZE_OF_HFP              (76)     /**< The size of a control block for HFP. */
#define BT_CONTROL_BLOCK_SIZE_OF_AVRCP            (48)     /**< The size of a control block for AVRCP. */
#define BT_CONTROL_BLOCK_SIZE_OF_A2DP_SEP         (16)     /**< The size of a control block for A2DP_SEP. */
#define BT_CONTROL_BLOCK_SIZE_OF_A2DP             (132)    /**< The size of a control block for A2DP. */
#define BT_CONTROL_BLOCK_SIZE_OF_PBAPC            (100)    /**< The size of a control block for PBAPC. */
#define BT_CONTROL_BLOCK_SIZE_OF_SPP              (44)     /**< The size of a control block for SPP. */
#define BT_CONTROL_BLOCK_SIZE_OF_AWS              (100)     /**<The size of a control block size for AWS. */
#define BT_CONTROL_BLOCK_SIZE_OF_GATT             (32)     /**<The size of a control block size for GATT. */

/**
* @brief     Bluetooth memory control block types.
*/
typedef enum {
    BT_MEMORY_CONTROL_BLOCK_TIMER = 0,          /**< Timer. */
    BT_MEMORY_CONTROL_BLOCK_LE_CONNECTION,      /**< Bluetooth LE connection. */
    BT_MEMORY_CONTROL_BLOCK_EDR_CONNECTION,     /**< BR/EDR connection. */
    BT_MEMORY_CONTROL_BLOCK_RFCOMM,             /**< RFCOMM. */
    BT_MEMORY_CONTROL_BLOCK_AVRCP,              /**< AVRCP. */
    BT_MEMORY_CONTROL_BLOCK_PBAPC,              /**< PBAPC. */
    BT_MEMORY_CONTROL_BLOCK_A2DP_SEP,           /**< A2DP SEP. */
    BT_MEMORY_CONTROL_BLOCK_A2DP,               /**< A2DP. */
    BT_MEMORY_CONTROL_BLOCK_HFP,                /**< HFP. */
    BT_MEMORY_CONTROL_BLOCK_SPP,                /**< SPP. */
    BT_MEMORY_CONTROL_BLOCK_AWS,                /**< AWS. */
    BT_MEMORY_CONTROL_BLOCK_GATT,               /**< GATT. */
    BT_MEMORY_CONTROL_BLOCK_NUM                 /**< The total number of memory control block types. */
} bt_memory_control_block_t;

/**
 * @brief    Bluetooth memory buffer types for the Memory Management module.
 * @{
 */
typedef enum {
    BT_MEMORY_TX_BUFFER = 0,     /**< TX packet buffer.*/
    BT_MEMORY_RX_BUFFER          /**< RX packet buffer.*/
} bt_memory_packet_t;

#define BT_MEMORY_TX_BUFFER_AVAILABLE_IND     (BT_MODULE_MM | 0x00010000)     /**< A buffer availability indication with #bt_memory_tx_buffer_available_ind_t. */
#define BT_MEMORY_FREE_GARBAGE_IND            (BT_MODULE_MM | 0x00010001)      /**< Ask app to free garbage memory with NULL payload. */

/**
 * @}
 */
/**
 * @}
 */

/**
* @defgroup Bluetoothhbif_memory_struct Struct
* @{
*/

/**
*  @brief TX buffer availability indication.
*/
typedef struct {
    uint32_t      size; /**< The size of available TX buffer in bytes. */
} bt_memory_tx_buffer_available_ind_t;

/**
* @}
*/
/**
 * @brief     This function initializes a TX/RX packet buffer for the Bluetooth stack.
 * @param[in] type is #BT_MEMORY_TX_BUFFER or #BT_MEMORY_RX_BUFFER.
 * @param[in] buf  is a pointer to the buffer.
 * @param[in] size is the size of the buffer in bytes.
 * @return    void.
 */
void  bt_memory_init_packet(bt_memory_packet_t type, char *buf, uint32_t size);
/**
 * @brief     This function initializes a control block buffer for the Bluetooth stack.
 * @param[in] type is #BT_MEMORY_CONTROL_BLOCK_TIMER or #BT_MEMORY_CONTROL_BLOCK_LE_CONNECTION.
 * @param[in] buf  is a pointer to the buffer.
 * @param[in] size is the size of the buffer, in bytes.
 * @return    void.
 */
void  bt_memory_init_control_block(bt_memory_control_block_t type, char *buf, uint32_t size);

/**
 * @}
 */

/**
 * @addtogroup BluetoothCommonPanic Panic
 * @{
 * This section defines the event, structure and types for Bluetooth panic feature.
 *
 * @section bt_panic_usage How to use this module
 *
 *   The panic event is triggered when Bluetooth host or controller has asserted or crashed. Once the panic event is received, the Bluetooth has to reboot.
 *  - Sample code:
 *      @code
 *          // The application reboots the Bluetooth, once Bluetooth panic event is received.
 *          bool is_panic = false;
 *          bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buf)
 *          {
 *              switch (msg)
 *              {
 *                  case BT_PANIC:
 *                      is_panic = true;
 *                      if (((bt_panic_t *)buff)->source == BT_PANIC_SOURCE_HOST) {
 *                          // Host is the source of panic event.
 *                      } else {
 *                          // Controller is the source of panic event.
 *                      }
 *
 *                      if (bt_power_off()) {
 *                          // The Bluetooth powered off successfully.
 *                      } else {
 *                          // The Bluetooth power off has failed.
 *                      }
 *                      break;
 *                  case BT_POWER_OFF_CNF:
 *                      {
 *                          if (is_panic == true) {
 *                              if(bt_power_on(NULL,NULL)) {
 *                                  // The Bluetooth powered off successfully.
 *                              } else {
 *                                  // The Bluetooth power off has failed.
 *                              }
 *                          }
 *                      }
 *                      break;
 *                  case BT_POWER_ON_CNF:
 *                      {
 *                          if (is_panic == true) {
 *                              is_panic = false;
 *                          }
 *                      }
 *                      break;
 *                  default:
 *                      break;
 *              }
 *              return status;
 *          }
 *      @endcode
 *
 */

/**
 * @defgroup Bluetoothbt_panic_define Define
 * @{
 * Define Bluetooth panic event and source types.
 */

/**
 * @brief    Bluetooth panic event.
 */
#define BT_PANIC                            (BT_MODULE_SYSTEM | 0x0003)    /**< Bluetooth host or controller asserted or crashed, the application has to reboot the Bluetooth.*/


/**
 * @brief    Bluetooth panic source type.
 */
#define BT_PANIC_SOURCE_HOST                0                              /**< Bluetooth host has asserted or crashed. */
#define BT_PANIC_SOURCE_CONTROLLER          1                              /**< Bluetooth controller has asserted or crashed. */

/**
 * @}
 */

/**
 * @defgroup Bluetoothbt_panic_struct Struct
 * @{
 * Define the Bluetooth panic structure.
 */

/**
 *  @brief the Bluetooth panic event parameter.
 */

typedef struct {
    uint8_t source; /**< The source for the panic, such as #BT_PANIC_SOURCE_HOST or #BT_PANIC_SOURCE_CONTROLLER. */
} bt_panic_t;

/**
 * @}
 * @}
 */

/**
 * @addtogroup BluetoothCommonSYS_POWER Power management
 * @{
 * This section defines the confirmation types and APIs to switch the power on and off.
 *
 * @section bt_power_management_usage How to use this module
 *
 *   The application layer can power on the Bluetooth if it is powered off and a stack memory is assigned.
 *  - Sample code:
 *      @code
 *          // Power on the Bluetooth from the application.
 *          void user_application_power_on_function()
 *          {
 *              // Initialize the public_addr and the random_addr according to the application requirements.
 *              bt_bd_addr_t public_addr = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
 *              bt_bd_addr_t random_addr = {0xc0, 0x22, 0x33, 0x44, 0x55, 0xc6};
 *
 *              bt_power_on(&public_addr, &random_addr);
 *          }
 *
 *          // The application receives the power on confirmation event in the function #bt_app_event_callback() after calling #bt_power_on().
 *          bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buf)
 *          {
 *              switch (msg)
 *              {
 *                  case BT_POWER_ON_CNF:
 *                      // Add the implementation flow after the Bluetooth is powered on.
 *                      break;
 *                  case BT_GAP_LE_CONNECT_IND:
 *                      // Connection is established.
 *                      break;
 *                  default:
 *                      break;
 *              }
 *              return status;
 *          }
 *      @endcode
 *
 */

/**
 * @defgroup Bluetoothhbif_power_define Define
 * @{
 * Define the power on or off events.
 */

#define BT_POWER_ON_CNF                     (BT_MODULE_SYSTEM | 0x0001)    /**< Power on confirmation event with #bt_power_on_cnf_t. */
#define BT_POWER_OFF_CNF                    (BT_MODULE_SYSTEM | 0x0002)    /**< Power off confirmation event with NULL payload. */

/**
 * @}
 */

/**
 * @defgroup Bluetoothhbif_power_struct Struct
 * @{
 * Define power on event parameter structure.
 */

/**
 *  @brief Power on confirmation structure.
 */
typedef struct {
    const bt_bd_addr_t      local_public_addr; /**< The local public address of the device. */
} bt_power_on_cnf_t;

/**
 * @brief This structure defines the detail information about config tx power level.
 */
typedef struct {
    uint8_t    bt_init_tx_power_level; /**< Initial level of bt connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power is. */
    uint8_t    bt_max_tx_power_level;  /**< Maximum level of bt connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power is. */
    uint8_t    le_init_tx_power_level; /**< Initial level of le connection radio transmission power. The range is between 0 and 7. The larger the value, the stronger the TX power is. */
} bt_config_tx_power_t;

/**
 * @}
 */

/**
 * @brief     This function powers on the Bluetooth. The application layer receives #BT_POWER_ON_CNF with #bt_power_on_cnf_t after the Bluetooth is powered on.
              Make sure the memory is initialized, the Bluetooth task is created and Bluetooth is powered on before calling any other Bluetooth APIs.
 * @param[in] public_addr   is a pointer to a given public address (6 bytes).
 *                          If public_addr is NULL, the controller will generate a public address and provide it to the user using #BT_POWER_ON_CNF with #bt_power_on_cnf_t.
 *                          If public_addr is not NULL, the address in the #BT_POWER_ON_CNF is same as public_addr.
 *                          The given public address should not be [00-00-00-00-00-00].
 * @param[in] random_addr   is a pointer to a given random address(6 bytes).
 * @return                  #BT_STATUS_SUCCESS, the power on request has been sent successfully.
 *                          #BT_STATUS_FAIL, the operation fails due to the same operation is ongoing.
 *                          #BT_STATUS_OUT_OF_MEMORY, the operation fails due to out of memory.
 */
bt_status_t bt_power_on(bt_bd_addr_ptr_t public_addr, bt_bd_addr_ptr_t random_addr);

/**
 * @brief     This function powers off the Bluetooth. The SDK will not send disconnect requests to the existing connections during the power off operation.
 *            The application layer will receive #BT_POWER_OFF_CNF after the Bluetooth is powered off.
 * @return    #BT_STATUS_SUCCESS, the power off request has been sent successfully.
 *            #BT_STATUS_FAIL, the operation fails due to the same operation is ongoing.
 *            #BT_STATUS_OUT_OF_MEMORY, the operation fails due to out of memory.
 */
bt_status_t bt_power_off(void);

/**
 * @brief     This function configurates the radio transmission power level for bt and le connections. The transmission power settings is not mandatory.
              User can configurate the radio transmission power before power on Bluetooth, and the configuration will not be reset until chip reboot.
 * @param[in] tx_power_info         is a pointer to a structure that specifies the parameters for radio transmission power level.
 * @return    #BT_STATUS_SUCCESS, the operation success.
 *            #BT_STATUS_FAIL, the operation fails. because the Bluetooth is power on or bt_init_tx_power_level is bigger than bt_max_tx_power_level.
 */
bt_status_t bt_config_tx_power_level(const bt_config_tx_power_t *tx_power_info);

/**
 * @brief   This function is a static callback for the application to listen to the event. Provide a user-defined callback.
 * @param[in] msg     is the callback message type.
 * @param[in] status  is the status of the callback message.
 * @param[in] buf     is the payload of the callback message.
 * @return            The status of this operation returned from the callback.
 */
bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buf);

/**
 * @brief   This function is for memory allocation from Rx buffer by application.
 * @param[in] buffer_size   is the allocation size requested.
 * @return            The point to Rx buffer allocated.
 */
void *bt_alloc_rx_buffer_from_external(uint32_t buffer_size);

/** @}
 *  @}
 *  @}
 */

BT_EXTERN_C_END

#endif

