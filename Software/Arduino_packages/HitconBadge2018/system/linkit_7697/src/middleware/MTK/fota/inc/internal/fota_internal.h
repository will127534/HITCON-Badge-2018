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

#ifndef __FOTA_H__
#define __FOTA_H__


#include <stdint.h>


/****************************************************************************
 *
 * ENUM and Macro
 *
 ****************************************************************************/


typedef enum {
    FOTA_STATUS_OK,
    FOTA_STATUS_IS_EMPTY,
    FOTA_STATUS_IS_FULL,
    FOTA_STATUS_ERROR_INVALD_PARAMETER, //< supplied parameter is not valid.
    FOTA_STATUS_ERROR_BLOCK_ALIGN,      //< partition not block-aligned.
    FOTA_STATUS_ERROR_OUT_OF_RANGE,     //< the partition ran out-of-space.
    FOTA_STATUS_ERROR_UNKNOWN_ID,       //< the partition is not known.
    FOTA_STATUS_ERROR_NOT_INITIALIZED,  //< FOTA is not initialized yet.
    FOTA_STATUS_ERROR_FLASH_OP,         //< Flash operation failed.
#if defined(MTK_FOTA_DUAL_IMAGE_ENABLE)
    FOTA_STATUS_ERROR_CONTROL_BLOCK_CORRUPTION,
    FOTA_STATUS_DOWNLOAD_FAIL,
#endif
} fota_status_t;


//#define FOTA_HEADER_MAGIC       (0xFEEDFACE)

#define FOTA_COMPRESSION_NONE   (0)
#define FOTA_COMPRESSION_LZMA2  (2)


/****************************************************************************
 *
 * Structure
 *
 ****************************************************************************/


/**
 * The header to be attached to the beginning of image files to signify
 * the image header. The header provides the length, target partition,
 * compression type, and checksum.
 *
 * The size of the header is 16 bytes.
 *
 * The header can be chained to support images of several partitions to be
 * concatenated into one upgrade.
 */
typedef struct fota_header_s {
    uint32_t    magic;       //< the magic number, always set as:
                             //< FOTA_HEADER_MAGIC.

    uint32_t    zipper : 4;  //< the compression type, currently supports
                             //< FOTA_COMPRESSION_NONE and
                             //< FOTA_COMPRESSION_LZMA2.

    uint32_t    target : 28; //< the partition ID. where this image should
                             //< be written to.

    uint32_t    length;      //< the legnth of the code. The 32 bytes of this
                             //< header is not included.

    uint32_t    checksum;    //< offset based on address when running
                             //< read/write API. Starts from 0, upperbound
                             //< is length.
} fota_header_t;



typedef struct fota_partition_s {
    uint32_t    id;
    uint32_t    address;
    uint32_t    length;
    uint32_t    offset; //< offset based on address when running read/write
                        //< API. Starts from 0, upperbound is length.
} fota_partition_t;


typedef struct fota_flash_s {
    fota_partition_t    *table;
    uint8_t             table_entries;
    uint32_t            block_size;
} fota_flash_t;


/****************************************************************************
 *
 * API Function Declaration
 *
 ****************************************************************************/


fota_status_t fota_header_init(fota_header_t    *flash,
                               uint32_t         target,
                               uint32_t         compression);



/**
 * Provide flash configuration to FOTA.
 *
 * This API takes the user supplied partition and flash information for
 * further API calls.
 *
 * @retval FOTA_STATUS_OK the API call ends successfully.
 *
 * @retval FOTA_STATUS_ERROR_INVALD_PARAMETER Some supplied parameter is
 * not valid. Such as the pointer is NULL, the block size is not a power
 * of 2.
 *
 * @retval FOTA_STATUS_ERROR_BLOCK_ALIGN one or more of the supplied
 * partitions is/are not block-aligned.
 */
fota_status_t fota_init(fota_flash_t *flash);


/**
 * Determines the <i>partition</i> contains data or not by reading first
 * 256 bytes and check the content is all 0xFF or not.
 *
 * @retval FOTA_STATUS_IS_EMPTY if the partition is empty.
 *
 * @retval FOTA_STATUS_IS_FULL if the partition is not empty.
 *
 * @retval FOTA_STATUS_ERROR_UNKNOWN_ID if the <i>partition</i> is not in
 * partition table.
 *
 * @retval FOTA_STATUS_ERROR_NOT_INITIALIZED if FOTA was not initialized.
 */
fota_status_t fota_is_empty(uint32_t partition);


