/*!
 * \file server.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include "RequestQueue.h"

RequestQueue::RequestQueue() {
    sem_init(&queueSem, 0, 0);
}

void RequestQueue::addRequest(int client_socket, struct sockaddr_in client_addr, socklen_t client_addr_length) {
    auto request = RequestInfo(client_socket, client_addr, client_addr_length);

    pthread_mutex_lock(&queueMutex);
    queue.push_back(request);
    pthread_mutex_unlock(&queueMutex);
    sem_post(&queueSem);
}

RequestInfo RequestQueue::getRequest() {

    sem_wait(&queueSem);
    pthread_mutex_lock(&queueMutex);

    auto request = queue.front();
    queue.pop_front();

    pthread_mutex_unlock(&queueMutex);

    return request;
}