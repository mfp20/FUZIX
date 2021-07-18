#ifndef _RT_LOG_DOT_H
#define _RT_LOG_DOT_H

#include "rt_stdio.h"
#include "rt_time.h"

#include <string.h>

// colors
#ifndef LOG_COLOR
#define LOG_COLOR (0)
#endif
#define LOG_COLOR_NONE "\e[0m"
#define LOG_COLOR_BLACK "\e[0;30m"
#define LOG_COLOR_L_BLACK "\e[1;30m"
#define LOG_COLOR_RED "\e[0;31m"
#define LOG_COLOR_L_RED "\e[1;31m"
#define LOG_COLOR_GREEN "\e[0;32m"
#define LOG_COLOR_L_GREEN "\e[1;32m"
#define LOG_COLOR_BROWN "\e[0;33m"
#define LOG_COLOR_YELLOW "\e[1;33m"
#define LOG_COLOR_BLUE "\e[0;34m"
#define LOG_COLOR_L_BLUE "\e[1;34m"
#define LOG_COLOR_PURPLE "\e[0;35m"
#define LOG_COLOR_L_PURPLE "\e[1;35m"
#define LOG_COLOR_CYAN "\e[0;36m"
#define LOG_COLOR_L_CYAN "\e[1;36m"
#define LOG_COLOR_GRAY "\e[0;37m"
#define LOG_COLOR_WHITE "\e[1;37m"
#define LOG_STYLE_BOLD "\e[1m"
#define LOG_STYLE_UNDERLINE "\e[4m"
#define LOG_STYLE_BLINK "\e[5m"
#define LOG_STYLE_REVERSE "\e[7m"
#define LOG_STYLE_HIDE "\e[8m"
#define LOG_STYLE_CLEAR "\e[2J"
#define LOG_STYLE_CLRLINE "\r\e[K" //or "\e[1K\r"

// levels (syslog-ish)
#ifndef LOG_LEVEL
#define LOG_LEVEL (7)
#endif
#define LOG_LEVEL_EMERG (0)
#define LOG_LEVEL_ALERT (1)
#define LOG_LEVEL_CRIT (2)
#define LOG_LEVEL_ERR (3)
#define LOG_LEVEL_WARN (4)
#define LOG_LEVEL_FUZIX (5)
#define LOG_LEVEL_NOTICE (6)
#define LOG_LEVEL_INFO (7)
#define LOG_LEVEL_DEBUG (8)

// level
#define LOG_CONTENT_LEVEL_EMERG		LOG_COLOR_PURPLE LOG_STYLE_REVERSE	"[EMERG]\t" LOG_COLOR_NONE
#define LOG_CONTENT_LEVEL_ALERT		LOG_COLOR_PURPLE	"[ALERT]\t" LOG_COLOR_NONE
#define LOG_CONTENT_LEVEL_CRIT		LOG_COLOR_RED		"[CRIT]\t"	LOG_COLOR_NONE
#define LOG_CONTENT_LEVEL_ERR		LOG_COLOR_BROWN 	"[ERR]\t"	LOG_COLOR_NONE
#define LOG_CONTENT_LEVEL_WARN		LOG_COLOR_YELLOW 	"[WARNING]\t" LOG_COLOR_NONE
#define LOG_CONTENT_LEVEL_FUZIX		LOG_COLOR_WHITE		"[FUZIX]\t" LOG_COLOR_NONE
#define LOG_CONTENT_LEVEL_NOTICE	LOG_COLOR_CYAN		"[NOTICE]\t" LOG_COLOR_NONE
#define LOG_CONTENT_LEVEL_INFO		LOG_COLOR_GREEN  	"[INFO]\t"	LOG_COLOR_NONE
#define LOG_CONTENT_LEVEL_DEBUG		LOG_COLOR_GREEN LOG_STYLE_REVERSE	"[DEBUG]\t"	LOG_COLOR_NONE
#define LOG_CONTENT_LEVEL_HEX		LOG_COLOR_GREEN LOG_STYLE_REVERSE	"[HEX]\t"	LOG_COLOR_NONE

// time
#define LOG_CONTENT_TIME_FMT LOG_COLOR_WHITE "%010ld "
#define LOG_CONTENT_TIME_VARS monotonic32()

