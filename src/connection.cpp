/*!
 * \file connection.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#include "connection.h"

/*!
 * \brief Initialize a connection object with a new client request.
 * IMPORTANT: This method blocks the execution until receive a new request.
 * 
 * \param [in] server_socket File descriptor of the server socket.
 */
Connection::Connection(int server_socket)
    : status(OK)
{
    client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_length);
    if (client_socket == -1)
    {
        perror("accept");
        // TODO: Call logger
        exit(EXIT_FAILURE);
    }
}