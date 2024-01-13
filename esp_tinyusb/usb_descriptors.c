/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "usb_descriptors.h"
#include "sdkconfig.h"
#include "tinyusb_types.h"

/*
 * A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
 * Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]         HID | MSC | CDC          [LSB]
 */
#define _PID_MAP(itf, n) ((CFG_TUD_##itf) << (n))
#define USB_TUSB_PID (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
    _PID_MAP(MIDI, 3) | _PID_MAP(AUDIO, 4) | _PID_MAP(NCM, 5) )

/**** Kconfig driven Descriptor ****/

//------------- Device Descriptor -------------//
const tusb_desc_device_t descriptor_dev_kconfig = {
    .bLength = sizeof(descriptor_dev_kconfig),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,

#if CFG_TUD_CDC
    // Use Interface Association Descriptor (IAD) for CDC
    // As required by USB Specs IAD's subclass must be common class (2) and protocol must be IAD (1)
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
#else
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
#endif

    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

#if CONFIG_TINYUSB_DESC_USE_ESPRESSIF_VID
    .idVendor = USB_ESPRESSIF_VID,
#else
    .idVendor = CONFIG_TINYUSB_DESC_CUSTOM_VID,
#endif

#if CONFIG_TINYUSB_DESC_USE_DEFAULT_PID
    .idProduct = USB_TUSB_PID,
#else
    .idProduct = CONFIG_TINYUSB_DESC_CUSTOM_PID,
#endif

    .bcdDevice = CONFIG_TINYUSB_DESC_BCD_DEVICE,

    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,

    .bNumConfigurations = 0x01
};

//------------- Array of String Descriptors -------------//
const char *descriptor_str_kconfig[] = {
    // array of pointer to string descriptors
    (char[]){0x09, 0x04},                // 0: is supported language is English (0x0409)
    CONFIG_TINYUSB_DESC_MANUFACTURER_STRING, // 1: Manufacturer
    CONFIG_TINYUSB_DESC_PRODUCT_STRING,      // 2: Product
    CONFIG_TINYUSB_DESC_SERIAL_STRING,       // 3: Serials, should use chip ID

#if CONFIG_TINYUSB_CDC_ENABLED
    CONFIG_TINYUSB_DESC_CDC_STRING,          // 4: CDC Interface
#endif

#if CONFIG_TINYUSB_MSC_ENABLED
    CONFIG_TINYUSB_DESC_MSC_STRING,          // 5: MSC Interface
#endif

#if CONFIG_TINYUSB_AUDIO_ENABLED             // 6: AUDIO Interface
    CONFIG_TINYUSB_DESC_AUDIO_STRING,
#endif

#if CONFIG_TINYUSB_NET_MODE_ECM_RNDIS || CONFIG_TINYUSB_NET_MODE_NCM
    "USB net",                               // 7. NET Interface
    "",                                      // 8. MAC
#endif
    NULL                                     // NULL: Must be last. Indicates end of array
};

//------------- Interfaces enumeration -------------//
enum {
#if CFG_TUD_CDC
    ITF_NUM_CDC = 0,
    ITF_NUM_CDC_DATA,
#endif

#if CFG_TUD_CDC > 1
    ITF_NUM_CDC1,
    ITF_NUM_CDC1_DATA,
#endif

#if CFG_TUD_MSC
    ITF_NUM_MSC,
#endif

#if CFG_TUD_AUDIO
    ITF_NUM_AUDIO_CONTROL,
    ITF_NUM_AUDIO_STREAMING,
#endif

#if CFG_TUD_NCM
    ITF_NUM_NET,
    ITF_NUM_NET_DATA,
#endif

    ITF_NUM_TOTAL
};