// true for windows, false for linux/mac
#if true
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif

#ifdef BUILD_DEBUG
#define LOG_CONTENT_ERRFILE_FMT "%20s :: %3d :: %15s\t"
#define LOG_CONTENT_ERRFILE_VARS __FILENAME__, __LINE__, __func__
#define LOG_CONTENT_FMT(level)		LOG_CONTENT_TIME_FMT LOG_##level LOG_COLOR_WHITE LOG_CONTENT_ERRFILE_FMT LOG_COLOR_NONE
#define LOG_CONTENT_VARS			LOG_CONTENT_TIME_VARS, LOG_CONTENT_ERRFILE_VARS
#else
#define LOG_CONTENT_FMT(level)		LOG_CONTENT_TIME_FMT LOG_##level
#define LOG_CONTENT_VARS			LOG_CONTENT_TIME_VARS
#endif

//
#define EMERG(STR, ...)		stdio_printf(LOG_CONTENT_FMT(CONTENT_LEVEL_EMERG)	STR LOG_COLOR_NONE "\n", LOG_CONTENT_VARS, ##__VA_ARGS__)
#define ALERT(STR, ...)		stdio_printf(LOG_CONTENT_FMT(CONTENT_LEVEL_ALERT)	STR LOG_COLOR_NONE "\n", LOG_CONTENT_VARS, ##__VA_ARGS__)
#define CRIT(STR, ...)		stdio_printf(LOG_CONTENT_FMT(CONTENT_LEVEL_CRIT)	STR LOG_COLOR_NONE "\n", LOG_CONTENT_VARS, ##__VA_ARGS__)
#define ERR(STR, ...)		stdio_printf(LOG_CONTENT_FMT(CONTENT_LEVEL_ERR)		STR LOG_COLOR_NONE "\n", LOG_CONTENT_VARS, ##__VA_ARGS__)
#define WARN(STR, ...)		stdio_printf(LOG_CONTENT_FMT(CONTENT_LEVEL_WARN)	STR LOG_COLOR_NONE "\n", LOG_CONTENT_VARS, ##__VA_ARGS__)
#define FUZIX_LOG_HEADER	stdio_printf(LOG_CONTENT_FMT(CONTENT_LEVEL_FUZIX)	LOG_COLOR_NONE, LOG_CONTENT_VARS)
#define NOTICE(STR, ...)	stdio_printf(LOG_CONTENT_FMT(CONTENT_LEVEL_NOTICE)	STR LOG_COLOR_NONE "\n", LOG_CONTENT_VARS, ##__VA_ARGS__)
#define INFO(STR, ...)		stdio_printf(LOG_CONTENT_FMT(CONTENT_LEVEL_INFO)	STR LOG_COLOR_NONE "\n", LOG_CONTENT_VARS, ##__VA_ARGS__)
#define DEBUG(STR, ...)		stdio_printf(LOG_CONTENT_FMT(CONTENT_LEVEL_DEBUG)	STR LOG_COLOR_NONE "\n", LOG_CONTENT_VARS, ##__VA_ARGS__)
//#define HEX(PTR, LEN, STR, ...)		LOG_FMT_ARGS(LOG_CONTENT_TIME_FMT LOG_CONTENT_LEVEL_HEX LOG_CONTENT_ERRFILE_FMT STR, LOG_CONTENT_VARS, ##__VA_ARGS__)

// undefine colors according to configuration
#if LOG_COLOR < 1

#undef LOG_COLOR_NONE
#define LOG_COLOR_NONE

#undef LOG_COLOR_RED
#define LOG_COLOR_RED

#undef LOG_COLOR_PURPLE
#define LOG_COLOR_PURPLE

#undef LOG_COLOR_YELLOW
#define LOG_COLOR_YELLOW

#undef LOG_COLOR_BROWN
#define LOG_COLOR_BROWN

#undef LOG_COLOR_GREEN
#define LOG_COLOR_GREEN

#undef LOG_COLOR_CYAN
#define LOG_COLOR_CYAN

#undef LOG_COLOR_BLUE
#define LOG_COLOR_BLUE

#undef LOG_COLOR_GRAY
#define LOG_COLOR_GRAY

#endif

// runtime log level setup
extern uint8_t log_level;

#endif
