/*!
 * \file connection.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "connection.h"
// #include "HTTP.h"

/*!
 * \brief Initialize a connection object with a new client request.
 * 
 * \param [in] server_socket File descriptor of the server socket.
 */
Connection::Connection(int client_socket, struct sockaddr_in client_addr, socklen_t client_addr_length)
    : client_addr(client_addr), client_addr_length(client_addr_length), client_socket(client_socket), status(OK),
      client_request(REQUEST), server_request(REQUEST), response(RESPONSE)
{
}


Connection::~Connection()
{
    if (client_socket != -1)
        close(client_socket);

    if (server_socket != -1)
        close(server_socket);

    if (logger_socket != -1)
        close(logger_socket);
}


void Connection::receiveRequest()
{
    // client_request = new HTTPRequest(receiveHTTPMessage(client_socket, status));
}
