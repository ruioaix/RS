/**
 * This file may be included by any other *.c or *.h file.
 * it provide functions and preprocessor macros that could be used most commonly by  other functions.
 *
 * Author: RuiXiao <xrfind@gmail.com>
 */
#ifndef DEBUG_H
#define DEBUG_H

//#define NDEBUG

/****************VERBOSE LEVEL*******************************************************************/
#define LOG_FATAL    (1)
#define LOG_ERR      (2)
#define LOG_WARN     (3)
#define LOG_INFO     (4)
#define LOG_DBG      (5)

#define LOG(level, ...) do {\
	if (level <= debug_level) {\
		fprintf(dbgstream, "%s:%d", __FILE__, __LINE__);\
		fprintf(dbgstream, __VA_ARGS__);\
		fprintf(dbgstream, "\n");\
		fflush(dbgstream);\
	}\
} while(0)

extern FILE *dbgstream;
extern int debug_level;