enum {
    TUSB_DESC_TOTAL_LEN = TUD_CONFIG_DESC_LEN +
                          CFG_TUD_CDC * TUD_CDC_DESC_LEN +
                          CFG_TUD_MSC * TUD_MSC_DESC_LEN +
#if CONFIG_TINYUSB_AUDIO_1_CH
        CFG_TUD_AUDIO * TUD_AUDIO_MIC_ONE_CH_DESC_LEN +
#elif CONFIG_TINYUSB_AUDIO_2_CH
        CFG_TUD_AUDIO * TUD_AUDIO_MIC_TWO_CH_DESC_LEN +
#else
        CFG_TUD_AUDIO * TUD_AUDIO_MIC_FOUR_CH_DESC_LEN +
#endif
                          CFG_TUD_NCM * TUD_CDC_NCM_DESC_LEN
};

//------------- USB Endpoint numbers -------------//
enum {
    // Available USB Endpoints: 5 IN/OUT EPs and 1 IN EP
    EP_EMPTY = 0,
#if CFG_TUD_CDC
    EPNUM_0_CDC_NOTIF,
    EPNUM_0_CDC,
#endif

#if CFG_TUD_CDC > 1
    EPNUM_1_CDC_NOTIF,
    EPNUM_1_CDC,
#endif

#if CFG_TUD_AUDIO
    EPNUM_AUDIO_DATA,
#endif

#if CFG_TUD_MSC
    EPNUM_MSC,
#endif

#if CFG_TUD_NCM
    EPNUM_NET_NOTIF,
    EPNUM_NET_DATA,
#endif
};

//------------- STRID -------------//
enum {
    STRID_LANGID = 0,
    STRID_MANUFACTURER,
    STRID_PRODUCT,
    STRID_SERIAL,

#if CFG_TUD_CDC
    STRID_CDC_INTERFACE,
#endif

#if CFG_TUD_MSC
    STRID_MSC_INTERFACE,
#endif

#if CFG_TUD_AUDIO
    STRID_AUDIO_INTERFACE,
#endif

#if CFG_TUD_NCM
    STRID_NET_INTERFACE,
    STRID_MAC,
#endif

};

