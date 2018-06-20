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

#ifndef __BT_SDP_H__
#define __BT_SDP_H__

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothSDP SDP
 * @{
 * This section defines the server part API of the service discovery protocol, so that the user can add the customized records.
 * @{
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b SDP                        | Service Discovery Protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Bluetooth#SDP">Wikipedia</a>. |
 * |\b L2CAP                      | Logical Link Control And Adaptation Protocol Specification. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#Logical_link_control_and_adaption_protocol_.28L2CAP.29">Wikipedia</a>. |
 * |\b ATT                        | Attribute Protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#Low_Energy_Attribute_Protocol_.28ATT.29">Wikipedia</a>.|
 * |\b UUID                       | Universally Unique Identifier. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Universally_unique_identifier">Wikipedia</a>. |
 * |\b RFCOMM                     | Radio Frequency Communications. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Bluetooth#RFCOMM">Wikipedia</a>. |
 * |\b OBEX                       | Object exchange. For more information, please refer to <a href="https://en.wikipedia.org/wiki/OBject_EXchange">Wikipedia</a>. |
 * |\b SIM                        | Subscriber identity module. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Subscriber_identity_module">Wikipedia</a>. |
 * |\b URL                        | Uniform resource locator. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Uniform_Resource_Locator">Wikipedia</a>. |
 *
 * @section bt_sdp_api_usage How to use this module
 *
 * - Add a customized record with the service name, language list, service class ID list and the protocol description list for a sample profile.
 *   Please add your own record according to the SDP interoperability requirements in the related profile specification. To add customized records with the SDP macros and API:
 *  - Sample code:
 *     @code
 *        static const uint8_t bt_sample_service_name[] =
 *        {
 *            BT_SDP_TEXT_8BIT(7), // The string length of the service name.
 *            'S', 'a', 'm', 'p', 'l', 'e','\0'
 *        };
 *
 *        static const uint8_t bt_sample_language_list[] =
 *        {
 *            BT_SDP_ATTRIBUTE_HEADER_8BIT(9),// In this example, each item is 3 bytes and the total length of the following list items is 9 bytes.
 *
 *            // The language is defined as "en". A language identifier represents the natural language. Please refer to the ISO 639:1988 (E/F): "Code for the representation of names of languages" and
 *            // the ISO 639 Codes (Names of Languages) at "http://userpage.chemie.fu-berlin.de/diverse/doc/ISO_639.html".
 *            BT_SDP_UINT_16BIT(0x656E),
 *
 *            // The encoding is defined as UTF-8. A character encoding identifier which can be found in IANA'S database and
 *            // the Character Sets at "http://www.isi.edu/in-notes/iana/assignments/character-sets".
 *            BT_SDP_UINT_16BIT(0x006A),
 *
 *            // A base attribute ID for the natural language in the service record.
 *            BT_SDP_UINT_16BIT(0x0100)
 *        };
 *
 *        static const uint8_t bt_sample_service_class_id_list[] =
 *        {
 *            BT_SDP_ATTRIBUTE_HEADER_8BIT(6),                    // In this example, each item is 3 bytes and the total length of the following list items is 6 bytes.
 *            BT_SDP_UUID_16BIT(BT_SDP_SERVICE_CLASS_HANDSFREE),
 *            BT_SDP_UUID_16BIT(BT_SDP_SERVICE_CLASS_GENERIC_AUDIO)
 *        };
 *
 *        static const uint8_t bt_sample_protocol_description_list[] =
 *        {
 *            BT_SDP_ATTRIBUTE_HEADER_8BIT(12),         // The total length of all list items is 12 bytes.
 *
 *            BT_SDP_ATTRIBUTE_HEADER_8BIT(3),          // The total length of the following list item is 3 bytes.
 *            BT_SDP_UUID_16BIT(BT_SDP_PROTOCOL_L2CAP),
 *

 *            BT_SDP_ATTRIBUTE_HEADER_8BIT(5),          // The total length of the following two list items is 5 bytes.
 *            BT_SDP_UUID_16BIT(BT_SDP_PROTOCOL_RFCOMM),
 *            BT_SDP_UINT_8BIT(0x01)
 *        };

 *        static const bt_sdps_attribute_t bt_sample_sdp_attributes[] =
 *        {
 *            BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_SERVICE_CLASS_ID_LIST, bt_sample_service_class_id_list),
 *            BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_PROTOCOL_DESC_LIST, bt_sample_protocol_description_list),
 *            BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_LANG_BASE_ID_LIST, bt_sample_language_list),
 *            //Service name must add a base attribute ID, default is 0x0100, please refer to your profile.
 *            BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_SERVICE_NAME+0x0100, bt_sample_service_name)
 *        };
 *
 *        static const bt_sdps_record_t bt_sample_sdp_record =
 *        {
 *            .attribute_list_length = sizeof(bt_sample_sdp_attributes),
 *            .attribute_list = bt_sample_sdp_attributes,
 *        };
 *
 *        static const bt_sdps_record_t *sdps_sample_record[]= {
 *            &bt_sample_sdp_record,
 *        };
 *
 *        // This API invoked by the SDK process should be implemented by the application.
 *        // If no records to be found by the remote device, set the record_list to NULL and return 0.
 *        uint8_t bt_sdps_get_customized_record(const bt_sdps_record_t *** record_list)
 *        {
 *            *record_list = &sdps_sample_record;
 *            return sizeof(sdps_sample_record)/sizeof(bt_sdps_record_t*);
 *        }
 *     @endcode
 */


