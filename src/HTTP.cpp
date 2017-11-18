/*!
 * \file HTTP.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
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
    if (position != std::string::npos)
    {
        expected_content_length = strtol(&message.c_str()[ position + sizeof("Content-Length:") ], NULL, 10);
        message_content_length = message.size() - message.find("\r\n\r\n") - sizeof("\r\n\r\n");

        if(message_content_length < expected_content_length)
            return false;
        else
            return true;
    }
    else
    {
        // The message doesn't have any content besides the header
        return true;
    }
}