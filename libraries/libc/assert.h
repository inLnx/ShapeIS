#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <sys/cdefs.h>

/**
 * @brief debug levels[trace, info, warning, error, fatal]
 * 
 */
enum debug_level {
	DEBUG_TRACE = 0,
	DEBUG_INFO = 1,
	DEBUG_WARNING = 2,
	DEBUG_ERROR = 3,
	DEBUG_FATAL = 4,
};

/**
 * @param level 
 * @param prefix 
 * @param file 
 * @param line 
 * @param func 
 */
void __dbg(enum debug_level level, bool prefix, const char *file, int line, const char *func, ...);

/**
 * @brief if debug enabled
 * 
 */
#ifndef NDEBUG /* start of ifndef */

#define log(...) __dbg(DEBUG_INFO, false, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define err(...) __dbg(DEBUG_ERROR, false, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define dlog(...) __dbg(DEBUG_INFO, true, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define derr(...) __dbg(DEBUG_ERROR, true, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define log(...) ((void)0)
#define err(...) ((void)0)
#define dlog(...) ((void)0)
#define derr(...) ((void)0)

#endif /* end of ifndef */

#define __with_fmt(func, default_fmt, ...) (PP_NARG(__VA_ARGS__) == 0 ? func(default_fmt) : func(__VA_ARGS__))
#define assert(expression, ...) ((expression)  \
									 ? (void)0 \
									 : (void)({ __with_fmt(dlog, "expression " #expression " is falsy", ##__VA_ARGS__); __asm__ __volatile("int $0x01"); }))
#define assert_not_reached(...) ({ __with_fmt(dlog, "should not be reached", ##__VA_ARGS__); __asm__ __volatile__("int $0x01"); })
#define assert_not_implemented(...) __with_fmt(dlog, "is not implemented", ##__VA_ARGS__)
