/*!
 * \file connection.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "connection.h"
#include "HTTP.h"
#include "debug.h"

/*!
 * \brief Initialize a connection object with a new client request.
 * IMPORTANT: This method blocks the execution until receive a new request.
 * 
 * \param [in] server_socket File descriptor of the server socket.
 */
Connection::Connection(int server_socket)
    : client_request(nullptr), status(OK)
{
    PRINT_DEBUG("%s: Waiting for new connections\n", __PRETTY_FUNCTION__);

    client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_length);
    if (client_socket == -1)
    {
        int errsv = errno;
        perror("accept");
        // TODO: Call logger
        if (errsv != EINTR)
        exit(EXIT_FAILURE);
    }
}


Connection::~Connection()
{
    delete client_request;

    if (client_socket != -1)
        close(client_socket);
}


void Connection::receiveRequest()
{
    PRINT_DEBUG("%s: Receiving new request\n", __PRETTY_FUNCTION__);

    client_request = new HTTPRequest(receiveHTTPMessage(client_socket, status));
}
