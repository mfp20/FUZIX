#include "log.h"

#include "tusb_config.h"
#include <tusb.h>

#include <pico.h>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <pico/unique_id.h>
#include <pico/stdio/driver.h>

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
#define EPNUM13 0x8d
#define EPNUM14 0x8e
#define EPNUM15 0x8f

#define USBD_VID                (0x2E8A) // Raspberry Pi
// Temporary setup until I figure out how to dynamically reconfigure USB at runtime
// PID depends on the 8 interfaces availability and we have 16 bits to flag:
// (MSb) 0--- ---- ---- ---- (LSb)
//       |||| |||| |||| |||`-----  cdc tty
//       |||| |||| |||| ||`------  cdc log
//       |||| |||| |||| |`-------  cdc user1
//       |||| |||| ||||  `-------  cdc user2
//       |||| |||| |||`----------  unused
//       |||| |||| ||`-----------  unused
//       |||| |||| |`------------  unused
//       |||| |||| `-------------  unused
//       |||| |||`---------------  vendor binary multiplexer
//       |||| ||`----------------  vendor user1
//       |||| |`-----------------  vendor user2
//       |||| `------------------  unused
//       |||`--------------------  unused
//       ||`---------------------  unused
//       |`----------------------  unused
//       `-----------------------  set to 0 to avoid PID 0xFFFF
#define PID_MAP(dev, nbit)      ( (USB_DEV_##dev) << (nbit) )
#define USBD_PID                ( \
                                  PID_MAP(CONSOLE,  1)  + \
                                  PID_MAP(LOG,      2)  + \
                                  PID_MAP(TTY1,     3)  + \
                                  PID_MAP(TTY2,     4) + \
                                  PID_MAP(MPLEX,    9)  + \
                                  PID_MAP(RAW1,     10)  + \
                                  PID_MAP(RAW2,     11) + \
                                  0 \
                                )

#define USBD_STR_LANG   (0x00)
#define USBD_STR_MANUF  (0x01)
#define USBD_STR_PRODUCT (0x02)
#define USBD_STR_SERIAL (0x03)
#define USBD_STR_CONSOLE (0x04)
#define USBD_STR_LOG    (0x05)
#define USBD_STR_CDC1    (0x06)
#define USBD_STR_CDC2    (0x07)
#define USBD_STR_MPLEX  (0x08)
#define USBD_STR_VENDOR1 (0x09)
#define USBD_STR_VENDOR2 (0x0a)


//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+

// Note: descriptors returned from callbacks must exist long enough for transfer to complete
tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200, // USB Specification version 2.0 (TODO check polling rate, 2.0 should be 125us)
    .bDeviceClass       = 0xEF,   // Multi-interface Function Code Device
    .bDeviceSubClass    = 0x02,   // Common Class Sub Class
    .bDeviceProtocol    = MISC_PROTOCOL_IAD, // Interface Association Descriptor protocol
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

// C string for iSerialNumber in USB Device Descriptor, two chars per byte + terminating NULL
char usb_serial[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];
// convert pico_id into a string to be used as descriptor
void tusb_id2str(void) {
  // Why a uint8_t[8] array inside a struct instead of an uint64_t an inquiring mind might wonder
  pico_unique_board_id_t pico_id;
  // get board id for later use in USB descriptor array
  pico_get_unique_board_id(&pico_id);

  for (int i = 0; i < PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2; i++) {
    // Byte index inside the uid array
    int bi = i / 2;
    // Use high nibble first to keep memory order (just cosmetics)
    uint8_t nibble = (pico_id.id[bi] >> 4) & 0x0F;
    pico_id.id[bi] <<= 4;
    // Binary to hex digit
    usb_serial[i] = nibble < 10 ? nibble + '0' : nibble + 'A' - 10;
  }
}

