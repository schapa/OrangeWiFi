/*
 * WiFiNode.cpp
 *
 *  Created on: Jun 29, 2016
 *      Author: shapa
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "WiFiNode.hpp"
#include "dbg_base.h"
#if 01
#include "dbg_trace.h"
#endif

bool WiFiNode::open() {
	bool result = false;
	do {
		if (!ctrlIfaceName) {
			DBGMSG_ERR("ctrlIfaceName is [%p]", ctrlIfaceName);
			break;
		}
		const size_t wpaPathLength = strlen(ctrlIfaceBasePath) + strlen(ctrlIfaceName) + 1;
		char path[wpaPathLength];
		snprintf(path, wpaPathLength, "%s%s", ctrlIfaceBasePath, ctrlIfaceName);

		DBGMSG_M("Trying to open [%s]", path);
		wpaCtrl = wpa_ctrl_open(path);

		if (!wpaCtrl) {
			DBGMSG_ERR("Failed to Open Interface");
			break;
		}
		if (wpa_ctrl_attach(wpaCtrl)) {
			DBGMSG_ERR("Failed to Attach To Interface %d");
			break;
		}
		result = true;
	} while (0);
	DBGMSG_M("Result %s", result ? "Ok" : "Error");
	return result;
}

void WiFiNode::close() {
	if (wpaCtrl) {
		DBGMSG_M("Closing [%p]", wpaCtrl);
		wpa_ctrl_close(wpaCtrl);
		wpaCtrl = nullptr;
	}
}

bool WiFiNode::scan() {
	if (!wpaCtrl) {
		DBGMSG_ERR("iface is closed");
		return false;
	}
	char reply[1024] = {0};
	size_t replyLen = sizeof(reply) - 1;

	int res = makeRequest("SCAN", reply, &replyLen);
	if (res || !strstr(reply, "OK")) {
		DBGMSG_ERR("Failed to start scanning [%d]", res);
		return false;
	}
	if (waitFor(WPA_EVENT_SCAN_RESULTS, 5000)) {
		DBGMSG_ERR("scanning failed");
		return false;
	}

	replyLen = sizeof(reply) - 1;
	res = makeRequest("SCAN_RESULTS", reply, &replyLen);
	if (res) {
		DBGMSG_ERR("Failed to get scanning results [%d]", res);
		return false;
	}

//	TODO: parse scan result here

	return true;
}

void WiFiNode::removeAllNetworks() {
	if (!wpaCtrl) {
		DBGMSG_ERR("iface is closed");
		return;
	}
	char reply[1024] = {0};
	size_t replyLen = sizeof(reply) - 1;
	int reqRes = makeRequest("LIST_NETWORKS", reply, &replyLen);
	if (reqRes) {
		DBGMSG_ERR("Failed to list networks [%d]", reqRes);
	} else {
		int linesCount = 0;
		char *ptr = reply;
		bool active = strstr(reply, "CURRENT");
		while (ptr && (ptr = strstr(ptr+1, "\n"))) {
			linesCount++;
		}
		DBGMSG_M("Found [%d] networks", linesCount-1);
		if (linesCount > 1) {
			reqRes = makeRequest("REMOVE_NETWORK ALL", reply, &replyLen);
			if (active) {
				waitFor(WPA_EVENT_DISCONNECTED, 1000);
			}
		}
	}
}

bool WiFiNode::conect(const char *ssid, const char *psk) {
	if (!wpaCtrl) {
		DBGMSG_ERR("iface is closed");
		return false;
	}
	if (!ssid || !psk) {
		DBGMSG_ERR("Credential is Null");
		return false;
	}
//	removeAllNetworks();
	char reply[1024] = {0};
	size_t replyLen = sizeof(reply) - 1;
	int reqRes = makeRequest("ADD_NETWORK", reply, &replyLen);
	if (reqRes) {
		DBGMSG_ERR("Failed to add network [%d]", reqRes);
		return false;
	}
	char *tmp;
	int networkId = strtol(reply, &tmp, 10);
	DBGMSG_M("New Network id is [%d]", networkId);

	char requestBuffer[1024];
	const size_t requestBufferSize = sizeof(requestBuffer) - 1;

	snprintf(requestBuffer, requestBufferSize, "SET_NETWORK %d ssid \"%s\"", networkId, ssid);
	reqRes = makeRequest(requestBuffer, reply, &replyLen);
	if (reqRes || !strstr(reply, "OK")) {
		DBGMSG_ERR("Failed to set ssid [%d]", reqRes);
		return false;
	}

	snprintf(requestBuffer, requestBufferSize, "SET_NETWORK %d psk \"%s\"", networkId, psk);
	reqRes = makeRequest(requestBuffer, reply, &replyLen);
	if (reqRes || !strstr(reply, "OK")) {
		DBGMSG_ERR("Failed to set psk [%d]", reqRes);
		return false;
	}

	snprintf(requestBuffer, requestBufferSize, "ENABLE_NETWORK %d", networkId);
	reqRes = makeRequest(requestBuffer, reply, &replyLen);
	if (reqRes || !strstr(reply, "OK")) {
		DBGMSG_ERR("Failed to enable network %d [%d]", networkId, reqRes);
		return false;
	}

	if (waitFor(WPA_EVENT_CONNECTED, 15000)) {
		DBGMSG_ERR("Connection Failed");
		return false;
	}
	DBGMSG_H("Connected to <%s>", ssid);
	int obt = obtainIp();
	DBGMSG_H("obtain ip  <%d>", obt);

	return true;
}

int WiFiNode::makeRequest(const char *cmd, char *reply, size_t *replyLen) {
	if (!wpaCtrl || !cmd || !reply || !replyLen) {
		DBGMSG_ERR("Input is Null");
		return -1;
	}
	int res = wpa_ctrl_request(wpaCtrl, cmd, strlen(cmd), reply, replyLen, NULL);
	reply[*replyLen] = '\0';

	DBGMSG_M("Req [%s] reply [%d] Reply: \n%s", cmd, *replyLen, reply);
	if (res || !replyLen) {
		DBGMSG_ERR("Req failed");
	}
	return res;
}

int WiFiNode::waitFor(const char *waitStr, const int &timeoutMs) {
	int result = -1;
	int retry = timeoutMs ? timeoutMs : 1000;

	if (!wpaCtrl || !waitStr) {
		DBGMSG_ERR("Input is Null");
		return result;
	}
	char replyBuff[1024] = {0};
	const size_t replySize = sizeof(replyBuff) - 1;

	do {
		if (!wpa_ctrl_pending(wpaCtrl)) {
			usleep(1000);
		} else {
			size_t replyLen = replySize;
			int readRes = wpa_ctrl_recv(wpaCtrl, replyBuff, &replyLen);
			if (readRes) {
				DBGMSG_ERR("Read failed %d", readRes);
				return readRes;
			}
			replyBuff[replyLen] = '\0';
			DBGMSG_M("Read %d [%s]", replyLen, replyBuff);
			if (strstr(replyBuff, waitStr)) {
				DBGMSG_H("Compare Hit!");
				result = 0;
				DBGMSG_M("wpa_ctrl_pending %d", wpa_ctrl_pending(wpaCtrl));
				break;
			}
			DBGMSG_M("Continue waiting. Elapsed %d from %d", timeoutMs - retry, timeoutMs);
		}
	} while (retry--);
	if (!retry) {
		DBGMSG_M("Exit by Timeout");
	}
	return result;
}

int WiFiNode::obtainIp() {
	char buffer[1024] = {0};

	snprintf(buffer, sizeof(buffer), "dhclient %s", ctrlIfaceName);
	return system(buffer);
}
