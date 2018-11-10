/*!
 * \file server.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <list>

class RequestInfo {
   public:
    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_addr_length;

    RequestInfo(int client_socket, struct sockaddr_in client_addr, socklen_t client_addr_length)
        : client_socket(client_socket), client_addr(client_addr), client_addr_length(client_addr_length) {}
};

class RequestQueue {
   private:
    std::list<RequestInfo> queue;
    pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
    sem_t queueSem;

   public:
    RequestQueue();
    void addRequest(int client_socket, struct sockaddr_in client_addr, socklen_t client_addr_length);
    RequestInfo getRequest();
};