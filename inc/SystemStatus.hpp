/*
 * SystemStatus.hpp
 *
 *  Created on: Jun 8, 2016
 *      Author: shapa
 */

#ifndef SYSTEMSTATUS_HPP_
#define SYSTEMSTATUS_HPP_

#include <pthread.h>

#include "MsgQueue.hpp"

typedef enum {
	INFORM_INIT,
	INFORM_PREHEAT,
	INFORM_IDLE,
	INFORM_SLEEP,
	INFORM_CONNECTION_LOST,
	INFORM_ERROR,
	INFORM_LAST
} SystemStatus_t;

class SystemStatus {

public:
	SystemStatus() : pid(0), status(INFORM_INIT) {};
	~SystemStatus();

public:
	int run();

	void setStatus(const SystemStatus_t& status);
	const SystemStatus_t& getStatus();

	const pthread_t & getPid();
private:
	pthread_t pid;
	SystemStatus_t status;
};



#endif /* SYSTEMSTATUS_HPP_ */
