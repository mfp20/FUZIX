#include "rt_log.h"

uint8_t log_level = LOG_LEVEL;

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
	stdio_printf("\t\t\t------ stdio color test end ------\n");
}
