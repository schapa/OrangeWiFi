/*
 * bsp.cpp
 *
 *  Created on: Jun 8, 2016
 *      Author: shapa
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "bsp.hpp"
#include "wpa_ctrl.h"
#include "dbg_base.h"
#if 01
#include "dbg_trace.h"
#endif

static const char *s_sysEth0 = "/sys/class/net/eth0/operstate";
static const char *s_sysWlan0 = "/sys/class/net/wlan0/operstate";
static const char *s_sysLedRed = "/sys/class/gpio_sw/normal_led/data";
static const char *s_sysLedGreen = "/sys/class/gpio_sw/standby_led/data";

BoardSupportPackage &BSP = BoardSupportPackage::getInstance();

static bool getNetworkState(int fd);

bool BoardSupportPackage::init() {
	bool result = false;
	do {
		clock_gettime(CLOCK_REALTIME, &startTime);
		fdEth = open(s_sysEth0, O_RDONLY);
		fdWlan = open(s_sysWlan0, O_RDONLY);
		fdLedRed = open(s_sysLedRed, O_RDWR);
		fdLedGreen = open(s_sysLedGreen, O_RDWR);

		if ((fdEth == -1) || (fdWlan == -1)) {
			DBGMSG_ERR("Failed to open NetDev: %d:%d", fdEth, fdWlan);
			break;
		}
		if ((fdLedRed == -1) || (fdLedGreen == -1)) {
			DBGMSG_ERR("Failed to open LedDev: %d:%d", fdLedRed, fdLedGreen);
			break;
		}
		setLedStateRed(false);
		setLedStateGreen(false);
		result = true;
	} while (0);
	DBGMSG_M("Init result %s", result ? "Ok" : "Error");
	return result;
}

bool BoardSupportPackage::getEthState() {

	bool result = getNetworkState(fdEth);

	if (result != isEthStateOk) {
		isEthStateOk = result;
		DBGMSG_H("Send Evt about eth state change");
	}

	return result;
}

bool BoardSupportPackage::getWlanState() {

	bool result = getNetworkState(fdWlan);

	if (result != isWlanStateOk) {
		isWlanStateOk = result;
		DBGMSG_H("Send evt about Wlan state change");
	}

	return result;
}

int BoardSupportPackage::getUptime() {

	struct timespec current;
	clock_gettime(CLOCK_REALTIME, &current);
	return current.tv_sec - startTime.tv_sec;
}

int BoardSupportPackage::getUptimeMs() {
	struct timespec current;
	clock_gettime(CLOCK_REALTIME, &current);
	return abs(current.tv_nsec - startTime.tv_nsec)/1000/1000;
}

void BoardSupportPackage::setLedStateRed(const bool& val) {
	const char *buff = val ? "1" : "0";
	const int len = strlen(buff);
	if (len != write(fdLedRed, buff, len))
		DBGMSG_ERR("Failed to set state %d", val);
	else
		DBGMSG_M("LED is %s", val ? "On" : "Off");

}

void BoardSupportPackage::setLedStateGreen(const bool& val) {
	const char *buff = val ? "1" : "0";
	const int len = strlen(buff);
	if (len != write(fdLedGreen, buff, len))
		DBGMSG_ERR("Failed to set state %d", val);
	else
		DBGMSG_M("LED is %s", val ? "On" : "Off");
}

void BoardSupportPackage::connectToAP(const char *ssid, const char *psk) {

}

static bool getNetworkState(int fd) {
	static const char *stateOk = "up";
	bool result = false;
	do {
		char textState[128];
		int res = lseek(fd, 0L, SEEK_SET);
		if (res) {
			DBGMSG_ERR("Seek failed %d", res);
			break;
		}
		res = read(fd, textState, sizeof(textState));
		if (!res) {
			DBGMSG_ERR("Read failed %d", res);
			break;
		}
		result = !strncmp(textState, stateOk, strlen(stateOk));
	} while (false);

	return result;
}

extern "C" int System_getUptime(void) {
	return BSP.getUptime();
}
extern "C" int System_getUptimeMs(void) {
	return BSP.getUptimeMs();
}
