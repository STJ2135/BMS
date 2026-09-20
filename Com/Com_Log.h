/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016-11-12     Bernard      The first version
 * 2018-05-25     armink       Add simple API, such as LOG_D, LOG_E
 */

/*
 * The macro definitions for debug
 *
 * These macros are defined in static. If you want to use debug macro, you can
 * use as following code:
 *
 * In your C/C++ file, enable/disable DEBUG_ENABLE macro, and then include this
 * header file.
 *
 * #define DBG_TAG           "MOD_TAG"
 * #define DBG_LVL           DBG_INFO
 * #include "Com_Log.h"          // must after of DBG_LVL, DBG_TAG or other options
 *
 * Then in your C/C++ file, you can use LOG_X macro to print out logs:
 * LOG_D("this is a debug log!");
 * LOG_E("this is a error log!");
 */

#ifndef __COM_LOG_H__
#define __COM_LOG_H__

#include <rtthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RT_DEBUG
#define RT_DEBUG_COLOR



#define PRINTF_REDEFINE		rt_kprintf


/* the debug log will force enable when RT_DEBUG macro is defined */
#if defined(RT_DEBUG) && !defined(DBG_ENABLE)
#define DBG_ENABLE
#endif

/* it will force output color log when RT_DEBUG_COLOR macro is defined */
#if defined(RT_DEBUG_COLOR) && !defined(DBG_COLOR)
#define DBG_COLOR
#endif

#if defined(RT_USING_ULOG)
/* using ulog compatible with rtdbg  */
#include <ulog.h>
#else

/* DEBUG level */
#define DBG_NONE	        0
#define DBG_ERROR           1
#define DBG_WARNING         2
#define DBG_INFO            3
#define DBG_LOG             4

#ifdef DBG_TAG
#ifndef DBG_SECTION_NAME
#define DBG_SECTION_NAME    DBG_TAG
#endif
#else
/* compatible with old version */
#ifndef DBG_SECTION_NAME
#define DBG_SECTION_NAME    "DBG"
#endif
#endif /* DBG_TAG */

#ifdef DBG_ENABLE

#ifdef DBG_LVL
#ifndef DBG_LEVEL
#define DBG_LEVEL         DBG_LVL
#endif
#else
/* compatible with old version */
#ifndef DBG_LEVEL
#define DBG_LEVEL         DBG_WARNING
#endif
#endif /* DBG_LVL */

/*
 * The color for terminal (foreground)
 * BLACK    30
 * RED      31
 * GREEN    32
 * YELLOW   33
 * BLUE     34
 * PURPLE   35
 * CYAN     36
 * WHITE    37
 */
#ifdef DBG_COLOR
#define _DBG_COLOR(n)        PRINTF_REDEFINE("\033["#n"m")
#define _DBG_LOG_HDR(lvl_name, color_n)                    \
	PRINTF_REDEFINE("\033["#color_n"m[" lvl_name "/" DBG_SECTION_NAME "] ")
#define _DBG_LOG_X_END                                     \
	PRINTF_REDEFINE("\033[0m\r\n")
#else
#define _DBG_COLOR(n)
#define _DBG_LOG_HDR(lvl_name, color_n)                    \
	PRINTF_REDEFINE("[" lvl_name "/" DBG_SECTION_NAME "] ")
#define _DBG_LOG_X_END                                     \
	PRINTF_REDEFINE("\r\n")
#endif /* DBG_COLOR */

/*
 * static debug routine
 * NOTE: This is a NOT RECOMMENDED API. Please using LOG_X API.
 *       It will be DISCARDED later. Because it will take up more resources.
 */
#define DBG_PRINT(level, fmt, ...)                            \
	if ((level) <= DBG_LEVEL)                               \
	{                                                       \
		switch(level)                                       \
		{                                                   \
			case DBG_ERROR:   _DBG_LOG_HDR("E", 31); break; \
			case DBG_WARNING: _DBG_LOG_HDR("W", 33); break; \
			case DBG_INFO:    _DBG_LOG_HDR("I", 32); break; \
			case DBG_LOG:     _DBG_LOG_HDR("D", 0); break;  \
			default: break;                                 \
		}                                                   \
		PRINTF_REDEFINE(fmt, ##__VA_ARGS__);                \
		_DBG_COLOR(0);                                      \
	}

#define DBG_HERE                                            \
	if ((DBG_LEVEL) <= DBG_LOG){                            \
		PRINTF_REDEFINE(DBG_SECTION_NAME " Here %s:%d\n",   \
			__FUNCTION__, __LINE__);                        \
	}

#define DBG_LOG_LINE(lvl, color_n, fmt, ...)                \
	do                                                      \
	{                                                       \
		_DBG_LOG_HDR(lvl, color_n);                         \
		PRINTF_REDEFINE(fmt, ##__VA_ARGS__);                \
		_DBG_LOG_X_END;                                     \
	}                                                       \
	while (0)

#define DBG_RAW(...)         printf(__VA_ARGS__);

#else
#define DBG_PRINT(level, fmt, ...)
#define DBG_HERE
#define DBG_ENTER
#define DBG_EXIT
#define DBG_LOG_LINE(lvl, color_n, fmt, ...)
#define DBG_RAW(...)
#endif /* DBG_ENABLE */

#if (DBG_LEVEL >= DBG_LOG)
#define LOG_D(fmt, ...)      DBG_LOG_LINE("D", 0, fmt, ##__VA_ARGS__)
#else
#define LOG_D(...)
#endif

#if (DBG_LEVEL >= DBG_INFO)
#define LOG_I(fmt, ...)      DBG_LOG_LINE("I", 32, fmt, ##__VA_ARGS__)
#else
#define LOG_I(...)
#endif

#if (DBG_LEVEL >= DBG_WARNING)
#define LOG_W(fmt, ...)      DBG_LOG_LINE("W", 33, fmt, ##__VA_ARGS__)
#else
#define LOG_W(...)
#endif

#if (DBG_LEVEL >= DBG_ERROR)
#define LOG_E(fmt, ...)      DBG_LOG_LINE("E", 31, fmt, ##__VA_ARGS__)
#else
#define LOG_E(...)
#endif

#define LOG_RAW(...)         DBG_RAW(__VA_ARGS__)

#define LOG_HEX(name, width, buf, size)

#endif /* defined(RT_USING_ULOG) && define(DBG_ENABLE) */

#ifdef __cplusplus
}
#endif

#endif /* __COM_LOG_H__ */