static const char *const string_desc_arr[] = {
    [USBD_STR_LANG]     = (const char[]) { 0x09, 0x04 }, // 0: is supported language is English (0x0409)
    [USBD_STR_MANUF]    = "Raspberry Pi",
    [USBD_STR_PRODUCT]  = "Pico Fuzix",
    [USBD_STR_SERIAL]   = usb_serial,
    [USBD_STR_CONSOLE]  = "Fuzix Console",
    [USBD_STR_LOG]      = "Fuzix Log",
    [USBD_STR_MPLEX]    = "Fuzix Multiplexer",
    [USBD_STR_CDC1]      = "User defined CDC 1",
    [USBD_STR_VENDOR1]   = "User defined Vendor 1",
    [USBD_STR_CDC2]      = "User defined CDC 2",
    [USBD_STR_VENDOR2]   = "User defined Vendor 2"
};

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    static uint16_t _desc_str[32];
    (void) langid;

    uint8_t chr_count;

    if ( index == 0) {
        _desc_str[1] = 0x0409; // supported language is English
        //memcpy(&_desc_str[1], string_desc_arr[0], 2);
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
#if USB_DEV_TTY1
  ITFNUM2, ITFNUM3_DATA,
#endif
#if USB_DEV_TTY2
  ITFNUM3, ITFNUM5_DATA,
#endif
#if USB_DEV_MPLEX
  ITFNUM4, ITFNUM2_DATA,
#endif
#if USB_DEV_RAW1
  ITFNUM5, ITFNUM4_DATA,
#endif
#if USB_DEV_RAW2
  ITFNUM6, ITFNUM6_DATA,
#endif
};

#define CONFIG_TOTAL_LEN (  (TUD_CONFIG_DESC_LEN)                   + \
                            (TUD_CDC_DESC_LEN     * CFG_TUD_CDC   ) + \
                            (TUD_VENDOR_DESC_LEN  * CFG_TUD_VENDOR) \
                         )

static const uint8_t desc_configuration[] = {
    // header
    TUD_CONFIG_DESCRIPTOR(1, (CFG_TUD_CDC*2)+CFG_TUD_VENDOR, USBD_STR_LANG, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, USBD_MAX_POWER_MA),
    // Interfaces
#if USB_DEV_CONSOLE
    TUD_CDC_DESCRIPTOR(ITFNUM0, USBD_STR_CONSOLE, EPNUM1, USB_PACKET_MAX_SIZE_DATA_SINGLE, EPNUM2 & 0x7F, EPNUM2, USB_PACKET_MAX_SIZE_DATA_SINGLE),
#endif
#if USB_DEV_LOG
    TUD_CDC_DESCRIPTOR(ITFNUM1, USBD_STR_LOG, EPNUM3, USB_PACKET_MAX_SIZE_DATA_SINGLE, EPNUM4 & 0x7F, EPNUM4, USB_PACKET_MAX_SIZE_DATA_SINGLE),
#endif
#if USB_DEV_TTY1
    TUD_CDC_DESCRIPTOR(ITFNUM2, USBD_STR_CDC1, EPNUM5, USB_PACKET_MAX_SIZE_DATA_SINGLE, EPNUM6 & 0x7F, EPNUM6, USB_PACKET_MAX_SIZE_DATA_SINGLE),
#endif
#if USB_DEV_TTY2
    TUD_CDC_DESCRIPTOR(ITFNUM3, USBD_STR_CDC2, EPNUM7, USB_PACKET_MAX_SIZE_DATA_SINGLE, EPNUM8 & 0x7F, EPNUM8, USB_PACKET_MAX_SIZE_DATA_SINGLE),
#endif
#if USB_DEV_MPLEX
    TUD_VENDOR_DESCRIPTOR(ITFNUM4, USBD_STR_MPLEX, EPNUM9 & 0x7F, EPNUM10, USB_PACKET_MAX_SIZE_DATA_SINGLE),
#endif
#if USB_DEV_RAW1
    TUD_VENDOR_DESCRIPTOR(ITFNUM5, USBD_STR_VENDOR1, EPNUM11 & 0x7F, EPNUM12, USB_PACKET_MAX_SIZE_DATA_SINGLE),
#endif
#if USB_DEV_RAW2
    TUD_VENDOR_DESCRIPTOR(ITFNUM6, USBD_STR_VENDOR2, EPNUM13 & 0x7F, EPNUM14, USB_PACKET_MAX_SIZE_DATA_SINGLE),
#endif
};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_descriptor_configuration_cb(uint8_t index) {
  (void) index; // for multiple configurations
  return desc_configuration;
}


