/*!
 * \file server.h
 * \author Gustavo Henrique Fernandes Carvalho
 */

#ifndef SERVER_H
#define SERVER_H

#include <string>

void runProxyServer();
// int connectToHost(const std::string& host, ConnectionStatus& status);
int send_buffer(int socketfd, const unsigned char *buffer, const uint n_bytes);

#endif // SERVER_H