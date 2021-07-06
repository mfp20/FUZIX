#ifndef _SDK_LOG_DOT_H
#define _SDK_LOG_DOT_H

#include "rt.h"
#include "rt_time.h"
#include "rt_stdio.h"

#include <stdio.h>	// printf
#include <errno.h>	// errno
#include <string.h> // strerror

// compile-time level setup
#ifndef LOG_LEVEL
#define LOG_LEVEL (7)
#endif
// runtime log level setup
extern uint8_t log_level;

#ifndef LOG_COLOR
#define LOG_COLOR (0)
#endif

// colors
#define NONE "\e[0m"
#define BLACK "\e[0;30m"
#define L_BLACK "\e[1;30m"
#define RED "\e[0;31m"
#define L_RED "\e[1;31m"
#define GREEN "\e[0;32m"
#define L_GREEN "\e[1;32m"
#define BROWN "\e[0;33m"
#define YELLOW "\e[1;33m"
#define BLUE "\e[0;34m"
#define L_BLUE "\e[1;34m"
#define PURPLE "\e[0;35m"
#define L_PURPLE "\e[1;35m"
#define CYAN "\e[0;36m"
#define L_CYAN "\e[1;36m"
#define GRAY "\e[0;37m"
#define WHITE "\e[1;37m"

#define BOLD "\e[1m"
#define UNDERLINE "\e[4m"
#define BLINK "\e[5m"
#define REVERSE "\e[7m"
#define HIDE "\e[8m"
#define CLEAR "\e[2J"
#define CLRLINE "\r\e[K" //or "\e[1K\r"

#if false
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif

/* safe readable version of errno */
#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#ifdef BUILD_DEBUG
#define LOG_CONTENT_FORMAT "(errno: %s) %s::%d::%s\t"
#define LOG_CONTENT_VARS clean_errno(), __FILENAME__, __LINE__, __func__
#else
#define LOG_CONTENT_FORMAT "(errno: %s) %s\t"
#define LOG_CONTENT_VARS clean_errno()
#endif

