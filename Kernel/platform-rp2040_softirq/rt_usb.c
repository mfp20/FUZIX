#include "rt_log.h"
#include "rt_time.h"
#include "rt_softirq.h"
#include "rt_chardev.h"
#include "rt_blockdev.h"
#include "rt_usb.h"
#include "rt_usb_mplex.h"
#include "rt_fuzix.h"

#include <tusb.h>

#define EPNUM0 0x80 // usb control endpoint, not usable for interfaces
#define EPNUM1 0x81
#define EPNUM2 0x82
#define EPNUM3 0x83
#define EPNUM4 0x84
#define EPNUM5 0x85
#define EPNUM6 0x86
#define EPNUM7 0x87
#define EPNUM8 0x88
#define EPNUM9 0x89
#define EPNUM10 0x8a
#define EPNUM11 0x8b
#define EPNUM12 0x8c
#define EPNUM13 0x8d
#define EPNUM14 0x8e
#define EPNUM15 0x8f

#define USBD_VID (0x2E8A) // Raspberry Pi
// Temporary setup until I figure out how to dynamically reconfigure USB at runtime
// PID depends on the 8 interfaces availability and we have 16 bits to flag:
// (MSb) 0--- ---- ---- ---- (LSb)
//       |||| |||| |||| |||`-----  cdc tty1
//       |||| |||| |||| ||`------  cdc tty2
//       |||| |||| |||| |`-------  cdc tty3
//       |||| |||| ||||  `-------  cdc spare
//       |||| |||| |||`----------  unused
//       |||| |||| ||`-----------  unused
//       |||| |||| |`------------  unused
//       |||| |||| `-------------  unused
//       |||| |||`---------------  system binary multiplexer
//       |||| ||`----------------  user binary multiplexer
//       |||| |`-----------------  vendor spare
//       |||| `------------------  unused
//       |||`--------------------  unused
//       ||`---------------------  unused
//       |`----------------------  unused
//       `-----------------------  set to 0 to avoid PID 0xFFFF
#define PID_MAP(dev, nbit) ((USB_DEV_##dev) << (nbit))
#define USBD_PID (        \
	PID_MAP(TTY1, 1) + \
	PID_MAP(TTY2, 2) +     \
	PID_MAP(TTY3, 3) +    \
	PID_MAP(CDC, 4) +    \
	PID_MAP(MPLEX_SYS, 9) +   \
	PID_MAP(MPLEX_USR, 10) +   \
	PID_MAP(VENDOR, 11) +   \
	0)

#define USBD_STR_LANG (0x00)
#define USBD_STR_MANUF (0x01)
#define USBD_STR_PRODUCT (0x02)
#define USBD_STR_SERIAL (0x03)
#define USBD_STR_TTY1 (0x04)
#define USBD_STR_TTY2 (0x05)
#define USBD_STR_TTY3 (0x06)
#define USBD_STR_CDC (0x07)
#define USBD_STR_MPLEX_SYS (0x08)
#define USBD_STR_MPLEX_USR (0x09)
#define USBD_STR_VENDOR (0x0a)

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+

// Note: descriptors returned from callbacks must exist long enough for transfer to complete
tusb_desc_device_t const desc_device = {
	.bLength = sizeof(tusb_desc_device_t),
	.bDescriptorType = TUSB_DESC_DEVICE,
	.bcdUSB = 0x0200,					  // USB Specification version 2.0 (TODO check polling rate, 2.0 should be 125us)
	.bDeviceClass = 0xEF,				  // Multi-interface Function Code Device
	.bDeviceSubClass = 0x02,			  // Common Class Sub Class
	.bDeviceProtocol = MISC_PROTOCOL_IAD, // Interface Association Descriptor protocol
	.bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
	.idVendor = USBD_VID,
	.idProduct = USBD_PID,
	.bcdDevice = 0x0001, // Version 00.01
	.iManufacturer = USBD_STR_MANUF,
	.iProduct = USBD_STR_PRODUCT,
	.iSerialNumber = USBD_STR_SERIAL,
	.bNumConfigurations = 0x01};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
