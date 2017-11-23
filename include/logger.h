/*!
 * \file logger.h
 * \author Gustavo Henrique Fernandes Carvalho
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <string>

void runLoggerServer();
void log(int logger_socket, std::string message);
int connectToLogger();

#endif // LOGGER_H