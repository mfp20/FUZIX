#include "rt_stdio.h"
#include "rt_log.h"
#include "rt_softirq.h"
#include "rt_chardev.h"

#include <stdio.h>
#include <stdarg.h>

char stdio_buffer[256];
static int stdio_buffer_index = 0;

uint8_t log_level = LOG_LEVEL;

//--------------------------------------------------------------------+
// enhanced logging
//--------------------------------------------------------------------+

static void stdio_buffer_flush(void) {
	for (uint32_t i=0;i<stdio_buffer_index;i++)
		rt_select_write(stdio_buffer[i]);
	stdio_buffer_index = 0;
}

void stdio_putchar(uint8_t b) {
	stdio_buffer[stdio_buffer_index] = b;
	stdio_buffer_index++;
	// truncate and print
	if (stdio_buffer_index==254) {
		if (stdio_buffer[stdio_buffer_index] != '\n') {
			stdio_buffer[stdio_buffer_index] = '\n';
		}
		stdio_buffer[stdio_buffer_index+1] = '\0';
		stdio_buffer_flush();
		WARN("Previous stdio buffer truncated to 255 chars.");
		return;
	}
	// terminate string and print
	if (b == '\n') {
		stdio_buffer[stdio_buffer_index] = '\0';
		stdio_buffer_flush();
		return;
	}
}

void stdio_kputchar(uint8_t b) {
	if (stdio_buffer_index==0) {
		FUZIX_LOG_HEADER;
	}
	stdio_putchar(b);
}

void stdio_printf(const char *fmt, ...) {
    va_list arglist;
    va_start( arglist, fmt );
    stdio_buffer_index = vsnprintf(stdio_buffer, 256, fmt, arglist);
    va_end( arglist );
	if ((stdio_buffer_index>0)&&(stdio_buffer_index<256)) {
		stdio_buffer_flush();
	}
	else
	{
		stdio_buffer_flush();
		WARN("Previous stdio buffer truncated to 255 chars.");
	}
}

void log_set_level(uint8_t level)
{
	if (log_level > LOG_LEVEL)
		level = LOG_LEVEL;
	log_level = level;
}

void log_snprintf_hex(unsigned char *in, unsigned int count, char *out)
{
	unsigned int size = 0;
	char ascii[16] = {};
	int index = 0;
	while (size < count)
	{
		if (!(size % 0x10))
		{
			index += snprintf(out + index, count * 5, "\n\t\t%08x: ", size);
		}
		else if (!(size % 8))
		{
			//	After per 8 bytes insert two space for split
			index += snprintf(out + index, count * 5, " ");
		}

		// handle Ascii detail area, store current byte
		ascii[size % 16] = ((in[size] >= '!') && (in[size] <= '~')) ? in[size] : '.';

		// print current byte
		index += snprintf(out + index, count * 5, "%02x ", in[size]);
		size++;

		//
		if (!(size % 16) || (size == count))
		{
			//	Empty bytes
			unsigned char len = size % 16;
			if (len)
			{
				len = 16 - len;
				while (len--)
				{
					index += snprintf(out + index, count * 5, "   ");
					if (len == 8)
					{
						index += snprintf(out + index, count * 5, " ");
					}
				}
			}
			index += snprintf(out + index, count * 5, "    %s", ascii);
			if (size == count)
				break;
		}
	}
}

void log_test_color(void) {
	unsigned char data[20] = {32, 1, 24, 56, 102, 5, 78, 92, 200, 0, 32, 1, 24, 56, 102, 5, 78, 92, 200, 0};

	stdio_printf(LOG_STYLE_CLEAR "------ stdio color test start ------\n");
	stdio_printf("NOTE: if you have problems reading the folowing lines\n");
	stdio_printf("      enable ANSI colors on your terminal\n");
	stdio_printf("      or disable colors in config.h\n");
	EMERG("emergency log entry");
	ALERT("alert log entry");
	CRIT("critical log entry");
	ERR("error log entry");
	WARN("warning log entry");
	FUZIX_LOG_HEADER;stdio_printf("fuzix kernel log entry\n");
	NOTICE("notice log entry");
	INFO("info log entry");
	DEBUG("debug log entry");
	//HEX(data, 20, "hex %s", "log entry");
	stdio_printf("                ------ stdio color test end ------\n");
}

//--------------------------------------------------------------------+
// stdio driver
//--------------------------------------------------------------------+

static void stdio_out_chars(const char *buf, int len)
{
	stdio_printf("%.*s", len, buf);
}

static int stdio_in_chars(char *buf, int len)
{
	for (uint32_t i=0;i<len;i++)
		buf[i] = rt_select_read();
	return len;
}

stdio_driver_t stdio_driver = {
	.out_chars = stdio_out_chars,
	.in_chars = stdio_in_chars,
	.crlf_enabled = true
};

/* vim: sw=4 ts=4 et: */
