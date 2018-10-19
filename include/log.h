/*!
 * \file log.h
 * \author Gustavo Henrique Fernandes Carvalho
 */

#ifndef LOG_H
#define LOG_H

bool startLog();
void closeLog();
void log(std::string message);
void log_raw(std::string message);

#endif  // LOG_H