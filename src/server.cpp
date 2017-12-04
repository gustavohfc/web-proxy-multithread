/*!
 * \file main.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <iostream>

#include "log.h"
#include "connection.h"
#include "cache.h"
#include "filter.h"

/*!
 * \brief Server port to receive new connections.
 */
static const int SERVER_PORT = 12345;

/*!
 * \brief Number of connections allowed on the incoming queue.
 */
static const int LISTEN_BACKLOG_SIZE = 20;

/*!
 * \brief Indicates if a SIGINT was received.
 */
static bool SIGINT_received = false;


#define BUFFER_SIZE 10000


// Functions prototype
static int initializeServerSocket(struct sockaddr_in& serv_addr);
static void handleRequest(int client_socket, struct sockaddr_in client_addr, socklen_t client_addr_length);


/*!
 * \brief Initiates and runs the proxy server until receive a SIGINT.
 */
void runProxyServer()
{
    int server_socket;
    struct sockaddr_in serv_addr;

    // Create a socket to accept requests
    server_socket = initializeServerSocket(serv_addr);

    while (!SIGINT_received)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_length = sizeof client_addr;

        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_length);
        if (client_socket < 0)
        {
            log("Accept() fail");
            continue;
        }

        handleRequest(client_socket, client_addr, client_addr_length);
    }

    close(server_socket);
}


void HandleSignalParent(int signum)
{
    SIGINT_received = true;
}


/*!
 * \brief Specifies the action to be associated with a specific signal in the parent process.
 */
void setParentSigaction()
{
    // Register a signal handler to SIGINT
    struct sigaction sa;
    sa.sa_handler = HandleSignalParent;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
}


/*!
 * \brief Initialize a TCP/IPv4 listener socket and bind it to the port defined by SERVER_PORT.
 * 
 * \param [out] serv_addr Struct filled with the new socket address information.
 * \return File descriptor of the initialized socket.
 */
int initializeServerSocket(struct sockaddr_in& serv_addr)
{
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    // serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Create a IPv4 TCP socket
    int sockfd = socket(serv_addr.sin_family, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        // TODO: Call logger
        exit(EXIT_FAILURE);
    }

    // Make the socket reusable to make easier to execute the program multiple times
    int yes=1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        perror("setsockopt");
        // TODO: Call logger
        exit(EXIT_FAILURE);
    }

    // Bind the socket with the port
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
        close(sockfd);
        // TODO: Call logger
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Make the socket avaiable for new connections request
    if (listen(sockfd, LISTEN_BACKLOG_SIZE) == -1) {
        perror("listen");
        // TODO: Call logger
        exit(EXIT_FAILURE);
    }

    return sockfd;
}


int connectToHost(const std::string& host, ConnectionStatus& status)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // Use IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // Use TCP

    char* host_name = strdup(host.c_str());
    // Remove the port number from the host name
    char* colon_char = strchr(host_name, ':');
    if (colon_char != NULL)
    {
        *colon_char = '\0';
    }

    if ((rv = getaddrinfo(host_name, "http", &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        status = CANNOT_CONNECT_TO_HOST;
        return -1;
    }

    // Loop through all the results and try to connect
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        status = CANNOT_CONNECT_TO_HOST;
        return -1;
    }

    freeaddrinfo(servinfo);

    return sockfd;
}


void receiveMessage(int socket, HTTPMessage& message, ConnectionStatus& status)
{
    char *buffer = new char[BUFFER_SIZE];

    do
    {
        // Receive a piece of the message
        int n_bytes = recv(socket, buffer, BUFFER_SIZE - 1, 0);
        if (n_bytes < 0)
        {
            perror("recv");
            // TODO: Call logger
            status = INVALID_REQUEST;
            break;
        }
        else if (n_bytes == 0)
        {
            log("Conexao fechada antes de receber a mensagem.");
            // TODO: Call logger
            status = INVALID_REQUEST;
            break;
        }

        status = message.addMessageData(buffer, n_bytes);
        if (status != OK)
            break;

    } while (!message.is_message_complete());

    delete[] buffer;
}


int send_buffer(int socketfd, const unsigned char *buffer, const uint n_bytes)
{
    uint bytes_sent = 0;

    while (bytes_sent != n_bytes)
    {
        int rv = send(socketfd, buffer, n_bytes - bytes_sent, 0);
        if (rv < 0)
        {
            return -1;
        }

        bytes_sent += rv;
    }

    return 0;
}


void handleRequest(int client_socket, struct sockaddr_in client_addr, socklen_t client_addr_length)
{
    Connection connection(client_socket, client_addr, client_addr_length);


    connection.receiveRequest();
    if (connection.status != OK)
    {
        // TODO: connection.sendError()
        return;
    }

    Filter filter;
    connection.status = filter.filteringRequest(connection.client_request);
    if (connection.status != OK)
    {   
        connection.sendError(connection.status);
        return;
    }

    connection.client_request.addHeader("Connection", "close");
    getResponseMessage(connection);
    if (connection.status != OK)
    {
    //     // TODO: connection.sendError()
        return;
    }

    connection.status = filter.filteringResponse(connection.response);
    if (connection.status != OK)
    {
        connection.sendError(connection.status);
        return;
    }

    connection.response.addHeader("Connection", "close");
    connection.sendResponse();
}