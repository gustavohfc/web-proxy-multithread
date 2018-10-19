/*!
 * \file filter.h
 * \author Andre Luis Souto
 */

#ifndef FILTER_H
#define FILTER_H

#include <string.h>
#include <string>

#include "HTTPMessage.h"
#include "connection.h"

#include <string>
#include <vector>

class Filter {
   private:
    std::vector<std::string> whitelist, blacklist, deny_terms;

    void readWhiteList(std::vector<std::string> &whitelist);
    void readBlackList(std::vector<std::string> &blacklist);
    void readDenyTerms(std::vector<std::string> &deny_terms);
    int checkInList(std::vector<std::string> &list, std::string &url);
    int checkDenyTerms(std::string body, std::vector<std::string> &deny_terms);

   public:
    Filter();
    ConnectionStatus filteringRequest(HTTPMessage clientRequest);
    ConnectionStatus filteringResponse(HTTPMessage response, std::string url);
};

#endif  // FILTER_H