const uint8_t *tud_descriptor_device_cb(void)
{
	return (const uint8_t *)&desc_device;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

enum
{
#if USB_DEV_TTY1
	ITFNUM0,
	ITFNUM0_DATA,
#endif
#if USB_DEV_TTY2
	ITFNUM1,
	ITFNUM1_DATA,
#endif
#if USB_DEV_TTY3
	ITFNUM2,
	ITFNUM3_DATA,
#endif
#if USB_DEV_CDC
	ITFNUM3,
	ITFNUM5_DATA,
#endif
#if USB_DEV_MPLEX_SYS
	ITFNUM4,
	ITFNUM2_DATA,
#endif
#if USB_DEV_MPLEX_USR
	ITFNUM5,
	ITFNUM4_DATA,
#endif
#if USB_DEV_VENDOR
	ITFNUM6,
	ITFNUM6_DATA,
#endif
};

#define CONFIG_TOTAL_LEN ((TUD_CONFIG_DESC_LEN) +            \
						  (TUD_CDC_DESC_LEN * CFG_TUD_CDC) + \
						  (TUD_VENDOR_DESC_LEN * CFG_TUD_VENDOR))

static const uint8_t desc_configuration[] = {
	// header
	TUD_CONFIG_DESCRIPTOR(1, (CFG_TUD_CDC * 2) + CFG_TUD_VENDOR, USBD_STR_LANG, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, USBD_MAX_POWER_MA),
// Interfaces
#if USB_DEV_TTY1
	TUD_CDC_DESCRIPTOR(ITFNUM0, USBD_STR_TTY1, EPNUM1, USB_PACKET_MAX_SIZE_DATA_SINGLE, EPNUM2 & 0x7F, EPNUM2, USB_PACKET_MAX_SIZE_DATA_SINGLE),
#endif
#if USB_DEV_TTY2
	TUD_CDC_DESCRIPTOR(ITFNUM1, USBD_STR_TTY2, EPNUM3, USB_PACKET_MAX_SIZE_DATA_SINGLE, EPNUM4 & 0x7F, EPNUM4, USB_PACKET_MAX_SIZE_DATA_SINGLE),
#endif
#if USB_DEV_TTY3
	TUD_CDC_DESCRIPTOR(ITFNUM2, USBD_STR_TTY3, EPNUM5, USB_PACKET_MAX_SIZE_DATA_SINGLE, EPNUM6 & 0x7F, EPNUM6, USB_PACKET_MAX_SIZE_DATA_SINGLE),
#endif
#if USB_DEV_CDC
	TUD_CDC_DESCRIPTOR(ITFNUM3, USBD_STR_CDC, EPNUM7, USB_PACKET_MAX_SIZE_DATA_SINGLE, EPNUM8 & 0x7F, EPNUM8, USB_PACKET_MAX_SIZE_DATA_SINGLE),
#endif
#if USB_DEV_MPLEX_SYS
	TUD_VENDOR_DESCRIPTOR(ITFNUM4, USBD_STR_MPLEX_SYS, EPNUM9 & 0x7F, EPNUM10, USB_PACKET_MAX_SIZE_DATA_SINGLE),
#endif
#if USB_DEV_MPLEX_USR
	TUD_VENDOR_DESCRIPTOR(ITFNUM5, USBD_STR_MPLEX_USR, EPNUM11 & 0x7F, EPNUM12, USB_PACKET_MAX_SIZE_DATA_SINGLE),
#endif
#if USB_DEV_VENDOR
	TUD_VENDOR_DESCRIPTOR(ITFNUM6, USBD_STR_VENDOR, EPNUM13 & 0x7F, EPNUM14, USB_PACKET_MAX_SIZE_DATA_SINGLE),
#endif
};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
	(void)index; // for multiple configurations
	return desc_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// C string for iSerialNumber in USB Device Descriptor, two chars per byte + terminating NULL
char usb_serial[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];

