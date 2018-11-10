/*!
 * \file server.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <csignal>
#include <iostream>

#include "cache.h"
#include "connection.h"
#include "filter.h"
#include "log.h"

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
static void handleRequest(int client_socket, struct sockaddr_in client_addr, socklen_t client_addr_length, bool enable_gui);

/*!
 * \brief Initiates and runs the proxy server until receive a SIGINT.
 */
void runProxyServer(bool enable_gui) {
    int server_socket;
    struct sockaddr_in serv_addr;

    // Create a socket to accept requests
    server_socket = initializeServerSocket(serv_addr);

    while (!SIGINT_received) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_length = sizeof client_addr;

        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_length);
        if (client_socket < 0) {
            log("Accept() fail");
            continue;
        }

        handleRequest(client_socket, client_addr, client_addr_length, enable_gui);
    }

    close(server_socket);
}

/*!
 * \brief Initialize a TCP/IPv4 listener socket and bind it to the port defined by SERVER_PORT.
 * 
 * \param [out] serv_addr Struct filled with the new socket address information.
 * \return File descriptor of the initialized socket.
 */
int initializeServerSocket(struct sockaddr_in& serv_addr) {
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    // serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Create a IPv4 TCP socket
    int sockfd = socket(serv_addr.sin_family, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        // TODO: Call logger
        exit(EXIT_FAILURE);
    }

    // Make the socket reusable to make easier to execute the program multiple times
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        // TODO: Call logger
        exit(EXIT_FAILURE);
    }

    // Bind the socket with the port
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
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

/*!
 * \brief Try to connect to a external server.
 * 
 * \param [in] url Url of the external host.
 * \param [out] status Indicate the result of the connection.
 * 
 * \return Socket to communicate with the host.
 */
int connectToHost(const std::string& host, ConnectionStatus& status) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;      // Use IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;  // Use TCP

    char* host_name = strdup(host.c_str());
    // Remove the port number from the host name
    char* colon_char = strchr(host_name, ':');
    if (colon_char != NULL) {
        *colon_char = '\0';
    }

    if ((rv = getaddrinfo(host_name, "http", &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        status = CANNOT_CONNECT_TO_HOST;
        return -1;
    }

    // Loop through all the results and try to connect
    for (p = servinfo; p != NULL; p = p->ai_next) {
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

    free(host_name);
    freeaddrinfo(servinfo);

    return sockfd;
}

/*!
 * \brief Receive a HTTP message from the socket.
 * 
 * \param [in] socket Source of the message.
 * \param [out] message Received message.
 * \param [out] status Indicate the result.
 */
void receiveMessage(int socket, HTTPMessage& message, ConnectionStatus& status) {
    char* buffer = new char[BUFFER_SIZE];

    do {
        // Receive a piece of the message
        int n_bytes = recv(socket, buffer, BUFFER_SIZE - 1, 0);
        if (n_bytes == EAGAIN || n_bytes == EWOULDBLOCK) {
            log("recv timeout.");
            status = INVALID_REQUEST;
        }
        else if (n_bytes < 0) {
            perror("recv");
            status = INVALID_REQUEST;
        } else if (n_bytes == 0) {
            log("Conexao fechada antes de receber a mensagem.");
            status = INVALID_REQUEST;
        }

        if (status != OK)
            break;

        status = message.addMessageData(buffer, n_bytes);

    } while (!message.is_message_complete());

    delete[] buffer;
}

/*!
 * \brief Send a HTTP message to the socket.
 * 
 * \param [in] socketfd Destination of the message.
 * \param [in] buffer Continuous byte array with the message.
 * \param [in] n_bytes Number of bytes in the buffer.
 */
int send_buffer(int socketfd, const unsigned char* buffer, const uint n_bytes) {
    uint bytes_sent = 0;

    while (bytes_sent != n_bytes) {
        int rv = send(socketfd, buffer, n_bytes - bytes_sent, 0);
        if (rv < 0) {
            return -1;
        }

        bytes_sent += rv;
    }

    return 0;
}

/*!
 * \brief Handles a client request.
 * 
 * \param [in] client_socket Socket to communicate with the client.
 * \param [in] client_addr Informations of the client.
 * \param [in] client_addr_length Size of client_addr.
 * \param [in] enable_gui Indicates if the gui option is active.
 */
void handleRequest(int client_socket, struct sockaddr_in client_addr, socklen_t client_addr_length, bool enable_gui) {
    static Filter filter;
    // static UI ui;

    // Initialize new connection
    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    Connection connection(client_socket, client_addr, client_addr_length);

    // Receive the client request
    connection.receiveRequest();
    if (connection.status != OK) {
        connection.sendError();
        return;
    }
    connection.client_request.changeHeader("Connection", "close");

    // Open the inspector if it's enabled
    // if (enable_gui) {
    //     ui.handleConnection(&connection, REQUEST);
    //     if (connection.status != OK) {
    //         connection.sendError();
    //         return;
    //     }
    // }

    // Filter the request
    connection.status = filter.filterRequest(connection.client_request);
    if (connection.status != OK) {
        connection.sendError();
        return;
    }

    // Get the response message from the cache or from the external server
    getResponseMessage(connection);
    if (connection.status != OK) {
        connection.sendError();
        return;
    }
    connection.response.changeHeader("Connection", "close");

    // Open the inspector if it's enabled
    // if (enable_gui) {
    //     ui.handleConnection(&connection, RESPONSE);
    //     if (connection.status != OK) {
    //         connection.sendError();
    //         return;
    //     }
    // }

    // Filter the response
    connection.status = filter.filterResponse(connection.response, connection.client_request.getHost());
    if (connection.status != OK) {
        connection.sendError();
        return;
    }

    // Send the response to the client
    connection.sendResponse();
}