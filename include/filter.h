/*!
 * \file filter.h
 * \author Andre Luis Souto
 */

#ifndef FILTER_H
#define FILTER_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string>

#include "logger_client.h"
#include "logger_server.h"
#include "HTTPMessage.h"

class Filter
{
private:
    


public:
	void readWhiteList(std::vector<std::string> &whitelist);
	void readBlackList(std::vector<std::string> &blacklist);
	void readDenyTerms(std::vector<std::string> &deny_terms);

};

#endif // FILTER_H