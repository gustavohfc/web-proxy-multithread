/*!
 * \file cache.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include "cache.h"
#include "HTTP.h"
#include "server.h"
#include <unistd.h>

void Cache::getResponseMessage(Connection& connection)
{
    // TODO: Verify local cache before send the request to a external server

    int sockfd_host = connectToHost(connection.getClientRequest()->getHeaders().find("Host")->second, connection.status);
    sendHTTPMessage(sockfd_host, connection.getClientRequest()->toMessage(), connection.status);
    connection.response = new HTTPResponse(receiveHTTPMessage(sockfd_host, connection.status));

    close(sockfd_host);
}