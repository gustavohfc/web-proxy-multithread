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

enum ConnectionStatus {OK, INVALID_REQUEST, TIMEOUT, FILTER_BLOCKED};

class Connection
{
private:
    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_addr_length;
    std::string request;


public:
    ConnectionStatus status;

    Connection(int server_socket);
    ~Connection();
    void receiveRequest();

    const std::string& getRequest() const { return request; };
};

#endif // CONNECTION_H