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

#include "HTTPMessage.h"

enum ConnectionStatus {OK, INVALID_REQUEST, TIMEOUT, FILTER_BLOCKED, CANNOT_SEND_MESSAGE_TO_HOST, CANNOT_CONNECT_TO_HOST};

class Connection
{
private:
    struct sockaddr_in client_addr;
    socklen_t client_addr_length;


public:
    int client_socket, server_socket, logger_socket;
    ConnectionStatus status;
    HTTPMessage client_request;
    HTTPMessage server_request;
    HTTPMessage response;

    Connection(int client_socket, struct sockaddr_in client_addr, socklen_t client_addr_length);
    ~Connection();
    void receiveRequest();
};

#endif // CONNECTION_H