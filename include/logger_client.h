/*!
 * \file logger_client.h
 * \author Gustavo Henrique Fernandes Carvalho
 */

#ifndef LOGGER_CLIENT_H
#define LOGGER_CLIENT_H

#include <string>

void log(std::string message);
bool connectToLogger(uint _process_number);

#endif // LOGGER_CLIENT_H