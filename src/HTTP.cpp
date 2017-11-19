/*!
 * \file HTTP.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "HTTP.h"
#include "debug.h"

/*!
 * \brief Receive a HTTP message from sockfd.
 * 
 * \param [in] server_socket File descriptor of the server socket.
 * \param [out] status If occur a error when receiving the message the status is changed according to the error.
 * \return String containing the HTTP message received.
 */
std::string receiveHTTPMessage(int sockfd, ConnectionStatus& status)
{
    std::string message;
    static char buffer[256];

    do
    {
        // Receive a piece of the message
        int n_bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n_bytes < 0)
        {
            perror("recv");
            // TODO: Call logger
            status = INVALID_REQUEST;
            break;
        }
        else if (n_bytes == 0)
        {
            fprintf(stderr, "Connection close before receiving the whole message.\n");
            // TODO: Call logger
            status = INVALID_REQUEST;
            break;
        }

        buffer[n_bytes] = '\0';

        message.append(buffer);

    } while(!receiveWholeHTTPMessage(message));

    PRINT_DEBUG("----------------------------------------------------\n");
    PRINT_DEBUG("%s: HTTP message received: \n%s\n", __PRETTY_FUNCTION__, message.c_str());
    PRINT_DEBUG("----------------------------------------------------\n");

    return message;
}


/*!
 * \brief Check if the message is incomplete.
 * 
 * \param [in] message Message received until now.
 * \return Returns if it's necessary to receive more data from the socket to fulfill the message.
 */
bool receiveWholeHTTPMessage(const std::string& message)
{
    long int expected_content_length, message_content_length;

    // Check if the header isn't complete
    if (message.find("\r\n\r\n") == std::string::npos)
    {
        // Receive the remaing header
        return false;
    }

    // Get the Content-Length
    std::size_t position = message.find("Content-Length:");
    if (position == std::string::npos)
    {
        // The message doesn't have any content besides the header
        return true;
    }

    expected_content_length = strtol(&message.c_str()[ position + sizeof("Content-Length:") ], NULL, 10);
    message_content_length = message.size() - message.find("\r\n\r\n") - sizeof("\r\n\r\n");

    return message_content_length >= expected_content_length;
}


void sendHTTPMessage(const std::string& host, std::string message, ConnectionStatus& status)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    char s[INET6_ADDRSTRLEN];
    int rv;
    uint bytes_sent = 0;

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
        status = CANNOT_SEND_MESSAGE;
        return;
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
        status = CANNOT_SEND_MESSAGE;
        return;
    }

    if (((struct sockaddr *) p->ai_addr)->sa_family == AF_INET)
    {
        inet_ntop(p->ai_family, &(((struct sockaddr_in *)((struct sockaddr *) p->ai_addr))->sin_addr), s, sizeof s);
    }
    else
    {
        inet_ntop(p->ai_family, &(((struct sockaddr_in6 *)((struct sockaddr *) p->ai_addr))->sin6_addr), s, sizeof s);
    }

    freeaddrinfo(servinfo);

    while (bytes_sent < message.size())
    {
        rv = send(sockfd, &message.c_str()[bytes_sent], message.size() - bytes_sent, 0);
        if (rv == -1)
        {
            perror("send");
            status = CANNOT_SEND_MESSAGE;
        }

        bytes_sent += rv;
    }

    close(sockfd);
}