//--------------------------------------------------------------------+
// stdio drivers
//--------------------------------------------------------------------+

#define STDIO_USB_STDOUT_TIMEOUT_US 500000

static void stdio_usb_out_chars(uint8_t itf, const char *buf, int length) {
    static uint64_t last_avail_time;
    if (tud_cdc_n_connected(itf)) {
        for (int i = 0; i < length;) {
            int n = length - i;
            int avail = tud_cdc_n_write_available(itf);
            if (n > avail) n = avail;
            if (n) {
                int n2 = tud_cdc_n_write(itf, buf + i, n);
                tud_task();
                tud_cdc_n_write_flush(itf);
                i += n2;
                last_avail_time = time_us_64();
            } else {
                tud_task();
                tud_cdc_n_write_flush(itf);
                if (!tud_cdc_n_connected(itf) ||
                    (!tud_cdc_n_write_available(itf) && time_us_64() > last_avail_time + STDIO_USB_STDOUT_TIMEOUT_US)) {
                    break;
                }
            }
        }
    } else {
        // reset our timeout
        last_avail_time = 0;
    }
}
static int stdio_usb_in_chars(uint8_t itf, char *buf, int length) {
    int rc = PICO_ERROR_NO_DATA;
    if (tud_cdc_n_connected(itf) && tud_cdc_n_available(itf)) {
        int count = tud_cdc_n_read(itf, buf, length);
        rc =  count ? count : PICO_ERROR_NO_DATA;
    }
    return rc;
}

static void stdio_usb_cdc0_out_chars(const char *buf, int len) {
  stdio_usb_out_chars(0, buf, len);
}
static int stdio_usb_cdc0_in_chars(char *buf, int len) {
  return stdio_usb_in_chars(0, buf, len);
}
static void stdio_usb_cdc1_out_chars(const char *buf, int len) {
  stdio_usb_out_chars(1, buf, len);
}
static int stdio_usb_cdc1_in_chars(char *buf, int len) {
  return stdio_usb_in_chars(1, buf, len);
}
static void stdio_usb_cdc2_out_chars(const char *buf, int len) {
  stdio_usb_out_chars(2, buf, len);
}
static int stdio_usb_cdc2_in_chars(char *buf, int len) {
  return stdio_usb_in_chars(2, buf, len);
}
static void stdio_usb_cdc3_out_chars(const char *buf, int len) {
  stdio_usb_out_chars(3, buf, len);
}
static int stdio_usb_cdc3_in_chars(char *buf, int len) {
  return stdio_usb_in_chars(3, buf, len);
}
static void stdio_usb_cdc4_out_chars(const char *buf, int len) {
  stdio_usb_out_chars(4, buf, len);
}
static int stdio_usb_cdc4_in_chars(char *buf, int len) {
  return stdio_usb_in_chars(4, buf, len);
}
static void stdio_usb_cdc5_out_chars(const char *buf, int len) {
  stdio_usb_out_chars(5, buf, len);
}
static int stdio_usb_cdc5_in_chars(char *buf, int len) {
  return stdio_usb_in_chars(5, buf, len);
}

