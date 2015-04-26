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

#include <stdio.h>
#include <stdlib.h>

#define LOG(level, ...) do {\
	if (level <= loglevel) {\
		fprintf(logstream, "[%s: %dline] =>> ", __FILE__, __LINE__);\
		fprintf(logstream, __VA_ARGS__);\
		fprintf(logstream, "\n");\
		fflush(logstream);\
	}\
	if (level == LOG_FATAL) {\
		exit(-1);\
	}\
} while(0)

extern FILE *logstream;
extern int loglevel;

#endif
