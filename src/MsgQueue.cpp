/*
 * MsgQueue.cpp
 *
 *  Created on: Jun 8, 2016
 *      Author: shapa
 */

#include "MsgQueue.hpp"


MessageQueue::MessageQueue() : mutex(PTHREAD_MUTEX_INITIALIZER), cond(PTHREAD_COND_INITIALIZER) {

}

void MessageQueue::push(std::string msg) {
    pthread_mutex_lock(&mutex);
    queue.push(msg);
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&cond);
}

std::string MessageQueue::pendMsg() {
	std::string msg;
	pthread_mutex_lock(&mutex);
	if (!queue.empty()) {
		msg = queue.front();
		queue.pop();
		pthread_mutex_unlock(&mutex);
	} else {
		pthread_cond_wait(&cond, &mutex);
		msg = pendMsg();
    }
	return msg;
}

std::string MessageQueue::popMsg() {
	std::string msg;
	pthread_mutex_lock(&mutex);
	if (!queue.empty()) {
		msg = queue.front();
		queue.pop();
	}
	pthread_mutex_unlock(&mutex);
	return msg;
}