#define TUD_AUDIO_MIC_TWO_CH_DESCRIPTOR(_itfnum, _stridx, _nBytesPerSample, _nBitsUsedPerSample, _epin, _epsize)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         \
    /* Standard Interface Association Descriptor (IAD) */                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
    TUD_AUDIO_DESC_IAD(/*_firstitfs*/ _itfnum, /*_nitfs*/ 0x02, /*_stridx*/ 0x00),                                                                                                                                                                                                                                                                                                                                                                                                                                          /* Standard AC Interface Descriptor(4.7.1) */                                \
        TUD_AUDIO_DESC_STD_AC(/*_itfnum*/ _itfnum, /*_nEPs*/ 0x00, /*_stridx*/ _stridx),                                                                                                                                                                                                                                                                                                                                                                                                                                    /* Class-Specific AC Interface Header Descriptor(4.7.2) */                   \
        TUD_AUDIO_DESC_CS_AC(/*_bcdADC*/ 0x0200, /*_category*/ AUDIO_FUNC_IO_BOX, /*_totallen*/ TUD_AUDIO_DESC_CLK_SRC_LEN + TUD_AUDIO_DESC_INPUT_TERM_LEN + TUD_AUDIO_DESC_OUTPUT_TERM_LEN + TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL_LEN, /*_ctrl*/ AUDIO_CS_AS_INTERFACE_CTRL_LATENCY_POS),                                                                                                                                                                                                                               /* Clock Source Descriptor(4.7.2.1) */                                       \
        TUD_AUDIO_DESC_CLK_SRC(/*_clkid*/ 0x04, /*_attr*/ AUDIO_CLOCK_SOURCE_ATT_INT_FIX_CLK, /*_ctrl*/ (AUDIO_CTRL_R << AUDIO_CLOCK_SOURCE_CTRL_CLK_FRQ_POS), /*_assocTerm*/ 0x01, /*_stridx*/ 0x00),                                                                                                                                                                                                                                                                                                                      /* Input Terminal Descriptor(4.7.2.4) */                                     \
        TUD_AUDIO_DESC_INPUT_TERM(/*_termid*/ 0x01, /*_termtype*/ AUDIO_TERM_TYPE_USB_STREAMING, /*_assocTerm*/ 0x03, /*_clkid*/ 0x04, /*_nchannelslogical*/ 0x02, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_idxchannelnames*/ 0x00, /*_ctrl*/ AUDIO_CTRL_R << AUDIO_IN_TERM_CTRL_CONNECTOR_POS, /*_stridx*/ 0x00),                                                                                                                                                                                           /* Output Terminal Descriptor(4.7.2.5) */                                    \
        TUD_AUDIO_DESC_OUTPUT_TERM(/*_termid*/ 0x03, /*_termtype*/ AUDIO_TERM_TYPE_USB_STREAMING, /*_assocTerm*/ 0x01, /*_srcid*/ 0x02, /*_clkid*/ 0x04, /*_ctrl*/ 0x0000, /*_stridx*/ 0x00),                                                                                                                                                                                                                                                                                                                               /* Feature Unit Descriptor(4.7.2.8) */                                       \
        TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL(/*_unitid*/ 0x02, /*_srcid*/ 0x01, /*_ctrlch0master*/ AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS, /*_ctrlch1*/ AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS, /*_ctrlch2*/ AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS, /*_stridx*/ 0x00), /* Standard AS Interface Descriptor(4.9.1) */ /* Interface 1, Alternate 0 - default alternate setting with 0 bandwidth */  \
        TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)((_itfnum) + 1), /*_altset*/ 0x00, /*_nEPs*/ 0x00, /*_stridx*/ 0x00), /* Standard AS Interface Descriptor(4.9.1) */                                                                                                                                                                                                                                                                                                                                                  /* Interface 1, Alternate 1 - alternate interface for data streaming */      \
        TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)((_itfnum) + 1), /*_altset*/ 0x01, /*_nEPs*/ 0x01, /*_stridx*/ 0x00),                                                                                                                                                                                                                                                                                                                                                                                                /* Class-Specific AS Interface Descriptor(4.9.2) */                          \
        TUD_AUDIO_DESC_CS_AS_INT(/*_termid*/ 0x03, /*_ctrl*/ AUDIO_CTRL_NONE, /*_formattype*/ AUDIO_FORMAT_TYPE_I, /*_formats*/ AUDIO_DATA_FORMAT_TYPE_I_PCM, /*_nchannelsphysical*/ 0x02, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_stridx*/ 0x00),                                                                                                                                                                                                                                                          /* Type I Format Type Descriptor(2.3.1.6 - Audio Formats) */                 \
        TUD_AUDIO_DESC_TYPE_I_FORMAT(_nBytesPerSample, _nBitsUsedPerSample),                                                                                                                                                                                                                                                                                                                                                                                                                                                /* Standard AS Isochronous Audio Data Endpoint Descriptor(4.10.1.1) */       \
        TUD_AUDIO_DESC_STD_AS_ISO_EP(/*_ep*/ _epin, /*_attr*/ (TUSB_XFER_ISOCHRONOUS | TUSB_ISO_EP_ATT_ASYNCHRONOUS | TUSB_ISO_EP_ATT_DATA), /*_maxEPsize*/ _epsize, /*_interval*/ TUD_OPT_HIGH_SPEED ? 0x04 : 0x01),                                                                                                                                                                                                                                                                                                       /* Class-Specific AS Isochronous Audio Data Endpoint Descriptor(4.10.1.2) */ \
        TUD_AUDIO_DESC_CS_AS_ISO_EP(/*_attr*/ AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK, /*_ctrl*/ AUDIO_CTRL_NONE, /*_lockdelayunit*/ AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_UNDEFINED, /*_lockdelay*/ 0x0000)

