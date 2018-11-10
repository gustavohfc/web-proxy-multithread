/*!
 * \file server.h
 * \author Gustavo Henrique Fernandes Carvalho
 */

#ifndef SERVER_H
#define SERVER_H

#include <string>
#include "HTTPMessage.h"

void runProxyServer();
int connectToHost(const std::string& host, ConnectionStatus& status);
int initializeServerSocket(struct sockaddr_in& serv_addr);
int send_buffer(int socketfd, const unsigned char* buffer, const uint n_bytes);
void receiveMessage(int socket, HTTPMessage& message, ConnectionStatus& status);
void* workingThreadHandleRequest(void*);

#endif  // SERVER_H