#include <stdint.h>
#include "bt_type.h"

BT_EXTERN_C_BEGIN

/**
 * @defgroup Bluetoothhbif_sdp_define Define
 * @{
 * This section defines the macros for the SDP server.
 */

/**
 * @brief Data elements begin with a single byte that contains both type and
 *        size information.
 *
 *        To create the first byte of a data element, bitwise "or" the
 *        #bt_sdp_data_element_type_t and #bt_sdp_data_element_size_t values into a single byte.
 *
 *        For example, a standard 16 bits unsigned integer with a value of 0x57
 *        could be encoded as follows:
 *
 *        uint8_t val[3] = { BT_SDP_DATA_ELEMENT_TYPE_UINT | BT_SDP_DATA_ELEMENT_SIZE_2BYTE, 0x00, 0x57};
 *
 *        The text string "hello" could be encoded as follows:
 *
 *        uint8_t str[7] = { BT_SDP_DATA_ELEMENT_TYPE_TEXT | BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_8BIT, 0x05, 'h','e','l','l','o'};
 */
typedef uint8_t bt_sdp_data_element_type_t; /**< Define the type of data element. */

#define BT_SDP_DATA_ELEMENT_TYPE_NIL  0x00  /**< The NULL type. It requires a size of #BT_SDP_DATA_ELEMENT_SIZE_1BYTE, which means an actual size of 0 for this type. */

#define BT_SDP_DATA_ELEMENT_TYPE_UINT 0x08  /**< The element type for unsigned integer. The profile with this element type must use a size type of
                                                 #BT_SDP_DATA_ELEMENT_SIZE_1BYTE, #BT_SDP_DATA_ELEMENT_SIZE_2BYTE, #BT_SDP_DATA_ELEMENT_SIZE_4BYTE, #BT_SDP_DATA_ELEMENT_SIZE_8BYTE or #BT_SDP_DATA_ELEMENT_SIZE_16BYTE.*/

#define BT_SDP_DATA_ELEMENT_TYPE_SINT 0x10  /**< The element type for a signed integer. The profile with this element type must use a size type of
                                                 #BT_SDP_DATA_ELEMENT_SIZE_1BYTE, #BT_SDP_DATA_ELEMENT_SIZE_2BYTE, #BT_SDP_DATA_ELEMENT_SIZE_4BYTE, #BT_SDP_DATA_ELEMENT_SIZE_8BYTE or #BT_SDP_DATA_ELEMENT_SIZE_16BYTE.*/

#define BT_SDP_DATA_ELEMENT_TYPE_UUID 0x18  /**< The element type for the UUID. The profile with this element type must use the size type of 
                                                 #BT_SDP_DATA_ELEMENT_SIZE_2BYTE, #BT_SDP_DATA_ELEMENT_SIZE_4BYTE or #BT_SDP_DATA_ELEMENT_SIZE_16BYTE. */

#define BT_SDP_DATA_ELEMENT_TYPE_TEXT 0x20  /**< The element type for text string. The profile with this element type must use a size type of 
                                                 #BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_8BIT, #BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_16BIT or #BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_32BIT. */

#define BT_SDP_DATA_ELEMENT_TYPE_BOOL 0x28  /**< The element type for Boolean value. The profile with this element type must use a size type of #BT_SDP_DATA_ELEMENT_SIZE_1BYTE. */

#define BT_SDP_DATA_ELEMENT_TYPE_SEQ  0x30  /**< The element type for data element sequence. 
                                                 The profile with this element type must use a size type of #BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_8BIT, #BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_16BIT or #BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_32BIT. */

