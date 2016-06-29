/*
 * WiFiNode.hpp
 *
 *  Created on: Jun 29, 2016
 *      Author: shapa
 */

#ifndef WIFINODE_HPP_
#define WIFINODE_HPP_

#include <string.h>
#include "wpa_ctrl.h"

class WiFiNode {
public:
	WiFiNode() : ctrlIfaceName(nullptr), wpaCtrl(nullptr) {};
	WiFiNode(const char *name) : ctrlIfaceName(strdup(name)), wpaCtrl(nullptr) {};
	virtual ~WiFiNode() { close(); delete ctrlIfaceName; };

public:
	bool open();
	void close();
	bool scan();
	void removeAllNetworks();
	bool conect(const char *ssid, const char *psk);

private:
	int makeRequest(const char *cmd, char *reply, size_t *replyLen);
	int waitFor(const char *waitStr, const int &timeoutMs);
	int obtainIp();

private:
	char *ctrlIfaceName;
	const char *ctrlIfaceBasePath = "/var/run/wpa_supplicant/";

	struct wpa_ctrl *wpaCtrl;
};




#endif /* WIFINODE_HPP_ */
