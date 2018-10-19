/*!
 * \file server.h
 * \author Gustavo Henrique Fernandes Carvalho
 */

#ifndef SERVER_H
#define SERVER_H

#include <string>
#include "HTTPMessage.h"

void runProxyServer(bool enable_gui);
int connectToHost(const std::string& host, ConnectionStatus& status);
int send_buffer(int socketfd, const unsigned char* buffer, const uint n_bytes);
void receiveMessage(int socket, HTTPMessage& message, ConnectionStatus& status);

#endif  // SERVER_H