#define BT_SDP_DATA_ELEMENT_TYPE_ALT  0x38  /**< The element type for data element alternative. The data contains a sequence of data elements. 
                                                 This type is used when there is more than one sequence of #BT_SDP_DATA_ELEMENT_TYPE_SEQ.
                                                 The profile with this element type must use a size type of #BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_8BIT, #BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_16BIT or #BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_32BIT. */

#define BT_SDP_DATA_ELEMENT_TYPE_URL  0x40  /**< The element type for the URL. The profile with this element type must use a size type of
                                                 #BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_8BIT, #BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_16BIT or #BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_32BIT. */

#define BT_SDP_DATA_ELEMENT_TYPE_MASK 0xf8  /**< To get the element type, bitwise "and" this element type mask with the first byte of the data element. */


typedef uint8_t bt_sdp_data_element_size_t; /**< Define the type of data element size. */

#define BT_SDP_DATA_ELEMENT_SIZE_1BYTE      0x00    /**< A 1-byte element. It actually means size 0, however, if the element type is #BT_SDP_DATA_ELEMENT_TYPE_NIL. */

#define BT_SDP_DATA_ELEMENT_SIZE_2BYTE     0x01    /**< A 2-byte element. */
#define BT_SDP_DATA_ELEMENT_SIZE_4BYTE     0x02    /**< A 4-byte element. */
#define BT_SDP_DATA_ELEMENT_SIZE_8BYTE     0x03    /**< An 8-byte element. */
#define BT_SDP_DATA_ELEMENT_SIZE_16BYTE    0x04    /**< A 16-byte element. */
#define BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_8BIT  0x05    /**< The actual size of the data element, in bytes, is in the next 8 bits. */

#define BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_16BIT 0x06    /**< The actual size of the data element, in bytes, is in the next 16 bits. */

#define BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_32BIT 0x07    /**< The actual size of the data element, in bytes, is in the next 32 bits. */

#define BT_SDP_DATA_ELEMENT_SIZE_MASK       0x07    /**< To get the data size, bitwise "and" this element size mask with the first byte of the data element. */

/**
 * @brief Define the attribute ID.
 *        Attribute is identified by an attribute ID. This type includes
 *        most (but not all) of the possible attributes available on
 *        a Bluetooth device.
 *
 *        Upper layer services may use these attributes or define their own. In this case, the service must define
 *        attributes with IDs between 0x0200 and 0xFFFF.
 */
typedef uint16_t bt_sdp_attribute_id_t;   /**< Define the type of attribute ID. */


#define BT_SDP_ATTRIBUTE_ID_INVALID                             0xFFFF  /**< Invalid attribute ID. */

/**
 * @brief The following attributes are required to be present in all
 *        service records.
 */

/**
 * @brief A data element sequence of UUIDs. Each UUID represents a service
 *        class supported by the service record. At least one UUID must
 *        be present. The UUIDs should be listed in order from the most specific
 *        class to the most general class unless specified by the profile
 *        specifications defining the service classes.
 *
 *        The #bt_sdp_service_class_id_t type represents these UUIDs.
 */
#define BT_SDP_ATTRIBUTE_ID_SERVICE_CLASS_ID_LIST               0x0001

/**
 * @brief The following attributes are universal to all service records,
 *        meaning that the same attribute IDs are always used. However, attributes
 *        may or may not be present within a service record.
 *
 *        For more details about these attributes, please refer to the <a href="https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439&_ga=1.241778370.1195131350.1437993589">Bluetooth core specification version 4.2 [VOL 3, part B]</a>.
 */

#define BT_SDP_ATTRIBUTE_ID_SERVICE_RECORD_STATE                0x0002  /**< Facilitates caching of the service attributes. */
#define BT_SDP_ATTRIBUTE_ID_SERVICE_ID                          0x0003  /**< Identifies the service instance by the service record. */
#define BT_SDP_ATTRIBUTE_ID_PROTOCOL_DESC_LIST                  0x0004  /**< Describes one or more protocol stacks. */
#define BT_SDP_ATTRIBUTE_ID_BROWSE_GROUP_LIST                   0x0005  /**< Represents a browse group to which the service record belongs to. */
#define BT_SDP_ATTRIBUTE_ID_LANG_BASE_ID_LIST                   0x0006  /**< Supports human-readable attributes for multiple natural languages in a single service record. 
                                                                             It's a list in which each member contains a language identifier, a character encoding identifier and a base attribute ID.*/
