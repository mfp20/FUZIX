#include "rt_log.h"
#include "rt_softirq.h"
#include "rt_chardev.h"
#include "rt_blockdev.h"
#include "rt_uart.h"
#include "rt_usb.h"
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
#define PID_MAP(dev, nbit) ((USB_DEV_##dev) << (nbit))
#define USBD_PID (        \
	PID_MAP(CONSOLE, 1) + \
	PID_MAP(LOG, 2) +     \
	PID_MAP(TTY1, 3) +    \
	PID_MAP(TTY2, 4) +    \
	PID_MAP(MPLEX, 9) +   \
	PID_MAP(RAW1, 10) +   \
	PID_MAP(RAW2, 11) +   \
	0)

#define USBD_STR_LANG (0x00)
#define USBD_STR_MANUF (0x01)
#define USBD_STR_PRODUCT (0x02)
#define USBD_STR_SERIAL (0x03)
#define USBD_STR_CONSOLE (0x04)
#define USBD_STR_LOG (0x05)
#define USBD_STR_CDC1 (0x06)
#define USBD_STR_CDC2 (0x07)
#define USBD_STR_MPLEX (0x08)
#define USBD_STR_VENDOR1 (0x09)
#define USBD_STR_VENDOR2 (0x0a)

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
#if USB_DEV_CONSOLE
	ITFNUM0,
	ITFNUM0_DATA,
#endif
#if USB_DEV_LOG
	ITFNUM1,
	ITFNUM1_DATA,
#endif
#if USB_DEV_TTY1
	ITFNUM2,
	ITFNUM3_DATA,
#endif
#if USB_DEV_TTY2
	ITFNUM3,
	ITFNUM5_DATA,
#endif
#if USB_DEV_MPLEX
	ITFNUM4,
	ITFNUM2_DATA,
#endif
#if USB_DEV_RAW1
	ITFNUM5,
	ITFNUM4_DATA,
#endif
#if USB_DEV_RAW2
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
	[USBD_STR_CONSOLE] = "Fuzix tty1 (system console)",
	[USBD_STR_LOG] = "Fuzix tty2 (system log)",
	[USBD_STR_MPLEX] = "Fuzix binary interface",
	[USBD_STR_CDC1] = "Fuzix tty3 (user chardev)",
	[USBD_STR_VENDOR1] = "User binary interface",
	[USBD_STR_CDC2] = "Pico chardev",
	[USBD_STR_VENDOR2] = "Pico protocol"};

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
static byte_tx_t cdc0_cb = NULL;
static byte_tx_t cdc1_cb = NULL;
static byte_tx_t cdc2_cb = NULL;
static byte_tx_t cdc3_cb = NULL;
static byte_tx_t vend1_cb = NULL;
static byte_tx_t vend2_cb = NULL;