static stdio_driver_t stdio_usb_cdc_driver[6] = {
  {
  .out_chars = stdio_usb_cdc0_out_chars,
  .in_chars = stdio_usb_cdc0_in_chars,
  .crlf_enabled = true
  },
  {
  .out_chars = stdio_usb_cdc1_out_chars,
  .in_chars = stdio_usb_cdc1_in_chars,
  .crlf_enabled = true
  },
  {
  .out_chars = stdio_usb_cdc2_out_chars,
  .in_chars = stdio_usb_cdc2_in_chars,
  .crlf_enabled = true
  },
  {
  .out_chars = stdio_usb_cdc3_out_chars,
  .in_chars = stdio_usb_cdc3_in_chars,
  .crlf_enabled = true
  },
  {
  .out_chars = stdio_usb_cdc4_out_chars,
  .in_chars = stdio_usb_cdc4_in_chars,
  .crlf_enabled = true
  },
  {
  .out_chars = stdio_usb_cdc5_out_chars,
  .in_chars = stdio_usb_cdc5_in_chars,
  .crlf_enabled = true
  }
};

void devusb_cdc_stdio(uint8_t id, bool stdio) {
  stdio_set_driver_enabled(&stdio_usb_cdc_driver[id], stdio);
  if (stdio) {
    LOG_INF("stdio on USB CDC %d", id);
    LOG_EME("emergency log entry");
    LOG_ALE("alert log entry");
    LOG_CRI("critical log entry");
    LOG_ERR("error log entry");
    LOG_WAR("warning log entry");
    LOG_NOT("notice log entry");
    LOG_INF("info log entry");
    LOG_DEB("debug log entry");
    unsigned char data[20] = {32, 1, 24, 56, 102, 5, 78, 92, 200, 0, 32, 1, 24, 56, 102, 5, 78, 92, 200, 0};
    LOG_HEX(data, 20, "hex %s", "log entry");
  } else {
    LOG_INF("USB CDC %d free'd from stdio", id);
  }
}


//--------------------------------------------------------------------+
// callbacks
//--------------------------------------------------------------------+

// CDC: Invoked when received new data
void tud_cdc_rx_cb(uint8_t itf) {
  printf("tud_cdc_rx_cb %d\n", itf);
}

// CDC: Invoked when received `wanted_char`
//void tud_cdc_rx_wanted_cb(uint8_t itf, char wanted_char) {}

// CDC: Invoked when space becomes available in TX buffer
void tud_cdc_tx_complete_cb(uint8_t itf) {
  //printf("tud_cdc_tx_complete_cb %d\n", itf);
}

// CDC: Invoked when line state DTR & RTS are changed via SET_CONTROL_LINE_STATE
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
  printf("tud_cdc_line_state_cb %d\n", itf);
}

// CDC: Invoked when line coding is change via SET_LINE_CODING
void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const* line_coding) {
  printf("tud_cdc_line_coding_cb %d\n", itf);
}

// CDC: Invoked when received send break
void tud_cdc_send_break_cb(uint8_t itf, uint16_t duration_ms) {
  printf("tud_cdc_send_break_cb %d\n", itf);
}

// Vendor: Invoked when received new data
void tud_vendor_rx_cb(uint8_t itf) {
  printf("tud_vendor_rx_cb %d\n", itf);
}


//--------------------------------------------------------------------+
// tinyusb wrapper
//--------------------------------------------------------------------+

repeating_timer_t tusb_timer;

static bool tusb_handler(repeating_timer_t *rt) {
  tud_task();
  return true;
}

repeating_timer_t tusb_test;

void devusb_init(uint8_t stdio) {  
  tusb_id2str();
  tusb_init();
  add_repeating_timer_us(1000, tusb_handler, NULL, &tusb_timer);

  if (stdio&&(stdio<=CFG_TUD_CDC)) {
    devusb_cdc_stdio(stdio, true);
  } else if (stdio) {
    LOG_WAR("%d out of bounduaries, USB CDC number must be between 1 and %d", stdio, CFG_TUD_CDC);
    return;
  }
}