#define BT_SDP_ATTRIBUTE_ID_SERVICE_INFO_TIME_TO_LIVE           0x0007  /**< The information in a service record, expected to remain valid and unchanged. */
#define BT_SDP_ATTRIBUTE_ID_SERVICE_AVAILABILITY                0x0008  /**< Represents if the service is available to accept additional clients. */
#define BT_SDP_ATTRIBUTE_ID_BT_PROFILE_DESC_LIST                0x0009  /**< The descriptions of the Bluetooth profiles. */
#define BT_SDP_ATTRIBUTE_ID_DOC_URL                             0x000a  /**< The documentation path of a service record. */
#define BT_SDP_ATTRIBUTE_ID_CLIENT_EXEC_URL                     0x000b  /**< The execution location of the client. */
#define BT_SDP_ATTRIBUTE_ID_ICON_URL                            0x000c  /**< The icon location of the service. */
#define BT_SDP_ATTRIBUTE_ID_ADDITIONAL_PROT_DESC_LISTS          0x000d  /**< Same as #BT_SDP_ATTRIBUTE_ID_PROTOCOL_DESC_LIST, but for those services that require more channels which can't be described in the #BT_SDP_ATTRIBUTE_ID_PROTOCOL_DESC_LIST. */


/**
 * @brief The following attribute IDs must be added with the attribute ID base included in the #BT_SDP_ATTRIBUTE_ID_LANG_BASE_ID_LIST attribute,  
 *        usually it is 0x0100. Please refer to the example code.
 */
#define BT_SDP_ATTRIBUTE_ID_SERVICE_NAME                        0x0000  /**< The service name. */
#define BT_SDP_ATTRIBUTE_ID_SERVICE_DESCRIPTION                 0x0001  /**< The brief description of the service. */
#define BT_SDP_ATTRIBUTE_ID_PROVIDER_NAME                       0x0002  /**< The name of the service provider. */

/**
 * @brief The following attribute applies only to a service record that
 *        corresponds to a browse group descriptor service.
 */

/**
 * @brief A UUID to locate services that are part of the browse group.
 */
#define BT_SDP_ATTRIBUTE_ID_GROUP_ID                            0x0200

/**
 * @brief The following attributes apply only to the service record that
 *        corresponds to the service discovery server itself. Therefore, they
 *        are valid only when the #BT_SDP_ATTRIBUTE_ID_SERVICE_CLASS_ID_LIST contains
 *        the #BT_SDP_SERVICE_CLASS_SERVICE_DISCOVERY_SERVER UUID.
 */
#define BT_SDP_ATTRIBUTE_ID_VERSION_NUMBER_LIST                 0x0200  /**< A list of the version numbers supported by the SDP server. */
#define BT_SDP_ATTRIBUTE_ID_SERVICE_DATABASE_STATE              0x0201  /**< An Integer that facilitates caching of the service records. */

/**
 * @brief The following attributes are used by the specific profiles as
 *        defined in the profile specifications.
 */
#define BT_SDP_ATTRIBUTE_ID_SUPPORTED_FEATURES                  0x0311  /**< Describes the supported features. */

/**
 * @brief For phonebook access profile.
 */
#define BT_SDP_ATTRIBUTE_ID_SUPPORTED_REPOSITORIES              0x0314

/**
 * @brief The UUIDs associated with a specific service and profile.
 *        Any number of these UUIDs may be present in the
 *        #BT_SDP_ATTRIBUTE_ID_SERVICE_CLASS_ID_LIST attribute of a service record and may
 *        appear in the #BT_SDP_ATTRIBUTE_ID_BT_PROFILE_DESC_LIST ID. 
 */
typedef uint16_t bt_sdp_service_class_id_t; /**< Defines the type of service class ID. */

/**
 * @brief For SDP service class.
 */
#define BT_SDP_SERVICE_CLASS_SERVICE_DISCOVERY_SERVER             0x1000

/**
 * @brief For browse group descriptor service.
 */
#define BT_SDP_SERVICE_CLASS_BROWSE_GROUP_DESC                    0x1001

/**
 * @brief For public browse group service.
 */
#define BT_SDP_SERVICE_CLASS_PUBLIC_BROWSE_GROUP                  0x1002

/**
 * @brief For serial port service and profile.
 */
#define BT_SDP_SERVICE_CLASS_SERIAL_PORT                          0x1101

/**
 * @brief For OBEX object push service and object push profile.
 */
#define BT_SDP_SERVICE_CLASS_OBEX_OBJECT_PUSH                     0x1105

/**
 * @brief For OBEX file transfer service and file transfer profile.
 */
