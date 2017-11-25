/*!
 * \file logger_client.cpp
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
#include <time.h>
#include <poll.h>
#include <vector>
#include <iostream>
#include <fstream>

#include "server.h"
#include "logger_client.h"
#include "logger_server.h"

static int logger_socket = -1;
static uint process_number;


bool connectToLogger(uint _process_number)
{
    struct sockaddr_in loggerServerAddr;

    process_number = _process_number;

    //Create socket
    logger_socket = socket(AF_INET , SOCK_STREAM , 0);
    if (logger_socket == -1)
    {
        return false;
    }

    loggerServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    loggerServerAddr.sin_family = AF_INET;
    loggerServerAddr.sin_port = htons(LOGGER_SERVER_PORT);

    //Connect to logger server
    if (connect(logger_socket , (struct sockaddr *)&loggerServerAddr , sizeof(loggerServerAddr)) < 0)
    {
        return false;
    }

    return true;
}


void disconnectFromLogger()
{
    if (logger_socket != -1)
        close(logger_socket);
}


void log(std::string message)
{
    char timestamp[80];
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    strftime (timestamp, sizeof(timestamp), "[%X %x]",timeinfo);

    message.insert(0, " Process " + std::to_string(process_number) + ": ");
    message.insert(0, timestamp);
    message.append("\n");
    send_buffer(logger_socket, (unsigned char *) message.c_str(), message.size());
}