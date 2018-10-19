/*!
 * \file connection.h
 * \author Gustavo Henrique Fernandes Carvalho
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>

#include "HTTPMessage.h"

class Connection {
   private:
    const struct sockaddr_in client_addr;
    const socklen_t client_addr_length;

   public:
    int client_socket, server_socket;
    ConnectionStatus status;
    HTTPMessage client_request;
    HTTPMessage response;

    Connection(int client_socket, struct sockaddr_in client_addr, socklen_t client_addr_length);
    ~Connection();

    void receiveRequest();
    void receiveServerResponse();
    void sendResponse();
    void sendError();
};

#endif  // CONNECTION_H