#define BT_SDP_SERVICE_CLASS_OBEX_FILE_TRANSFER                   0x1106


/**
 * @brief For headset service and profile.
 */
#define BT_SDP_SERVICE_CLASS_HEADSET                              0x1108

/**
 * @brief For audio source.
 */
#define BT_SDP_SERVICE_CLASS_AUDIO_SOURCE                         0x110A

/**
 * @brief For audio sink.
 */
#define BT_SDP_SERVICE_CLASS_AUDIO_SINK                           0x110B

/**
 * @brief For audio/video remote control target.
 */
#define BT_SDP_SERVICE_CLASS_AV_REMOTE_CONTROL_TARGET             0x110C

/**
 * @brief For advanced audio distribution profile.
 */
#define BT_SDP_SERVICE_CLASS_AUDIO_DISTRIBUTION                   0x110D

/**
 * @brief For audio/video remote control.
 */
#define BT_SDP_SERVICE_CLASS_AV_REMOTE_CONTROL                    0x110E

/**
 * @brief For audio/video remote control.
 */
#define BT_SDP_SERVICE_CLASS_AV_REMOTE_CONTROL_CONTROL            0x110F

/**
 * @brief For headset audio gateway.
 */
#define BT_SDP_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY                0x1112

/**
 * @brief For handsfree profile.
 */
#define BT_SDP_SERVICE_CLASS_HANDSFREE                            0x111E

/**
 * @brief For handsfree audio gateway.
 */
#define BT_SDP_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY              0x111F

/**
 * @brief For human interface device profile.
 */
#define BT_SDP_SERVICE_CLASS_HUMAN_INTERFACE_DEVICE               0x1124


/**
 * @brief For SIM access profile.
 */
#define BT_SDP_SERVICE_CLASS_SIM_ACCESS                           0x112D

/**
 * @brief For phonebook access client.
 */
#define BT_SDP_SERVICE_CLASS_PBAP_CLIENT                          0x112E

/**
 * @brief For phonebook access server.
 */
#define BT_SDP_SERVICE_CLASS_PBAP_SERVER                          0x112F

/**
 * @brief For phonebook access.
 */
#define BT_SDP_SERVICE_CLASS_PBAP                                 0x1130

/**
 * @brief For headset access.
 */
#define BT_SDP_SERVICE_CLASS_HEADSET_HS                           0x1131

/**
 * @brief For generic audio service.
 */
#define BT_SDP_SERVICE_CLASS_GENERIC_AUDIO                        0x1203


/**
 * @brief The UUIDs associated with protocols.
 *        Any number of these UUIDs may be present in the
 *        #BT_SDP_ATTRIBUTE_ID_SERVICE_CLASS_ID_LIST attribute of a service record and may
 *        appear in the #BT_SDP_ATTRIBUTE_ID_BT_PROFILE_DESC_LIST ID.
 */
typedef uint16_t bt_sdp_protocol_uuid_t; /**< Defines the type of protocol ID.*/

#define BT_SDP_INVALID_UUID                     0xFFFF /**< Invalid UUID. */

#define BT_SDP_PROTOCOL_SDP                     0x0001 /**< Service discovery protocol. */

#define BT_SDP_PROTOCOL_RFCOMM                  0x0003 /**< RFCOMM protocol. */

#define BT_SDP_PROTOCOL_ATT                     0x0007 /**< ATT protocol. */

#define BT_SDP_PROTOCOL_OBEX                    0x0008 /**< OBEX protocol. */

#define BT_SDP_PROTOCOL_AVCTP                   0x0017 /**< Audio/video control transport protocol. */

#define BT_SDP_PROTOCOL_AVDTP                   0x0019 /**< Audio/video distribution transport protocol. */

#define BT_SDP_PROTOCOL_L2CAP                   0x0100 /**< L2CAP protocol. */





/**
 * @brief To format a #bt_sdps_attribute_t structure using the supplied attribute ID
 *        and attribute value. This macro is used to format an attribute into
 *        #bt_sdps_attribute_t structure for a SDP record.
 *
 *        Parameters:
 *          Attribute_id is the attribute ID, please refer to the #bt_sdp_attribute_id_t.
 *          Attribute is an array with the attribute value.
 */
#define BT_SDP_ATTRIBUTE(attribute_id, attribute)                     \
    { attribute_id,           /* Attribute ID. */          \
      sizeof(attribute),      /* Attribute size. */        \
      attribute }             /* Attribute value. */

