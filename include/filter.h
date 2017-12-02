/*!
 * \file filter.h
 * \author Andre Luis Souto
 */

#ifndef FILTER_H
#define FILTER_H

#include <string>
#include <string.h>

#include "logger_client.h"
#include "logger_server.h"
#include "HTTPMessage.h"
#include "connection.h"

class Filter
{
private:

	void readWhiteList(std::vector<std::string> &whitelist);
	void readBlackList(std::vector<std::string> &blacklist);
	void readDenyTerms(std::vector<std::string> &deny_terms);
	int checkInList(std::vector<std::string> &list, std::string &url);

public:

	ConnectionStatus filteringRequest(HTTPMessage clientRequest);
	ConnectionStatus filteringResponse(HTTPMessage response);

};

#endif // FILTER_H