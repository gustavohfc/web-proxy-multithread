/*!
 * \file server.h
 * \author Gustavo Henrique Fernandes Carvalho
 */

#ifndef SERVER_H
#define SERVER_H

#include <string>
#include "connection.h"

void runProxyServer();
int connectToHost(const std::string& host, ConnectionStatus& status);

#endif // SERVER_H