/**
 * @brief To form a data element sequence header using the supplied 8-bit
 *        size value. The data element sequence header is used in the SDP record attributes
 *        and the SDP queries. Notice that this macro only forms the header portion
 *        of the data element sequence. The actual data elements within this
 *        sequence will need to be formed by using other SDP macros.
 *
 *        Parameters:
 *          size - 8-bit size of the data element sequence.
 */
#define BT_SDP_ATTRIBUTE_HEADER_8BIT(size)                                     \
    BT_SDP_DATA_ELEMENT_TYPE_SEQ + BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_8BIT,      /* Type and size index is 0x35. */ \
             size                               /* 8-bit size. */

/**
 * @brief To form a data element sequence header using the supplied 16-bit
 *        size value. The data element sequence header is used in the SDP record attributes
 *        and the SDP queries. Notice that this macro only forms the header portion
 *        of the data element sequence. The actual data elements within this
 *        sequence will need to be formed by using other SDP macros.
 *
 *        Parameters:
 *          size - 16-bit size of the data element sequence.
 */
#define BT_SDP_ATTRIBUTE_HEADER_16BIT(size)                                     \
    BT_SDP_DATA_ELEMENT_TYPE_SEQ + BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_16BIT,      /* Type and size index is 0x36. */ \
             (uint8_t)(((size) & 0xff00) >> 8),  /* Bits[15:8] of size. */     \
             (uint8_t)((size) & 0x00ff)          /* Bits[7:0] of size. */

/**
 * @brief To form a data element sequence header using the supplied 32-bit
 *        size value. The data element sequence header is used in the SDP record attributes
 *        and the SDP queries. Notice that this macro only forms the header portion
 *        of the data element sequence. The actual data elements within this
 *        sequence will need to be formed by using other SDP macros.
 *
 *        Parameters:
 *          size - 32-bit size of the data element sequence.
 */
#define BT_SDP_ATTRIBUTE_HEADER_32BIT(size)                                        \
    BT_SDP_DATA_ELEMENT_TYPE_SEQ + BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_32BIT,           /* Type and size index is 0x37. */ \
             (uint8_t)(((size) & 0xff000000) >> 24),  /* Bits[32:24] of size. */    \
             (uint8_t)(((size) & 0x00ff0000) >> 16),  /* Bits[23:16] of size. */    \
             (uint8_t)(((size) & 0x0000ff00) >> 8),   /* Bits[15:8] of size. */     \
             (uint8_t)((size) & 0x000000ff)           /* Bits[7:0] of size. */

/**
 * @brief To form a UUID data element from the supplied 16-bit UUID value.
 *        The UUID data element is used in the SDP record attributes and the SDP queries.
 *
 *        Parameters:
 *          uuid - 16-bit UUID value, please refer to the #bt_sdp_service_class_id_t and #bt_sdp_protocol_uuid_t.
 */
#define BT_SDP_UUID_16BIT(uuid)                                              \
    BT_SDP_DATA_ELEMENT_TYPE_UUID + BT_SDP_DATA_ELEMENT_SIZE_2BYTE,            /* Type and size index is 0x19 */ \
              (uint8_t)(((uuid) & 0xff00) >> 8),    /* Bits[15:8] of UUID */     \
              (uint8_t)((uuid) & 0x00ff)            /* Bits[7:0] of UUID */

/**
 * @brief To form a UUID data element from the supplied 32-bit UUID value.
 *        The UUID data element is used in the SDP record attributes and the SDP queries.
 *
 *        Parameters:
 *          uuid - 32-bit UUID value, please refer to the #bt_sdp_service_class_id_t and #bt_sdp_protocol_uuid_t.
 *          16-bit UUID values can be converted to 32-bit by zero extending the 16-bit value.
 */
#define BT_SDP_UUID_32BIT(uuid)                                                 \
    BT_SDP_DATA_ELEMENT_TYPE_UUID + BT_SDP_DATA_ELEMENT_SIZE_4BYTE,               /* Type and size index is 0x1A */ \
              (uint8_t)(((uuid) & 0xff000000) >> 24),  /* Bits[32:24] of UUID */    \
              (uint8_t)(((uuid) & 0x00ff0000) >> 16),  /* Bits[23:16] of UUID */    \
              (uint8_t)(((uuid) & 0x0000ff00) >> 8),   /* Bits[15:8] of UUID */     \
              (uint8_t)((uuid) & 0x000000ff)           /* Bits[7:0] of UUID */

