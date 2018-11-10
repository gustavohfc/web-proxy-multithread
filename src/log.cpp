/*!
 * \file log.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <fstream>
#include <iostream>
#include "log.h"

static std::ofstream log_file;

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;


/*!
 * \brief Open the log file and save the stream.
 */
bool startLog() {
    log_file.open("Log_proxy.txt", std::ios::out | std::ios::app | std::ios::binary);

    if (log_file.is_open()) {
        log("\n\n\n---- Starting ----\n\n\n");
        return true;
    } else {
        return false;
    }
}

/*!
 * \brief Close the log file stram.
 */
void closeLog() {
    log_file.close();
}

/*!
 * \brief Log a the message adding a timestamp and a new line.
 */
void log(std::string message) {
    char timestamp[80];
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(timestamp, sizeof(timestamp), "[%X %x]", timeinfo);

    pthread_mutex_lock(&log_mutex);
    log_file << "[" << pthread_self() << "] " << timestamp << "  " << message << std::endl;
    pthread_mutex_unlock(&log_mutex);
}