// convert pico_id into a string to be used as descriptor
void tusb_id2str(void)
{
	// Why a uint8_t[8] array inside a struct instead of an uint64_t an inquiring mind might wonder
	pico_unique_board_id_t pico_id;
	// get board id for later use in USB descriptor array
	pico_get_unique_board_id(&pico_id);

	for (int i = 0; i < PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2; i++)
	{
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
	[USBD_STR_LANG] = (const char[]){0x09, 0x04}, // 0: is supported language is English (0x0409)
	[USBD_STR_MANUF] = "Raspberry Pi",
	[USBD_STR_PRODUCT] = "Pico Fuzix",
	[USBD_STR_SERIAL] = usb_serial,
	[USBD_STR_TTY1] = "Fuzix tty1 (system console)",
	[USBD_STR_TTY2] = "Fuzix tty2 (system log)",
	[USBD_STR_MPLEX_SYS] = "Fuzix binary interface",
	[USBD_STR_TTY3] = "Fuzix tty3 (user chardev)",
	[USBD_STR_MPLEX_USR] = "User binary interface",
	[USBD_STR_CDC] = "Pico chardev",
	[USBD_STR_VENDOR] = "Pico protocol"};

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
	static uint16_t _desc_str[32];
	(void)langid;

	uint8_t chr_count;

	if (index == 0)
	{
		_desc_str[1] = 0x0409; // supported language is English
		//memcpy(&_desc_str[1], string_desc_arr[0], 2);
		chr_count = 1;
	}
	else
	{
		// Convert ASCII string into UTF-16
		if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])))
			return NULL;

		const char *str = string_desc_arr[index];

		// Cap at max char
		chr_count = strlen(str);
		if (chr_count > 31)
			chr_count = 31;

		for (uint8_t i = 0; i < chr_count; i++)
		{
			_desc_str[1 + i] = str[i];
		}
	}

	// first byte is length (including header), second byte is string type
	_desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);

	return _desc_str;
}

//--------------------------------------------------------------------+
// tinyusb callbacks
//--------------------------------------------------------------------+

// callbacks for received data
static byte_tx_t cdc3_cb = NULL;
static byte_tx_t vend1_cb = NULL;
static byte_tx_t vend2_cb = NULL;

// CDC: Invoked when received new data
void tud_cdc_rx_cb(uint8_t itf)
{
	if (itf == 0)
	{
		uint8_t b = (uint8_t)tud_cdc_n_read_char(0);
		// TODO skip softirq
		//if (fuzix_ready && queue_is_empty(&softirq_out_q))
		//{
		//	fuzix_tty1_write(b);
		//}
		//else
		{
			softirq_out(DEV_ID_TTY1, b, 0, NULL);
		}
	}
	else if (itf == 1)
	{
		uint8_t b = (uint8_t)tud_cdc_n_read_char(1);
		// TODO skip softirq
		//if (fuzix_ready && queue_is_empty(&softirq_out_q))
		//{
		//	fuzix_tty2_write(b);
		//}
		//else
		{
			softirq_out(DEV_ID_TTY2, b, 0, NULL);
		}
	}
	else if (itf == 2)
	{
		uint8_t b = (uint8_t)tud_cdc_n_read_char(2);
		// TODO skip softirq
		//if (fuzix_ready && queue_is_empty(&softirq_out_q))
		//{
		//	fuzix_tty3_write(b);
		//}
		//else
		{
			softirq_out(DEV_ID_TTY3, b, 0, NULL);
		}
	}
	else if (itf == 3)
	{
		uint8_t b = (uint8_t)tud_cdc_n_read_char(3);
		if (cdc3_cb) cdc3_cb(b);
	}
}

// CDC: Invoked when received `wanted_char`
//void tud_cdc_rx_wanted_cb(uint8_t itf, char wanted_char) {}

// CDC: Invoked when space becomes available in TX buffer
//void tud_cdc_tx_complete_cb(uint8_t itf) {}

// CDC: Invoked when line state DTR & RTS are changed via SET_CONTROL_LINE_STATE
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
	//INFO("CDC %d: dtr %d, rts %d\n", itf, dtr, rts);
	if (dtr)
	{ // on connect
		//INFO("CDC%d connect", itf);
	}
	else
	{ // on disconnect
		//INFO("CDC%d disconnect", itf);
	}
}

// CDC: Invoked when line coding is changed via SET_LINE_CODING
//void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const *line_coding) {}

// CDC: Invoked when received send break
//void tud_cdc_send_break_cb(uint8_t itf, uint16_t duration_ms) {}

// Vendor: Invoked when received new data
void tud_vendor_rx_cb(uint8_t itf)
{
	INFO("tud_vendor_rx_cb %d\n", itf);
	if (itf==0) {
		// dispatch all packets in usb buffer
		do {
			uint32_t len = tud_vendor_n_available(itf);
			// first byte == packet size
			if (vend_expected==0) {
				usb_rx_packet_set_size();
				len--;
			}
			// whole packet
			if (len>=vend_expected) {
				usb_rx_packet_dispatch(vend_expected);
				len = len-vend_expected;
				// prepare next iteration
				if (len)
					usb_rx_packet_set_size();
				else
					vend_expected = 0;
			}
		} while (vend_expected);
	}
	else if (itf==1) {
		uint8_t b;
		while (tud_vendor_n_available(itf)>0) {
			tud_vendor_n_read(itf, &b, 1);
			if (vend1_cb)
				vend1_cb(b);
			else
                WARN("USB VEND1: packet received but callback is not set");
		}
	}
	else
	{
		uint8_t b;
		while (tud_vendor_n_available(itf)>0) {
			tud_vendor_n_read(itf, &b, 1);
			if (vend2_cb)
				vend2_cb(b);
			else
                WARN("USB VEND2: packet received but callback is not set");
		}
	}
}

