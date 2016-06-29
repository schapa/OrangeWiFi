/*
 * bsp.hpp
 *
 *  Created on: Jun 8, 2016
 *      Author: shapa
 */

#ifndef BSP_HPP_
#define BSP_HPP_

#include <time.h>

class BoardSupportPackage {
private:
	BoardSupportPackage() : fdEth(0), fdWlan(0), fdLedRed(0), fdLedGreen(0), isEthStateOk(false), isWlanStateOk(false) {};
	BoardSupportPackage(const BoardSupportPackage&) : BoardSupportPackage() {};
	virtual ~BoardSupportPackage() {};

public:
	static BoardSupportPackage &getInstance() {
		static BoardSupportPackage bsp;
		return bsp;
	}
	bool init();
	bool getEthState();
	bool getWlanState();
	int getUptime();
	int getUptimeMs();

	void setLedStateRed(const bool&);
	void setLedStateGreen(const bool&);

	void connectToAP(const char *ssid, const char *psk);

private:
	int fdEth;
	int fdWlan;
	int fdLedRed;
	int fdLedGreen;
	bool isEthStateOk;
	bool isWlanStateOk;
	struct timespec startTime;
};

extern BoardSupportPackage &BSP;

#endif /* BSP_HPP_ */