/**
 * @brief To form a UUID data element from the supplied 128-bit UUID value.
 *        The UUID data element is used in the SDP record attributes and the SDP queries.
 *
 *        Parameters:
 *          uuid - 128-bit UUID value, please refer to the #bt_sdp_service_class_id_t and #bt_sdp_protocol_uuid_t.
 *          16-bit UUID values can be converted to 128-bit using
 *          the following conversion: 128_bit_value = 16_bit_value * 2^96 +
 *          Bluetooth base UUID.
 */
#define BT_SDP_UUID_128BIT(uuid)                /* UUID must be a 16-byte array */ \
    BT_SDP_DATA_ELEMENT_TYPE_UUID + BT_SDP_DATA_ELEMENT_SIZE_16BYTE,       /* Type and size index is 0x1C. */       \
              uuid                              /* 128-bit UUID */

/**
 * @brief To form an unsigned integer data element from the supplied 8-bit unsigned integer value.
 *        The unsigned integer data element is used in SDP record attributes and the SDP queries.
 *
 *        Parameters:
 *          uint is an 8-bit unsigned integer value.
 */
#define BT_SDP_UINT_8BIT(uint)                                               \
    BT_SDP_DATA_ELEMENT_TYPE_UINT + BT_SDP_DATA_ELEMENT_SIZE_1BYTE,          /* Type and size index is 0x08. */ \
              (uint8_t)(uint)                    /* 8-bit unsigned integer */

/**
 * @brief To form an unsigned integer data element from the supplied 16-bit unsigned integer value.
 *        The unsigned integer data element is used in SDP record attributes and the SDP queries.
 *
 *        Parameters:
 *          The uint is a 16-bit unsigned integer value.
 */
#define BT_SDP_UINT_16BIT(uint)                                              \
    BT_SDP_DATA_ELEMENT_TYPE_UINT + BT_SDP_DATA_ELEMENT_SIZE_2BYTE,         /* Type and size index is 0x09. */ \
              (uint8_t)(((uint) & 0xff00) >> 8), /* Bits[15:8] of unsigned integer */     \
              (uint8_t)((uint) & 0x00ff)         /* Bits[7:0] of unsigned integer */

/**
 * @brief To form an unsigned integer data element from the supplied 32-bit unsigned integer value.
 *        The unsigned integer data element is used in SDP record attributes and the SDP queries.
 *
 *        Parameters:
 *          The uint is a 32-bit unsigned integer value.
 */
#define BT_SDP_UINT_32BIT(uint)                                                 \
    BT_SDP_DATA_ELEMENT_TYPE_UINT + BT_SDP_DATA_ELEMENT_SIZE_4BYTE,               /* Type and size index is 0x0A. */ \
              (uint8_t)(((uint) & 0xff000000) >> 24),  /* Bits[31:24] of unsigned integer */    \
              (uint8_t)(((uint) & 0x00ff0000) >> 16),  /* Bits[23:16] of unsigned integer */    \
              (uint8_t)(((uint) & 0x0000ff00) >> 8),   /* Bits[15:8] of unsigned integer */     \
              (uint8_t)((uint) & 0x000000ff)           /* Bits[7:0] of unsigned integer */

/**
 * @brief To form an unsigned integer data element from the supplied 64-bit unsigned integer value.
 *        The unsigned integer data element is used in SDP record attributes and the SDP queries.
 *
 *        Parameters:
 *          The uint is a 64-bit unsigned integer value.
 */
#define BT_SDP_UINT_64BIT(uint)                     /* unsigned integer must be an 8-byte array */ \
    BT_SDP_DATA_ELEMENT_TYPE_UINT + BT_SDP_DATA_ELEMENT_SIZE_8BYTE,            /* Type and size index is 0x0B. */       \
              uint                                  /* 64-bit unsigned integer */

/**
 * @brief To form an unsigned integer data element from the supplied 128-bit unsigned integer value.
 *        The unsigned integer data element is used in the SDP record attributes and the SDP queries.
 *
 *        Parameters:
 *          The uint is a 128-bit unsigned integer value.
 */
#define BT_SDP_UINT_128BIT(uint)                  /* unsigned integer must be a 16-byte array */ \
    BT_SDP_DATA_ELEMENT_TYPE_UINT + BT_SDP_DATA_ELEMENT_SIZE_16BYTE,         /* Type and size index is 0x0C. */       \
              uint                                /* 128-bit unsigned integer */

/**
 * @brief To form a text data element header from the supplied 8-bit size
 *        value. The text data element is used in the SDP record attributes and the SDP queries.
 *        Notice that this macro only forms the header portion of the text data
 *        element. The actual text data within this data element will need to
 *        be provided separately.
 *
 *        Parameters:
 *          The size is a 8-bit size value.
 */
