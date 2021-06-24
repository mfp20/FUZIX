#include "config.h"
#include "picosdk.h"
#include <pico/unique_id.h>
#include <tusb.h>


#define EPNUM0  0x80 // usb control endpoint, not usable for interfaces
#define EPNUM1  0x81
#define EPNUM2  0x82
#define EPNUM3  0x83
#define EPNUM4  0x84
#define EPNUM5  0x85
#define EPNUM6  0x86
#define EPNUM7  0x87
#define EPNUM8  0x88
#define EPNUM9  0x89
#define EPNUM10 0x8a
#define EPNUM11 0x8b
#define EPNUM12 0x8c
#define EPNUM13 0x0d
#define EPNUM14 0x8e
#define EPNUM15 0x8f

#define USBD_VID                (0x2E8A) // Raspberry Pi
// Temporary setup until I figure out how to dynamically reconfigure USB at runtime
// PID depends on the 32 endpoints usage, packed in couples (I/O), 16 bits:
// (MSb) 0--- ---- ---- ---1 (LSb)
//       |||| |||| |||| |||`-----  usb config (always 1 by usb design)
//       |||| |||| |||| ||`------  cdc tty cmd
//       |||| |||| |||| |`-------  cdc tty data
//       |||| |||| ||||  `-------  cdc log cmd
//       |||| |||| |||`----------  cdc log data
//       |||| |||| ||`-----------  vendor external filesystem out
//       |||| |||| |`------------  vendor external filesystem in
//       |||| |||| `-------------  cdc-spare0 cmd
//       |||| |||`---------------  cdc-spare0 data
//       |||| ||`----------------  vendor-spare0 out
//       |||| |`-----------------  vendor-spare0 in
//       |||| `------------------  cdc-spare1 cmd
//       |||`--------------------  cdc-spare1 data
//       ||`---------------------  vendor-spare1 out
//       |`----------------------  vendor-spare1 in
//       `-----------------------  unused
// ie: if all drivers are enabled, the resulting PID is 0x7FFF
#define PID_MAP(dev, nbit)      ( (USB_DEV_##dev) << (nbit) )
#define USBD_PID                ( 1                  + \
                                  PID_MAP(CONSOLE,  1)  + \
                                  PID_MAP(CONSOLE,  2)  + \
                                  PID_MAP(LOG,      3)  + \
                                  PID_MAP(LOG,      4)  + \
                                  PID_MAP(EXTFS,    5)  + \
                                  PID_MAP(EXTFS,    6)  + \
                                  PID_MAP(TTY1,     7)  + \
                                  PID_MAP(TTY1,     8)  + \
                                  PID_MAP(RAW1,     9)  + \
                                  PID_MAP(RAW1,     10) + \
                                  PID_MAP(TTY2,     11) + \
                                  PID_MAP(TTY2,     12) + \
                                  PID_MAP(RAW2,     13) + \
                                  PID_MAP(RAW2,     14) + \
                                  0 \
                                )

#define USBD_STR_LANG   (0x00)
#define USBD_STR_MANUF  (0x01)
#define USBD_STR_PRODUCT (0x02)
#define USBD_STR_SERIAL (0x03)
#define USBD_STR_CONSOLE (0x04)
#define USBD_STR_LOG    (0x05)
#define USBD_STR_EXTFS  (0x06)
#define USBD_STR_CDC    (0x07)
#define USBD_STR_VENDOR (0x08)

#define USBD_MAX_POWER_MA (100)
#define USBD_CDC_CMD_MAX_SIZE (64)
#define USBD_CDC_IN_OUT_MAX_SIZE (64)


//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+

// Note: descriptors returned from callbacks must exist long enough for transfer to complete
tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200, // USB Specification version 2.0 (microframes each 0.125ms)
    .bDeviceClass       = 0x00,   // Each interface specifies its own
    .bDeviceSubClass    = 0x00,   // Each interface specifies its own
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = USBD_VID,
    .idProduct          = USBD_PID,
    .bcdDevice          = 0x0001, // Version 00.01
    .iManufacturer      = USBD_STR_MANUF,
    .iProduct           = USBD_STR_PRODUCT,
    .iSerialNumber      = USBD_STR_SERIAL,
    .bNumConfigurations = 0x01
};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
const uint8_t *tud_descriptor_device_cb(void) {
    return (const uint8_t *)&desc_device;
}


//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// board unique ID from devices.c
extern pico_unique_board_id_t pico_id;
// C string for iSerialNumber in USB Device Descriptor, two chars per byte + terminating NULL
char usb_serial[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];
// convert pico_id into a string to be used as descriptor
void tusb_id2str(void) {
    for (int i = 0; i < PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2; i++) {
        /* Byte index inside the uid array */
        int bi = i / 2;
        /* Use high nibble first to keep memory order (just cosmetics) */
        uint8_t nibble = (pico_id.id[bi] >> 4) & 0x0F;
        pico_id.id[bi] <<= 4;
        /* Binary to hex digit */
        usb_serial[i] = nibble < 10 ? nibble + '0' : nibble + 'A' - 10;
    }
}

