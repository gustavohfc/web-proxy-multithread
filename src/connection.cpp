/*!
 * \file connection.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "connection.h"
#include "server.h"
#include "log.h"


/*!
 * \brief Initialize a connection object with a new client request.
 * 
 * \param [in] server_socket File descriptor of the server socket.
 */
Connection::Connection(int client_socket, struct sockaddr_in client_addr, socklen_t client_addr_length)
    : client_addr(client_addr), client_addr_length(client_addr_length), client_socket(client_socket), status(OK),
      client_request(REQUEST), response(RESPONSE)
{
    log("Trantando nova requisicao de " + std::string(inet_ntoa(client_addr.sin_addr)) + ":" + std::to_string(ntohs(client_addr.sin_port)));
}


Connection::~Connection()
{
    log("Encerrando conexao com "+ std::string(inet_ntoa(client_addr.sin_addr)) + ":" + std::to_string(ntohs(client_addr.sin_port)));

    if (client_socket != -1)
        close(client_socket);

    if (server_socket != -1)
        close(server_socket);
}


void Connection::receiveRequest()
{
    client_request = HTTPMessage(REQUEST);

    log("Recebendo requisicao do cliente");

    receiveMessage(client_socket, client_request, status);
}


void Connection::receiveServerResponse()
{
    response = HTTPMessage(RESPONSE);

    log("Enviando resposta para o cliente");

    receiveMessage(server_socket, response, status);
}


void Connection::sendResponse()
{
    send_buffer(client_socket, (unsigned char *) response.getMessage(), response.getMessageLength());

    auto header_Connection = response.getHeaders().find("Connection");
    if (header_Connection != response.getHeaders().end() && header_Connection->second.compare("keep-alive"))
    {
        log("Mantendo conexao viva para outras requisicoes.");
        status = KEEP_ALIVE;
    }
}