// CDC: Invoked when received new data
void tud_cdc_rx_cb(uint8_t itf)
{
	if (itf == 0)
	{
		uint8_t b = (uint8_t)tud_cdc_n_read_char(0);
		if (fuzix_ready && queue_is_empty(&softirq_out_q) && cdc0_cb)
		{
			INFO("tud_cdc_rx_cb TODO direct route\n");
			cdc0_cb(b);
		}
		else
		{
			softirq_out(DEV_ID_TTY1, b, 0, NULL);
		}
	}
	else if (itf == 1)
	{
		uint8_t b = (uint8_t)tud_cdc_n_read_char(1);
		if (fuzix_ready && queue_is_empty(&softirq_out_q) && cdc1_cb)
		{
			INFO("tud_cdc_rx_cb TODO direct route\n");
			cdc1_cb(b);
		}
		else
		{
			softirq_out(DEV_ID_TTY2, b, 0, NULL);
		}
	}
	else if (itf == 2)
	{
		uint8_t b = (uint8_t)tud_cdc_n_read_char(2);
		if (cdc2_cb) cdc2_cb(b);
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
			// first byte
			if (vend_expected==0) {
				usb_packet_set_size();
				len--;
			}
			// whole packet ready
			if (len>=vend_expected) {
				usb_packet_dispatch(vend_expected);
				len = len-vend_expected;
				// prepare next iteration
				if (len)
					usb_packet_set_size();
				else
					vend_expected = itf;
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

//--------------------------------------------------------------------+
// vendor chardev interfaces
//--------------------------------------------------------------------+

static uint8_t usb_vend_read(uint8_t tty)
{
	// TODO unused?
	if (tud_vendor_n_mounted(0))
	{
		if (tud_vendor_n_available(0) > 0) {
			uint8_t b = 0;
			tud_vendor_n_read(0, &b, 1);
			return b;
		}
		else
			WARN("VEND0 buffer full");
	}
	else
	{
		WARN("VEND0 not connected");
	}
	return 0;
}

static void usb_vend_write(uint8_t tty, uint8_t b)
{
	if (tud_vendor_n_mounted(0))
	{
		if (tud_vendor_n_write_available(0) > 2) {
			uint8_t pid = USB_PACKET_ID_TTY1;
			if (tty==2)
				pid = USB_PACKET_ID_TTY2;
			if (tty==3)
				pid = USB_PACKET_ID_TTY3;
			uint8_t pkt[3] = { 2, pid, b };
			tud_vendor_n_write(0, &b, 3);
		}
		else
			WARN("VEND0 buffer full");
	}
	else
		WARN("VEND0 not connected");
}

static bool usb_vend_writable(uint8_t tty)
{
	if (tud_vendor_n_mounted(0))
	{
		return (tud_vendor_n_write_available(0) > 2);
	}
	return false;
}

uint8_t usb_vend_tty1_read(void)
{
	return usb_vend_read(1);
}

void usb_vend_tty1_write(uint8_t b)
{
	usb_vend_write(1, b);
}

bool usb_vend_tty1_writable(void)
{
	return usb_vend_writable(1);
}

uint8_t usb_vend_tty2_read(void)
{
	return usb_vend_read(2);
}

void usb_vend_tty2_write(uint8_t b)
{
	usb_vend_write(2, b);
}

bool usb_vend_tty2_writable(void)
{
	return usb_vend_writable(2);
}

uint8_t usb_vend_tty3_read(void)
{
	return usb_vend_read(3);
}

void usb_vend_tty3_write(uint8_t b)
{
	usb_vend_write(3, b);
}

bool usb_vend_tty3_writable(void)
{
	return usb_vend_writable(3);
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
	tusb_id2str();
	tusb_init();

    // uses lowest prio alarm pool
	alarm_pool_add_repeating_timer_us(alarm_pool[ALARM_POOL_BE], 125, tusb_handler, NULL, &tusb_timer); // USB 2.0 -> 125us microframes
}

void usb_cdc0_set_cb(byte_tx_t rx_cb) {
	cdc0_cb = rx_cb;
}

void usb_cdc1_set_cb(byte_tx_t rx_cb) {
	cdc1_cb = rx_cb;
}

void usb_cdc2_set_cb(byte_tx_t rx_cb) {
	cdc2_cb = rx_cb;
}

void usb_cdc3_set_cb(byte_tx_t rx_cb) {
	cdc3_cb = rx_cb;
}

void usb_vend0_set_cb(usb_disk_buffer_addr_fptr disk_block_addr,
						usb_disk_rx_fptr disk_rx,
						usb_packet_chardev_fptr packet_core1_rx,
						usb_packet_chardev_fptr packet_tty1_rx,
						usb_packet_chardev_fptr packet_tty2_rx,
						usb_packet_chardev_fptr packet_tty3_rx
						) {
	usb_disk_block_addr = disk_block_addr;
	usb_disk_rx = disk_rx;
	usb_packet_core1_rx = packet_core1_rx;
	usb_packet_tty1_rx = packet_tty1_rx;
	usb_packet_tty2_rx = packet_tty2_rx;
	usb_packet_tty3_rx = packet_tty3_rx;
}

void usb_vend1_set_cb(byte_tx_t rx_cb) {
	vend1_cb = rx_cb;
}

void usb_vend2_set_cb(byte_tx_t rx_cb) {
	vend2_cb = rx_cb;
}