static const char *const string_desc_arr[] = {
    [USBD_STR_LANG]     = (const char[]) { 0x09, 0x04 }, // 0: is supported language is English (0x0409)
    [USBD_STR_MANUF]    = "Raspberry Pi",
    [USBD_STR_PRODUCT]  = "Pico Fuzix",
    [USBD_STR_SERIAL]   = usb_serial,
    [USBD_STR_CONSOLE]  = "Fuzix console",
    [USBD_STR_LOG]      = "Fuzix log",
    [USBD_STR_EXTFS]    = "Fuzix extfs",
    [USBD_STR_CDC]      = "Tinyusb CDC",
    [USBD_STR_VENDOR]   = "Tinyusb Vendor"
};

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    static uint16_t _desc_str[32];
    (void) langid;

    uint8_t chr_count;

    if ( index == 0) {
        //desc_str[1] = 0x0409; // supported language is English
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    } else {
        // Convert ASCII string into UTF-16
        if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) ) return NULL;

        const char* str = string_desc_arr[index];

        // Cap at max char
        chr_count = strlen(str);
        if ( chr_count > 31 ) chr_count = 31;

        for(uint8_t i=0; i<chr_count; i++) {
            _desc_str[1+i] = str[i];
        }
    }

    // first byte is length (including header), second byte is string type
    _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);

    return _desc_str;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

enum {
#if USB_DEV_CONSOLE
  ITFNUM0, ITFNUM0_DATA,
#endif
#if USB_DEV_LOG
  ITFNUM1, ITFNUM1_DATA,
#endif
#if USB_DEV_EXTFS
  ITFNUM2, ITFNUM2_DATA,
#endif
#if USB_DEV_TTY1
  ITFNUM3, ITFNUM3_DATA,
#endif
#if USB_DEV_RAW1
  ITFNUM4, ITFNUM4_DATA,
#endif
#if USB_DEV_TTY2
  ITFNUM5, ITFNUM5_DATA,
#endif
#if USB_DEV_RAW2
  ITFNUM6, ITFNUM6_DATA,
#endif
  ITFNUM_TOTAL
};

#define CONFIG_TOTAL_LEN (  (TUD_CONFIG_DESC_LEN)                   + \
                            (TUD_CDC_DESC_LEN     * CFG_TUD_CDC   ) + \
                            (TUD_VENDOR_DESC_LEN  * CFG_TUD_VENDOR) \
                         )

static const uint8_t desc_configuration[] = {
    // header
    TUD_CONFIG_DESCRIPTOR(1, ITFNUM_TOTAL, USBD_STR_LANG, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, USBD_MAX_POWER_MA),
#if USB_DEV_CONSOLE
    // Interface 0
    TUD_CDC_DESCRIPTOR(ITFNUM0, USBD_STR_CONSOLE, EPNUM1, USBD_CDC_CMD_MAX_SIZE, EPNUM2 & 0x7F, EPNUM2, USBD_CDC_IN_OUT_MAX_SIZE),
#endif
#if USB_DEV_LOG
    // Interface 1
    TUD_CDC_DESCRIPTOR(ITFNUM1, USBD_STR_LOG, EPNUM3, USBD_CDC_CMD_MAX_SIZE, EPNUM4 & 0x7F, EPNUM4, USBD_CDC_IN_OUT_MAX_SIZE),
#endif
#if USB_DEV_EXTFS
    // Interface 2
    TUD_VENDOR_DESCRIPTOR(ITFNUM2, USBD_STR_EXTFS, EPNUM5, EPNUM6, 64),
#endif
#if USB_DEV_TTY1
    // Interface 3
    TUD_CDC_DESCRIPTOR(ITFNUM3, USBD_STR_CDC, EPNUM7, USBD_CDC_CMD_MAX_SIZE, EPNUM8 & 0x7F, EPNUM8, USBD_CDC_IN_OUT_MAX_SIZE),
#endif
#if USB_DEV_RAW1
    // Interface 4
    TUD_VENDOR_DESCRIPTOR(ITFNUM4, USBD_STR_VENDOR, EPNUM9, EPNUM10, 64),
#endif
#if USB_DEV_TTY2
    // Interface 5
    TUD_CDC_DESCRIPTOR(ITFNUM5, USBD_STR_CDC, EPNUM11, USBD_CDC_CMD_MAX_SIZE, EPNUM12 & 0x7F, EPNUM12, USBD_CDC_IN_OUT_MAX_SIZE),
#endif
#if USB_DEV_RAW2
    // Interface 6
    TUD_VENDOR_DESCRIPTOR(ITFNUM6, USBD_STR_VENDOR, EPNUM13, EPNUM14, 64),
#endif
};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
  (void) index; // for multiple configurations
  return desc_configuration;
}


//--------------------------------------------------------------------+
// callbacks
//--------------------------------------------------------------------+

// Invoked when received new data
//void tud_cdc_rx_cb(uint8_t itf) {}

// Invoked when received `wanted_char`
//void tud_cdc_rx_wanted_cb(uint8_t itf, char wanted_char) {}

// Invoked when space becomes available in TX buffer
//void tud_cdc_tx_complete_cb(uint8_t itf) {}

// Invoked when line state DTR & RTS are changed via SET_CONTROL_LINE_STATE
//void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {}

// Invoked when line coding is change via SET_LINE_CODING
//void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const* line_coding) {}

// Invoked when received send break
//void tud_cdc_send_break_cb(uint8_t itf, uint16_t duration_ms) {}

