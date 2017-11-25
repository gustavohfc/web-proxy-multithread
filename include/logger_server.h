/*!
 * \file logger_server.h
 * \author Gustavo Henrique Fernandes Carvalho
 */

#ifndef LOGGER_SERVER_H
#define LOGGER_SERVER_H

/*!
 * \brief Server port to receive new connections.
 */
#define LOGGER_SERVER_PORT 22222

#include <string>

void runLoggerServer();

#endif // LOGGER_SERVER_H