/*
 * SystemStatus.cpp
 *
 *  Created on: Jun 8, 2016
 *      Author: shapa
 */

#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <stdio.h>
#include <iostream>

#include "SystemStatus.hpp"
#include "bsp.hpp"
#include "dbg_base.h"
#if 01
#include "dbg_trace.h"
#endif

#define TICKS_PER_SECOND 1000*1000

static struct {
	double activeTime;
	double passiveTime;
} s_timing[] = {
		[INFORM_INIT] = { 0.1*TICKS_PER_SECOND, 0.3*TICKS_PER_SECOND },
		[INFORM_PREHEAT] = { 1*TICKS_PER_SECOND, 0.25*TICKS_PER_SECOND },
		[INFORM_IDLE] = { 0.1*TICKS_PER_SECOND, TICKS_PER_SECOND },
		[INFORM_SLEEP] = { 0.05*TICKS_PER_SECOND, 2*TICKS_PER_SECOND},
		[INFORM_CONNECTION_LOST] = { 0.1*TICKS_PER_SECOND, 0.5*TICKS_PER_SECOND},
		[INFORM_ERROR] = { 0.05*TICKS_PER_SECOND, 0.05*TICKS_PER_SECOND},
};

static void *SystemLightThread (void *arg);

SystemStatus::~SystemStatus() {
	if (pid)
		pthread_cancel(pid);
}

int SystemStatus::run() {

	int result = pthread_create(&pid, NULL, SystemLightThread, static_cast<void*>(this));
    if (result) {
    	DBGMSG_ERR("Error - pthread_create() return code: %d", result);
    }
    return result;
}
void SystemStatus::setStatus(const SystemStatus_t& status) {
	DBGMSG_M("Set stat %d -> %d", this->status, status);
	this->status = status < INFORM_LAST ? status : INFORM_ERROR;
}
const SystemStatus_t& SystemStatus::getStatus() {
	DBGMSG_M("Status %d", this->status);
	return status;
}

const pthread_t& SystemStatus::getPid() {
	DBGMSG_M("PID %p", this->pid);
	return pid;
}

static void *SystemLightThread (void *arg) {
	(void)arg;

	SystemStatus *owner = static_cast<SystemStatus*>(arg);
	DBGMSG_H("Thread starting %p", owner->getPid());

	while (true) {
		BSP.setLedStateRed(true);
		usleep((int)s_timing[owner->getStatus()].activeTime);
		BSP.setLedStateRed(false);
		usleep((int)s_timing[owner->getStatus()].passiveTime);
	}
	return NULL;
}
