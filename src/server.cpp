/*!
 * \file main.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <csignal>
#include <iostream>

#include "connection.h"
#include "debug.h"

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


// Functions prototype
static void SIGINTHandler(int signum);
static int initializeServerSocket(struct sockaddr_in& serv_addr);


/*!
 * \brief Initiates and runs the proxy server until receive a SIGINT.
 */
void runProxyServer()
{
    int server_socket;
    struct sockaddr_in serv_addr;

    // Register a signal handler to SIGINT
    struct sigaction sa;
    sa.sa_handler = SIGINTHandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    server_socket = initializeServerSocket(serv_addr);

    while (!SIGINT_received)
    {
        // Await for new connections
        Connection connection(server_socket);

        connection.receiveRequest();
        if (connection.status != OK)
        {
            // TODO: connection.sendError()
            continue;
        }     

        // std::cout << connection.getRequest() << std::endl;

        // TODO: filter.request(&connection)
        // if (connection.status != OK)
        // {
            // TODO: connection.sendError()
            // continue;
        // }

        // TODO: cache.getFile(&connection)
        // if (connection.status != OK)
        // {
            // TODO: connection.sendError()
            // continue;
        // }

        // TODO: filter.response(&connection)
        // if (connection.status != OK)
        // {
            // TODO: connection.sendError()
            // continue;
        // }

        // TODO: connection.sendResponse();
    }

    close(server_socket);
}


/*!
 * \brief Handles the SIGINT, setting the flag SIGINT_received to true so the
 *        program can terminate normally.
 */
void SIGINTHandler(int signum)
{
    SIGINT_received = true;
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