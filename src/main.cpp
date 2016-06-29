//============================================================================
// Name        : OrangeWiFi.cpp
// Author      : shapa
// Version     :
// Copyright   : Under LGPL license
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <pthread.h>

#include "bsp.hpp"
#include "SystemStatus.hpp"

using namespace std;

#include "wpa_ctrl.h"
#include <string.h>
#include "WiFiNode.hpp"

int main() {
	SystemStatus statusLed;
	WiFiNode wifi("wlan1");

	BSP.init();
	statusLed.run();

	wifi.open();
	wifi.scan();
	wifi.removeAllNetworks();
	wifi.conect("shapa", "jnghfdkzq");
//
//	while (true) {
//		printf("%s\n", __PRETTY_FUNCTION__);
//		statusLed.setStatus(INFORM_SLEEP);
//		BSP.getEthState();
//		sleep(1);
//	}
//
//    pthread_join(statusLed.getPid(), NULL);

	return 0;
}
