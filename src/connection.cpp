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
    : client_addr(client_addr), client_addr_length(client_addr_length), client_socket(client_socket), server_socket(-1), status(OK),
      client_request(REQUEST), response(RESPONSE)
{
    log_raw("\n\n\n\n");
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

    log("Recebendo resposta do servidor");

    receiveMessage(server_socket, response, status);
}


void Connection::sendResponse()
{
    log("Enviando resposta para o cliente");

    std::vector<char> message = response.getMessage();

    send_buffer(client_socket, (unsigned char *) &message[0], message.size());
}


void Connection::sendError(ConnectionStatus status)
{   
    std::string message;


    if (status == INVALID_TERM)
    {   
        log("Encaminhamento nao autorizado devido a termo invalido encontrado na mensagem HTTP.");

        message = "HTTP/1.1 400 Bad Request\r\n"
                  "Connection: close\r\n"
                  "Content-type: text/html\r\n"
                  "\r\n"
                  "<html>\r\n"
                  "<head>\r\n"
                  "<title> ERROR </title>\r\n"
                  "</head>\r\n"
                  "<body>\r\n"
                  "<img src=\"https://media.giphy.com/media/njYrp176NQsHS/giphy.gif\" alt=\"gif\">"
                  "<h1> INVALID TERM FOUND </h1>\r\n"
                  "</body>\r\n"
                 "</html>\r\n\r\n";
    }
    else if (status == URL_BLOCKED) 
    {  
        log("Encaminhamento nao autorizado devido a URL bloqueada.");

        message = "HTTP/1.1 400 Bad Request\r\n"
                  "Connection: close\r\n"
                  "Content-type: text/html\r\n"
                  "\r\n"
                  "<html>\r\n"
                  "<head>\r\n"
                  "<title> ERROR </title>\r\n"
                  "</head>\r\n"
                  "<body>\r\n"
                  "<img src=\"https://media.giphy.com/media/njYrp176NQsHS/giphy.gif\" alt=\"gif\">"
                  "<h1> URL BLOCKED </h1>\r\n"
                  "</body>\r\n"
                  "</html>\r\n\r\n";
    }
    

    send_buffer(client_socket, (unsigned char *) &message[0], message.size());
}