/**
 * Make the <i>partition</i> empty (remove first block) and make
 * fota_is_empty() detects the partition is empty.
 *
 * @retval FOTA_STATUS_OK the API call ends successfully.
 *
 * @retval FOTA_STATUS_ERROR_FLASH_OP if the erase ended with error.
 *
 * @retval FOTA_STATUS_ERROR_UNKNOWN_ID if the <i>partition</i> is not in
 * partition table.
 *
 * @retval FOTA_STATUS_ERROR_NOT_INITIALIZED if FOTA was not initialized.
 */
fota_status_t fota_make_empty(uint32_t partition);


/**
 * Selects the offset of read/write pointer of the specified <i>partition</i>.
 *
 * This API must be called once before reading/writing to ANY partition to
 * ensure the current offset is correct.
 *
 * @retval FOTA_STATUS_OK the API call ends successfully.
 *
 * @retval FOTA_STATUS_ERROR_UNKNOWN_ID if the <i>partition</i> is not in
 * partition table.
 *
 * @retval FOTA_STATUS_ERROR_NOT_INITIALIZED if FOTA was not initialized.
 *
 * @retval FOTA_STATUS_OK the API call ends successfully.
 *
 * @retval FOTA_STATUS_ERROR_OUT_OF_RANGE if the specified offset goes
 * beyond the limit of this partition.
 */
fota_status_t fota_seek(uint32_t partition, uint32_t offset);


/**
 * Read the data at the current offset in the specified <i>partition</i> to
 * <i>buffer</i> and increments the read pointer to new offset.
 *
 * @note This API is supposed to be called multiple times because the dynamic
 * memory space is smaller than flash size for some partitions.
 *
 * @retval FOTA_STATUS_OK the API call ends successfully.
 *
 * @retval FOTA_STATUS_ERROR_UNKNOWN_ID if the <i>partition</i> is not in
 * partition table.
 *
 * @retval FOTA_STATUS_ERROR_INVALD_PARAMETER Some supplied parameter is not
 * valid. Such as the pointer is NULL, or the length is 0.
 *
 * @retval FOTA_STATUS_ERROR_FLASH_OP Flash erase/write failed.
 *
 * @retval FOTA_STATUS_ERROR_NOT_INITIALIZED if FOTA was not initialized.
 *
 * @retval FOTA_STATUS_ERROR_OUT_OF_RANGE if read the specified lengt from
 * the current offset goes beyond the limit of this partition.
 */
fota_status_t fota_read(uint32_t partition, uint8_t *buffer, uint32_t length);


/**
 * Write the data in <i>buffer</i> to flash partition and increments the
 * write pointer to new offset.
 *
 * @note This API is supposed to be called multiple times because the dynamic
 * memory space is smaller than flash size for some partitions.
 *
 * @retval FOTA_STATUS_OK the API call ends successfully.
 *
 * @retval FOTA_STATUS_ERROR_UNKNOWN_ID if the <i>partition</i> is not in
 * partition table.
 *
 * @retval FOTA_STATUS_ERROR_INVALD_PARAMETER Some supplied parameter is not
 * valid. Such as the pointer is NULL, or the length is 0.
 *
 * @retval FOTA_STATUS_ERROR_FLASH_OP Flash erase/write failed.
 *
 * @retval FOTA_STATUS_ERROR_NOT_INITIALIZED if FOTA was not initialized.
 *
 * @retval FOTA_STATUS_ERROR_OUT_OF_RANGE if read the specified lengt from
 * the current offset goes beyond the limit of this partition.
 */
fota_status_t fota_write(uint32_t partition, const uint8_t *buffer, uint32_t length);


/**
 * Copy data from <i>source</i> partition to <i>target</i> partition in
 * the unit of <i>length</i> bytes.
 *
 * This function moves the offset pointers of both partitions. Hence, further
 * <i>fota_read</i> and <i>fota_write</i> will not get the expected result.
 *
 * @retval FOTA_STATUS_OK the API call ends successfully.
 *
 * @retval FOTA_STATUS_ERROR_INVALD_PARAMETER Some supplied parameter is not
 * valid. Such as the pointer is NULL, or the length is 0.
 *
 * @retval FOTA_STATUS_ERROR_NOT_INITIALIZED if FOTA was not initialized.
 *
 * @retval FOTA_STATUS_ERROR_UNKNOWN_ID if the <i>source</i> or <i>target</i>
 * partition is not in partition table.
 */
fota_status_t fota_copy(uint32_t target,
                        uint32_t source,
                        uint8_t  *buffer,
                        uint32_t length);


#endif /* __FOTA_H__ */