//--------------------------------------------------------------------+
// API
//--------------------------------------------------------------------+

static repeating_timer_t tusb_timer;

static bool tusb_handler(repeating_timer_t *rt)
{
	tud_task();
	return true;
}

void usb_init(void)
{
	// tinyusb
	tusb_id2str();
	tusb_init();
    // uses lowest prio alarm pool
	alarm_pool_add_repeating_timer_us(alarm_pool[ALARM_POOL_BE], 125, tusb_handler, NULL, &tusb_timer); // USB 2.0 -> 125us microframes

	// connect to binary server on host and enable options
	INFO("USB waiting %lums for binary server...", USB_MPLEX_TIMEOUT);
	uint32_t start = monotonic32();
	uint32_t elapsed = 0;
	while (elapsed<USB_MPLEX_TIMEOUT*1000) {
		if (usb_connection_req())
			break;
		elapsed = monotonic32()-start;
	}
	if (usb_vend0_connected) {
		// set RTC date and time
		usb_datetime_get = usb_datetime_req;
		datetime_t t;
		usb_datetime_req(&t);
		rtc_datetime_set(t.year, t.month, t.day, t.dotw, t.hour, t.min, t.sec);
		char dt[32];
		rtc_datetime_tostring(&t, dt);
		INFO("USB time server enabled, new date and time: %s", dt);
		// chardevs
		if (usb_vend0_chardev_enabled)
			INFO("USB binary ttys enabled");
		// blockdevs
		if (usb_vend0_blockdev_enabled)
			INFO("USB external disks enabled");
	}
	else
		INFO("USB binary server not found, skipping");
}

void usb_cdc3_set_cb(byte_tx_t rx_cb) {
	cdc3_cb = rx_cb;
}

void usb_vend1_set_cb(byte_tx_t rx_cb) {
	vend1_cb = rx_cb;
}

void usb_vend2_set_cb(byte_tx_t rx_cb) {
	vend2_cb = rx_cb;
}

//--------------------------------------------------------------------+
// cdc chardev drivers
//--------------------------------------------------------------------+

static uint8_t usb_cdc_read(uint8_t cdc)
{
	if (tud_cdc_n_connected(cdc))
	{
		if (tud_cdc_n_available(cdc) > 0)
			return (uint8_t)tud_cdc_n_read_char(cdc);
		else
			WARN("CDC%d buffer full", cdc);
	}
	else
	{
		WARN("CDC%d not connected", cdc);
	}
	return 0;
}

static void usb_cdc_write(uint8_t cdc, uint8_t b)
{
	if (tud_cdc_n_connected(cdc))
	{
		if (tud_cdc_n_write_available(cdc) > 0) {
			tud_cdc_n_write_char(cdc, b);
			tud_cdc_n_write_flush(cdc);
		}
		else
			WARN("CDC%d buffer full", cdc);
	}
	else
		WARN("CDC%d not connected", cdc);
}

static bool usb_cdc_writable(uint8_t cdc)
{
	if (tud_cdc_n_connected(cdc))
	{
		return (tud_cdc_n_write_available(cdc) > 0);
	}
	return false;
}

uint8_t usb_cdc0_read(void)
{
	return usb_cdc_read(0);
}

void usb_cdc0_write(uint8_t b)
{
	usb_cdc_write(0, b);
}

bool usb_cdc0_writable(void)
{
	return usb_cdc_writable(0);
}

uint8_t usb_cdc1_read(void)
{
	return usb_cdc_read(1);
}

void usb_cdc1_write(uint8_t b)
{
	usb_cdc_write(1, b);
}

bool usb_cdc1_writable(void)
{
	return usb_cdc_writable(1);
}

uint8_t usb_cdc2_read(void)
{
	return usb_cdc_read(2);
}

void usb_cdc2_write(uint8_t b)
{
	usb_cdc_write(2, b);
}

bool usb_cdc2_writable(void)
{
	return usb_cdc_writable(2);
}