//------------- Configuration Descriptor -------------//
uint8_t const descriptor_cfg_kconfig[] = {
    // Configuration number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

#if CFG_TUD_CDC
    // Interface number, string index, EP notification address and size, EP data address (out, in) and size.
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, STRID_CDC_INTERFACE, 0x80 | EPNUM_0_CDC_NOTIF, 8, EPNUM_0_CDC, 0x80 | EPNUM_0_CDC, CFG_TUD_CDC_EP_BUFSIZE),
#endif

#if CFG_TUD_CDC > 1
    // Interface number, string index, EP notification address and size, EP data address (out, in) and size.
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC1, STRID_CDC_INTERFACE, 0x80 | EPNUM_1_CDC_NOTIF, 8, EPNUM_1_CDC, 0x80 | EPNUM_1_CDC, CFG_TUD_CDC_EP_BUFSIZE),
#endif

#if CFG_TUD_MSC
    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, STRID_MSC_INTERFACE, EPNUM_MSC, 0x80 | EPNUM_MSC, 64), // highspeed 512
#endif

#if CONFIG_TINYUSB_AUDIO_ENABLED
    #if CONFIG_TINYUSB_AUDIO_4_CH
        // Interface number, string index, nBytesPerSample, bitsUsedPerSample, EP In address, EP size
        TUD_AUDIO_MIC_FOUR_CH_DESCRIPTOR(ITF_NUM_AUDIO_CONTROL, STRID_AUDIO_INTERFACE, CFG_TUD_AUDIO_FUNC_1_N_BYTES_PER_SAMPLE_TX, CFG_TUD_AUDIO_FUNC_1_N_BYTES_PER_SAMPLE_TX * 8, (0x80 | EPNUM_AUDIO_DATA), CFG_TUD_AUDIO_EP_SZ_IN),
    #elif CONFIG_TINYUSB_AUDIO_2_CH
        // Interface number, string index, nBytesPerSample, bitsUsedPerSample, EP In address, EP size
        TUD_AUDIO_MIC_TWO_CH_DESCRIPTOR(ITF_NUM_AUDIO_CONTROL, STRID_AUDIO_INTERFACE, CFG_TUD_AUDIO_FUNC_1_N_BYTES_PER_SAMPLE_TX, CFG_TUD_AUDIO_FUNC_1_N_BYTES_PER_SAMPLE_TX * 8, (0x80 | EPNUM_AUDIO_DATA), CFG_TUD_AUDIO_EP_SZ_IN),
    #else
        // Interface number, string index, nBytesPerSample, bitsUsedPerSample, EP In address, EP size
        TUD_AUDIO_MIC_ONE_CH_DESCRIPTOR(ITF_NUM_AUDIO_CONTROL, STRID_AUDIO_INTERFACE, CFG_TUD_AUDIO_FUNC_1_N_BYTES_PER_SAMPLE_TX, CFG_TUD_AUDIO_FUNC_1_N_BYTES_PER_SAMPLE_TX * 8, (0x80 | EPNUM_AUDIO_DATA), CFG_TUD_AUDIO_EP_SZ_IN),
    #endif
#endif

#if CFG_TUD_NCM
    // Interface number, description string index, MAC address string index, EP notification address and size, EP data address (out, in), and size, max segment size.
    TUD_CDC_NCM_DESCRIPTOR(ITF_NUM_NET, STRID_NET_INTERFACE, STRID_MAC, (0x80 | EPNUM_NET_NOTIF), 64, EPNUM_NET_DATA, (0x80 | EPNUM_NET_DATA), CFG_TUD_NET_ENDPOINT_SIZE, CFG_TUD_NET_MTU),
#endif
};

#if CFG_TUD_NCM
uint8_t tusb_get_mac_string_id(void)
{
    return STRID_MAC;
}
#endif
/* End of Kconfig driven Descriptor */
