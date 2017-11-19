/*!
 * \file HTTP.h
 * \author Gustavo Henrique Fernandes Carvalho
 */

#ifndef HTTP_H
#define HTTP_H

#include <string>
#include "connection.h"

std::string receiveHTTPMessage(int sockfd, ConnectionStatus& status);
bool receiveWholeHTTPMessage(const std::string& message);
void sendHTTPMessage(const std::string& host, std::string message, ConnectionStatus& status);

#endif // HTTP_H