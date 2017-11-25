/*!
 * \file logger_server.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <vector>
#include <iostream>
#include <fstream>

#include "logger_server.h"
#include "server.h"

/*!
 * \brief Indicates if a SIGINT was received.
 */
static bool SIGINT_received = false;

// Functions prototype
int createLoggerServerSocket();
void setLoggerSigaction();
void HandleSignalLogger(int signum);


void runLoggerServer()
{
    char buffer[1024];
    std::vector<pollfd> poll_sockets;
    pollfd logger_welcome_fd, new_fd;

    // Start a new process
    pid_t pid = fork();
    if (pid == -1)
    {
        exit(EXIT_FAILURE);
    }
    else if(pid > 0)
    {
        // Parent process
        return;
    }

    setLoggerSigaction();

    std::ofstream log_file;
    log_file.open("log.txt", std::ios::out | std::ios::app);

    logger_welcome_fd.fd = createLoggerServerSocket();
    if (logger_welcome_fd.fd == -1)
    {
        log_file << "NAO FOI POSSIVEL ABRIR O SOCKET DO LOGGER SERVER" << std::endl;
    }
    logger_welcome_fd.events = POLLIN;

    poll_sockets.push_back(logger_welcome_fd);

    while(!SIGINT_received || poll_sockets.size() > 1)
    {
        int poll_activity = poll(&poll_sockets[0], poll_sockets.size(), -1);
        if (poll_activity < 0)
        {
            log_file << "Falha no pool()" << std::endl;
            continue;
        }

        for (uint i = 0; i < poll_sockets.size(); i++)
        {
            if (poll_sockets[i].revents == POLLIN)
            {
                if (poll_sockets[i].fd == logger_welcome_fd.fd)
                {
                    // Accept new connection
                    if((new_fd.fd = accept(poll_sockets[i].fd, NULL, NULL)) == -1)
                    {
                        log_file << "Falha no accept()" << std::endl;
                        continue;
                    }
                    new_fd.events = POLLIN | POLLHUP;
                    poll_sockets.push_back(new_fd);
                }
                else
                {
                    // Get message
                    char *next_new_line;
                    int n_bytes = recv(poll_sockets[i].fd, buffer, sizeof(buffer) - 1, MSG_PEEK | MSG_DONTWAIT);
                    if (n_bytes < 0)
                    {
                        log_file << "Falha no recv()" << std::endl;
                        continue;
                    }
                    else if (n_bytes == 0)
                    {
                        // Connection closed
                        close(poll_sockets[i].fd);
                        poll_sockets.erase(poll_sockets.begin() + i);
                        i--;
                        continue;
                    }
                    buffer[n_bytes] = 0;

                    if ((next_new_line = strchr(buffer, '\n')) != NULL)
                    {
                        // There is a complete message on socket
                        int line_length = next_new_line - buffer + 1;

                        if (recv(poll_sockets[i].fd, buffer, line_length, MSG_DONTWAIT) != line_length)
                        {
                            log_file << "Falha no segundo recv()" << std::endl;
                            continue;
                        }

                        log_file.write(buffer, line_length);
                        log_file.flush();
                    }
                }
            }
        }
    }


    for (auto fd : poll_sockets)
    {
        close(fd.fd);
    }

    log_file.close();

    exit(EXIT_SUCCESS);
}


int createLoggerServerSocket()
{
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(LOGGER_SERVER_PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Create a IPv4 TCP socket
    int sockfd = socket(serv_addr.sin_family, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        return -1;
    }

    // Make the socket reusable to make easier to execute the program multiple times
    int yes=1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        return -1;
    }

    // Bind the socket with the port
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
        close(sockfd);
        return -1;
    }

    // Make the socket avaiable for new connections request
    if (listen(sockfd, 50) == -1) {
        return -1;
    }

    return sockfd;
}


/*!
 * \brief Specifies the action to be associated with a specific signal in the logger process.
 */
void setLoggerSigaction()
{
    // Register a signal handler to SIGINT
    struct sigaction sa;
    sa.sa_handler = HandleSignalLogger;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
}


void HandleSignalLogger(int signum)
{
    SIGINT_received = true;
}
