/*!
 * \file connection.h
 * \author Gustavo Henrique Fernandes Carvalho
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string>
#include "HTTP-request.h"

enum ConnectionStatus {OK, INVALID_REQUEST, TIMEOUT, FILTER_BLOCKED, CANNOT_SEND_MESSAGE};

class Connection
{
private:
    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_addr_length;
    HTTPRequest* client_request;


public:
    ConnectionStatus status;

    Connection(int server_socket);
    ~Connection();
    void receiveRequest();

    const HTTPRequest* getClientRequest() const { return client_request; };
};

#endif // CONNECTION_H