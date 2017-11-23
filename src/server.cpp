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

#include "logger.h"
#include "connection.h"

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
static int initializeServerSocket(struct sockaddr_in& serv_addr);
static void HandleSignalParent(int signum);
static void setParentSigaction();
static void handleRequest(int client_socket, struct sockaddr_in client_addr, socklen_t client_addr_length);


/*!
 * \brief Initiates and runs the proxy server until receive a SIGINT.
 */
void runProxyServer()
{
    int server_socket, logger_socket;
    struct sockaddr_in serv_addr;
    // Cache cache;

    // Run a proxy server on a new process
    runLoggerServer();

    setParentSigaction();

    // Try to connect to the logger server, it try a few times because it may take a while to the
    // server start to accept connections
    for (int n_try = 0; n_try < 5; n_try++)
    {
        logger_socket = connectToLogger();

        if (logger_socket < 0)
        {
            usleep(500000);
        }
        else
        {
            break;
        }
    }

    if (logger_socket < 0)
    {
        exit(EXIT_FAILURE);
    }

    // Create a socket to accept requests
    server_socket = initializeServerSocket(serv_addr);

    while (!SIGINT_received)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_length = sizeof client_addr;

        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_length);
        if (client_socket < 0)
        {
            log(logger_socket, "Accept() fail");
            continue;
        }

        pid_t pid = fork();
        if (pid == -1)
        {
            close(client_socket);
            continue;
        }
        else if(pid == 0)
        {
            // setParentSigaction();
            handleRequest(client_socket, client_addr, client_addr_length);
            exit(EXIT_SUCCESS);
        }

        // Clear zombie process
        while(waitpid(-1, NULL, WNOHANG) > 0);

    }

    close(logger_socket);

    // Wait all child process
    waitpid(-1, NULL, 0);

    close(server_socket);
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


void HandleSignalParent(int signum)
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


// int connectToHost(const std::string& host, ConnectionStatus& status)
// {
//     int sockfd;
//     struct addrinfo hints, *servinfo, *p;
//     int rv;

//     memset(&hints, 0, sizeof hints);
//     hints.ai_family = AF_UNSPEC; // Use IPv4 or IPv6
//     hints.ai_socktype = SOCK_STREAM; // Use TCP

//     char* host_name = strdup(host.c_str());
//     // Remove the port number from the host name
//     char* colon_char = strchr(host_name, ':');
//     if (colon_char != NULL)
//     {
//         *colon_char = '\0';
//     }

//     PRINT_DEBUG("Connecting to %s ...", host_name);

//     if ((rv = getaddrinfo(host_name, "http", &hints, &servinfo)) != 0)
//     {
//         fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
//         status = CANNOT_CONNECT_TO_HOST;
//         return -1;
//     }

//     // Loop through all the results and try to connect
//     for(p = servinfo; p != NULL; p = p->ai_next) {
//         if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
//             perror("socket");
//             continue;
//         }

//         if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
//             close(sockfd);
//             perror("connect");
//             continue;
//         }

//         break;
//     }

//     if (p == NULL) {
//         fprintf(stderr, "client: failed to connect\n");
//         status = CANNOT_CONNECT_TO_HOST;
//         return -1;
//     }

//     // if (((struct sockaddr *) p->ai_addr)->sa_family == AF_INET)
//     // {
//     //     inet_ntop(p->ai_family, &(((struct sockaddr_in *)((struct sockaddr *) p->ai_addr))->sin_addr), s, sizeof s);
//     // }
//     // else
//     // {
//     //     inet_ntop(p->ai_family, &(((struct sockaddr_in6 *)((struct sockaddr *) p->ai_addr))->sin6_addr), s, sizeof s);
//     // }

//     freeaddrinfo(servinfo);

//     PRINT_DEBUG(" OK\n");

//     return sockfd;
// }


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

    // connection.receiveRequest();
    // if (connection.status != OK)
    // {
    //     // TODO: connection.sendError()
    //     continue;
    // }

    // // TODO: filter.request(connection)
    // // if (connection.status != OK)
    // // {
    //     // TODO: connection.sendError()
    //     // continue;
    // // }

    // cache.getResponseMessage(connection);
    // if (connection.status != OK)
    // {
    //     // TODO: connection.sendError()
    //     continue;
    // }

    // // TODO: filter.response(connection)
    // // if (connection.status != OK)
    // // {
    //     // TODO: connection.sendError()
    //     // continue;
    // // }

    // // TODO: connection.sendResponse();
}