//
#define LOG_EME_UNFILTERED(M, ...) stdio_printf("%010ld " RED "[EMERG]   " LOG_CONTENT_FORMAT NONE M, monotonic32(), LOG_CONTENT_VARS, ##__VA_ARGS__);
#define LOG_ALE_UNFILTERED(M, ...) stdio_printf("%010ld " PURPLE "[ALERT]   " LOG_CONTENT_FORMAT NONE M, monotonic32(), LOG_CONTENT_VARS, ##__VA_ARGS__);
#define LOG_CRI_UNFILTERED(M, ...) stdio_printf("%010ld " YELLOW "[CRIT]    " LOG_CONTENT_FORMAT NONE M, monotonic32(), LOG_CONTENT_VARS, ##__VA_ARGS__);
#define LOG_ERR_UNFILTERED(M, ...) stdio_printf("%010ld " BROWN "[ERR]     " LOG_CONTENT_FORMAT NONE M, monotonic32(), LOG_CONTENT_VARS, ##__VA_ARGS__);
#define LOG_WAR_UNFILTERED(M, ...) stdio_printf("%010ld " GREEN "[WARNING] " LOG_CONTENT_FORMAT NONE M, monotonic32(), LOG_CONTENT_VARS, ##__VA_ARGS__);
#define LOG_NOT_UNFILTERED(M, ...) stdio_printf("%010ld " L_BLUE "[NOTICE]  " LOG_CONTENT_FORMAT NONE M, monotonic32(), LOG_CONTENT_VARS, ##__VA_ARGS__);
#define LOG_INF_UNFILTERED(M, ...) stdio_printf("%010ld " CYAN "[INFO]    " LOG_CONTENT_FORMAT NONE M, monotonic32(), LOG_CONTENT_VARS, ##__VA_ARGS__);
#define LOG_DEB_UNFILTERED(M, ...) stdio_printf("%010ld " GRAY "[DEBUG]   " LOG_CONTENT_FORMAT NONE M, monotonic32(), LOG_CONTENT_VARS, ##__VA_ARGS__);
void log_snprintf_hex(unsigned char *in, unsigned int count, char *out);
#define LOG_HEX_UNFILTERED(D, N, M, ...)                  \
	{                                                     \
		char str_val[N * 15];                             \
		log_snprintf_hex(D, N, str_val);                        \
		stdio_printf("%010ld "                            \
			   "[DEBUG]   (%d Bytes) " M "%s",            \
			   monotonic32(), N, ##__VA_ARGS__, str_val); \
	}

// without \n
#define LOG_EME_NON(M, ...) \
	if (log_level >= LEVEL_EMERG) \
		LOG_EME_UNFILTERED(M, ##__VA_ARGS__);
#define LOG_ALE_NON(M, ...) \
	if (log_level >= LEVEL_ALERT) \
		LOG_ALE_UNFILTERED(M, ##__VA_ARGS__);
#define LOG_CRI_NON(M, ...) \
	if (log_level >= LEVEL_CRIT)  \
		LOG_CRI_UNFILTERED(M, ##__VA_ARGS__);
#define LOG_ERR_NON(M, ...) \
	if (log_level >= LEVEL_ERR)   \
		LOG_ERR_UNFILTERED(M, ##__VA_ARGS__);
#define LOG_WAR_NON(M, ...)   \
	if (log_level >= LEVEL_WARNING) \
		LOG_WAR_UNFILTERED(M, ##__VA_ARGS__);
#define LOG_NOT_NON(M, ...)  \
	if (log_level >= LEVEL_NOTICE) \
		LOG_NOT_UNFILTERED(M, ##__VA_ARGS__);
#define LOG_INF_NON(M, ...) \
	if (log_level >= LEVEL_INFO)  \
		LOG_INF_UNFILTERED(M, ##__VA_ARGS__);
#define LOG_DEB_NON(M, ...) \
	if (log_level >= LEVEL_DEBUG) \
		LOG_DEB_UNFILTERED(M, ##__VA_ARGS__);
#define LOG_HEX_NON(D, N, M, ...) \
	if (log_level >= LEVEL_DEBUG)       \
		LOG_HEX_UNFILTERED(D, N, M, ##__VA_ARGS__);

// add \n
#define LOG_EME(M, ...)            \
	LOG_EME_NON(M, ##__VA_ARGS__); \
	stdio_printf("\n");
#define LOG_ALE(M, ...)            \
	LOG_ALE_NON(M, ##__VA_ARGS__); \
	stdio_printf("\n");
#define LOG_CRI(M, ...)            \
	LOG_CRI_NON(M, ##__VA_ARGS__); \
	stdio_printf("\n");
#define LOG_ERR(M, ...)            \
	LOG_ERR_NON(M, ##__VA_ARGS__); \
	stdio_printf("\n");
#define LOG_WAR(M, ...)            \
	LOG_WAR_NON(M, ##__VA_ARGS__); \
	stdio_printf("\n");
#define LOG_NOT(M, ...)            \
	LOG_NOT_NON(M, ##__VA_ARGS__); \
	stdio_printf("\n");
#define LOG_INF(M, ...)            \
	LOG_INF_NON(M, ##__VA_ARGS__); \
	stdio_printf("\n");
#define LOG_DEB(M, ...)            \
	LOG_DEB_NON(M, ##__VA_ARGS__); \
	stdio_printf("\n");
#define LOG_HEX(D, N, M, ...)            \
	LOG_HEX_NON(D, N, M, ##__VA_ARGS__); \
	stdio_printf("\n");

// undefine levels according to configuration
#if LOG_LEVEL < LEVEL_DEBUG
#undef LOG_DEB_NON
#define LOG_DEB_NON(M, ...) ((void)0)
#undef LOG_DEB
#define LOG_DEB(M, ...) ((void)0)
#undef LOG_HEX_NON
#define LOG_HEX_NON(D, N, M, ...) ((void)0)
#undef LOG_HEX
#define LOG_HEX(D, N, M, ...) ((void)0)
#endif

#if LOG_LEVEL < LEVEL_INFO
#undef LOG_INF_NON
#define LOG_INF_NON(M, ...) ((void)0)
#undef LOG_INF
#define LOG_INF(M, ...) ((void)0)
#endif

#if LOG_LEVEL < LEVEL_NOTICE
#undef LOG_NOT_NON
#define LOG_NOT_NON(M, ...) ((void)0)
#undef LOG_NOT
#define LOG_NOT(M, ...) ((void)0)
#endif

#if LOG_LEVEL < LEVEL_WARNING
#undef LOG_WARING_NON
#define LOG_WARING_NON(M, ...) ((void)0)
#undef LOG_WARING
#define LOG_WARING(M, ...) ((void)0)
#endif

#if LOG_LEVEL < LEVEL_ERR
#undef LOG_ERR_NON
#define LOG_ERR_NON(M, ...) ((void)0)
#undef LOG_ERR
#define LOG_ERR(M, ...) ((void)0)
#endif

#if LOG_LEVEL < LEVEL_CRIT
#undef LOG_CRI_NON
#define LOG_CRI_NON(M, ...) ((void)0)
#undef LOG_CRI
#define LOG_CRI(M, ...) ((void)0)
#endif

#if LOG_LEVEL < LEVEL_ALERT
#undef LOG_ALE_NON
#define LOG_ALE_NON(M, ...) ((void)0)
#undef LOG_ALE
#define LOG_ALE(M, ...) ((void)0)
#endif

#if LOG_LEVEL < LEVEL_EMERG
#undef LOG_EME_NON
#define LOG_EME_NON(M, ...) ((void)0)
#undef LOG_EME
#define LOG_EME(M, ...) ((void)0)
#endif

// undefine colors according to configuration
#if LOG_COLOR < 1

#undef NONE
#define NONE

#undef RED
#define RED

#undef PURPLE
#define PURPLE

#undef YELLOW
#define YELLOW

#undef BROWN
#define BROWN

#undef GREEN
#define GREEN

#undef CYAN
#define CYAN

#undef BLUE
#define BLUE

#undef GRAY
#define GRAY

#endif

#endif
