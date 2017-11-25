/*!
 * \file connection.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connection.h"
#include "server.h"


/*!
 * \brief Initialize a connection object with a new client request.
 * 
 * \param [in] server_socket File descriptor of the server socket.
 */
Connection::Connection(int client_socket, struct sockaddr_in client_addr, socklen_t client_addr_length)
    : client_addr(client_addr), client_addr_length(client_addr_length), client_socket(client_socket), status(OK),
      client_request(REQUEST), response(RESPONSE)
{
}


Connection::~Connection()
{
    if (client_socket != -1)
        close(client_socket);

    if (server_socket != -1)
        close(server_socket);
}


void Connection::receiveRequest()
{
    receiveMessage(client_socket, client_request, status);
}

void Connection::receiveServerResponse()
{
    receiveMessage(server_socket, response, status);
}


void Connection::sendResponse()
{
    send_buffer(client_socket, (unsigned char *) response.getMessage(), response.getMessageLength());
}
