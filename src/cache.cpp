/*!
 * \file cache.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include "cache.h"
#include "HTTP.h"

void Cache::getResponseMessage(Connection& connection)
{
    // TODO: Verify local cache before send the request to a external server

    sendHTTPMessage(connection.getClientRequest()->getHeaders().find("Host")->second, connection.getClientRequest()->toMessage(), connection.status);
}