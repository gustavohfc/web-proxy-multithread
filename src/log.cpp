/*!
 * \file log.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <iostream>
#include <fstream>

static std::ofstream log_file;

/*!
 * \brief Open the log file and save the stream.
 */
bool startLog()
{
    log_file.open("Log_proxy.txt", std::ios::out | std::ios::app | std::ios::binary);

    if (log_file.is_open())
        return true;
    else
        return false;
}

/*!
 * \brief Close the log file stram.
 */
void closeLog()
{
    log_file.close();
}


/*!
 * \brief Log a the message adding a timestamp and a new line.
 */
void log(std::string message)
{
    char timestamp[80];
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    strftime (timestamp, sizeof(timestamp), "[%X %x]",timeinfo);

    log_file << timestamp << "  " << message << std::endl;
}


/*!
 * \brief Log a the message as it is.
 */
void log_raw(std::string message)
{
    log_file << message << std::endl;
}
