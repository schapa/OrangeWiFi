/*
 * dbg_trace.c
 *
 *  Created on: May 6, 2016
 *      Author: shapa
 */

#include "dbg_trace.h"
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

extern long System_getUptime(void);
extern long System_getUptimeMs(void);

void dbgmsg(const char *color, const char *siverity, const char *file, const char *func, int line, const char *fmt, ...) {

	char msgBuffer[4096];
	const int msgBufferSize = sizeof(msgBuffer);

	int occupied = snprintf(msgBuffer, msgBufferSize, "[%4lu.%03lu] %s::%s (%d)%s %s: ",
			System_getUptime(), System_getUptimeMs(), file, func, line, color, siverity);
	if (occupied < msgBufferSize) {
		va_list ap;
		va_start (ap, fmt);
		occupied += vsnprintf(&msgBuffer[occupied], msgBufferSize - occupied, fmt, ap);
		va_end (ap);
	}
	if (occupied < msgBufferSize) {
		occupied += snprintf(&msgBuffer[occupied], msgBufferSize - occupied, ANSI_ESC_DEFAULT"\r\n");
	}
	if (occupied > msgBufferSize) {
		char *trim = "...";
		size_t size = strlen(trim) + 1;
		snprintf(&msgBuffer[msgBufferSize-size], size, trim);
	}
	char *newBuff = (char*)malloc(occupied);
	if (newBuff) {
		memcpy((void*)newBuff, (void*)msgBuffer, occupied);
	}
    write(STDOUT_FILENO, newBuff, occupied);
}
