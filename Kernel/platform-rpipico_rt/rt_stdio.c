#include "rt_stdio.h"
#include "rt_log.h"
#include "rt_softirq.h"
#include "rt_chardev.h"

#include <stdio.h>
#include <stdarg.h>

static char buffer[256];

uint8_t log_level = LOG_LEVEL;

//--------------------------------------------------------------------+
// stdio driver
//--------------------------------------------------------------------+

static void stdio_out_chars(const char *buf, int len)
{
	for (uint32_t i=0;i<len;i++)
		rt_select_write(buf[i]);
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

//--------------------------------------------------------------------+
// enhanced logging
//--------------------------------------------------------------------+

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
	stdio_printf("\t\t\t------ stdio color test start ------\n");
	stdio_printf("\tNOTE: if you have problem to read the folowing lines just disable colors in config.h\n");
	EMERG("emergency log entry");
	ALERT("alert log entry");
	CRIT("critical log entry");
	ERR("error log entry");
	WARNING("warning log entry");
	NOTICE("notice log entry");
	INFO("info log entry");
	DEBUG("debug log entry");
	unsigned char data[20] = {32, 1, 24, 56, 102, 5, 78, 92, 200, 0, 32, 1, 24, 56, 102, 5, 78, 92, 200, 0};
	LOG_HEX(data, 20, "hex %s", "log entry");
	stdio_printf("\t\t\t------ stdio color test end ------\n");
}

void stdio_putchar(uint8_t b) {
	rt_select_write(b);
}

void stdio_printf(const char *fmt, ...) {
    va_list arglist;
    va_start( arglist, fmt );
    int len = vsnprintf(buffer, 256, fmt, arglist);
    va_end( arglist );
	if ((len>0)&&(len<256))
		stdio_out_chars(buffer, len);
	else {
		stdio_out_chars(buffer, len);
		WARNING("Last line truncated to 255 chars");
	}
}

void stdio_log(uint8_t level, const char *fmt, ...) {
    va_list arglist;
    va_start( arglist, fmt );
    vsnprintf(buffer, 256, fmt, arglist);
    va_end( arglist );

	stdio_printf("%s\n\r", buffer);
	return;

    switch (level) {
        case LEVEL_EMERG:
            LOG_EME("%s", buffer);
        break;
        case LEVEL_ALERT:
            LOG_ALE("%s", fmt);
        break;
        case LEVEL_CRIT:
            LOG_CRI("%s", fmt);
        break;
        case LEVEL_ERR:
            LOG_ERR("%s", fmt);
        break;
        case LEVEL_WARNING:
            LOG_WAR("%s", fmt);
        break;
        case LEVEL_NOTICE:
            LOG_NOT("%s", fmt);
        break;
        case LEVEL_INFO:
            LOG_INF("%s", buffer);
        break;
        case LEVEL_DEBUG:
            LOG_DEB("%s", fmt);
        break;
    }
}

/* vim: sw=4 ts=4 et: */
