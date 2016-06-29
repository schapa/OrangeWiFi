/*
 * MsgQueue.hpp
 *
 *  Created on: Jun 8, 2016
 *      Author: shapa
 */

#ifndef MSGQUEUE_HPP_
#define MSGQUEUE_HPP_

#include <queue>
#include <string>
#include <pthread.h>

class MessageQueue {
public:
	MessageQueue();

public:
	void push(std::string);
//	blocking call
	std::string pendMsg();
//	non-blocking call
	std::string popMsg();

private:
    std::queue<std::string> queue;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};


#endif /* MSGQUEUE_HPP_ */