#define BT_SDP_TEXT_8BIT(size)                                               \
    BT_SDP_DATA_ELEMENT_TYPE_TEXT + BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_8BIT,      /* Type and size index is 0x25. */ \
              (uint8_t)(size)                    /* 8-bit size */

/**
 * @brief To form a text data element header from the supplied 16-bit size
 *        value. The text data element is used in the SDP record attributes and the SDP queries.
 *        Notice that this macro only forms the header portion of the text data
 *        element. The actual text data within this data element will need to
 *        be provided separately.
 *
 *        Parameters:
 *          The size is 16-bit size value.
 */
#define BT_SDP_TEXT_16BIT(size)                                               \
    BT_SDP_DATA_ELEMENT_TYPE_TEXT + BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_16BIT,      /* Type and size index is 0x26. */ \
              (uint8_t)(((size) & 0xff00) >> 8),  /* Bits[15:8] of size */     \
              (uint8_t)((size) & 0x00ff)          /* Bits[7:0] of size */

/**
 * @brief To form a text data element header from the supplied 32-bit size
 *        value. The text data element is used in the SDP record attributes and the SDP queries.
 *        Notice that this macro only forms the header portion of the text data
 *        element. The actual text data within this data element will need to
 *        be provided separately.
 *
 *        Parameters:
 *          The size is 32-bit size value.
 */
#define BT_SDP_TEXT_32BIT(size)                                                 \
    BT_SDP_DATA_ELEMENT_TYPE_TEXT + BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_32BIT,           /* Type and size index is 0x27. */ \
              (uint8_t)(((size) & 0xff000000) >> 24),  /* Bits[32:24] of size */    \
              (uint8_t)(((size) & 0x00ff0000) >> 16),  /* Bits[23:16] of size */    \
              (uint8_t)(((size) & 0x0000ff00) >> 8),   /* Bits[15:8] of size */     \
              (uint8_t)((size) & 0x000000ff)           /* Bits[7:0] of size */

/**
 * @brief To form a URL data element header from the supplied 8-bit size
 *        value. The URL data element is used in the SDP record attributes and the SDP queries.
 *        Notice that this macro only forms the header portion of the URL data
 *        element. The actual URL data within this data element will need to
 *        be provided separately.
 *
 *        Parameters:
 *          The size is 8-bit size value.
 */
#define BT_SDP_URL_8BIT(size)     \
    BT_SDP_DATA_ELEMENT_TYPE_URL + BT_SDP_DATA_ELEMENT_SIZE_ADDITIONAL_8BIT, \
    (uint8_t)(size)

/**
 * @brief To form a Boolean data element from the supplied 8-bit Boolean value.
 *        The Boolean data element is used in the SDP record attributes and the SDP queries.
 *
 *        Parameters:
 *          The value is 8-bit Boolean value.
 */
#define BT_SDP_BOOL(value)                                                   \
    BT_SDP_DATA_ELEMENT_TYPE_BOOL + BT_SDP_DATA_ELEMENT_SIZE_1BYTE,   /* Type and size index is 0x28. */ \
              (uint8_t)(value)            /* Boolean value */

/**
 * @}
 */

/**
 * @defgroup Bluetoothhbif_sdp_struct Struct
 * @{
 * This section defines the structures for the SDP server.
 */

/**
 * @brief Defines the attribute of the SDP record.
 */
typedef struct {
    uint16_t attribute_id;  /**< The attribute ID refer to #bt_sdp_attribute_id_t. */
    uint16_t length;        /**< The length of the attribute value. */
    const uint8_t *value;   /**< A pointer to the attribute value. */
} bt_sdps_attribute_t;

/**
 * @brief Defines the attribute list of the SDP record.
 */
typedef struct {
    uint16_t attribute_list_length;           /**< The length of the attribute list. */
    const bt_sdps_attribute_t *attribute_list;  /**< A pointer to the attribute list. */
} bt_sdps_record_t;

/**
 * @}
 */

/**
 * @brief     This is a user defined callback and called by the SDP. It might be implemented by the application to provide the customized record and record number.
 * @param[in] record_list   is a pointer to the record list. If there is no customized record, the record_list should be NULL and return 0.
 * @return    The record number.
 */
uint8_t bt_sdps_get_customized_record(const bt_sdps_record_t *** record_list);

BT_EXTERN_C_END

/**
 * @}
 * @}
 * @}
 */


#endif /* __BT_SDP_H__ */

