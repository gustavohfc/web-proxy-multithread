/*!
 * \file filter.cpp
 * \author Andre Luis Souto
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>

#include "filter.h"
#include "log.h"
#include "server.h"

using namespace std;

Filter::Filter() {
    Filter::readWhiteList(whitelist);
    Filter::readBlackList(blacklist);
    Filter::readDenyTerms(deny_terms);
}

/*!
 * \brief Reading whitelist file and saving it in a vector<string>.
 */
void Filter::readWhiteList(vector<string> &whitelist) {
    ifstream infile;
    char str[255];

    infile.open("./files/whitelist");

    if (!infile) {
        log("[Filter] Cannot open whitelist file.");
    }

    //Reading file
    while (infile) {
        infile.getline(str, 255);
        whitelist.push_back(str);
    }

    infile.close();
}

/*!
 * \brief Reading blacklist file and saving it in a vector<string>.
 */
void Filter::readBlackList(vector<string> &blacklist) {
    ifstream infile;
    char str[255];

    infile.open("./files/blacklist");

    if (!infile) {
        log("[Filter] Cannot open blacklist file.");
    }

    //Reading file
    while (infile) {
        infile.getline(str, 255);
        blacklist.push_back(str);
    }

    infile.close();
}

/*!
 * \brief Reading deny_terms file and saving it in a vector<string>.
 */
void Filter::readDenyTerms(vector<string> &deny_terms) {
    ifstream infile;
    char str[255];

    infile.open("./files/deny_terms");

    if (!infile) {
        log("[Filter] Cannot open deny_terms file.");
    }

    //Reading file
    while (infile) {
        infile.getline(str, 255);
        deny_terms.push_back(str);
    }

    deny_terms.pop_back();
    infile.close();
}

/*!
 * \brief Checking if list has url.
 */
int Filter::checkInList(vector<string> &list, string &url) {
    int flag = 0;
    std::size_t i = 0;

    for (i = 0; i < list.size(); i++) {
        if (strcmp(list[i].c_str(), url.c_str()) == 0) {
            flag = 1;
        }
    }

    return flag;
}

/*!
 * \brief Checking if string has terms in deny_term list.
 */
int Filter::checkDenyTerms(string body, vector<string> &deny_terms) {
    std::size_t i = 0;

    if (body.size() != 0) {
        for (i = 0; i < deny_terms.size(); i++) {
            if (body.find(deny_terms[i]) != std::string::npos) {
                log("[Filter] Termo proibido encontrado ( " + deny_terms[i] + " ).");
                return -1;
            }
        }
    }

    return 1;
}

/*!
 * \brief Filtering request message.
 */
ConnectionStatus Filter::filterRequest(HTTPMessage clientRequest) {
    string url;
    int flag_wl = 0, flag_bl = 0, flag_dt = 0;

    url = clientRequest.getHost();

    // Checking whitelist
    flag_wl = Filter::checkInList(whitelist, url);

    if (flag_wl == 1) {
        // Url is in white list
        log("[Filter] URL na whitelist ( " + url + " ), requisicao autorizada.");
        return OK;
    }

    // Checking blacklist
    flag_bl = Filter::checkInList(blacklist, url);

    if (flag_bl == 1) {
        // Url is in black list
        log("[Filter] URL na black list ( " + url + " ), requisicao bloqueada.");
        return URL_BLOCKED;
    }

    // Checking deny_terms
    const vector<char> body = clientRequest.getBody();
    string str(body.begin(), body.end());

    flag_dt = Filter::checkDenyTerms(str, deny_terms);

    if (flag_dt == -1) {
        return INVALID_TERM;
    }

    log("[Filter] Requisicao autorizada ( " + url + " ).");

    return OK;
}

/*!
 * \brief Filtering response message.
 */
ConnectionStatus Filter::filterResponse(HTTPMessage response, std::string url) {
    vector<string> deny_terms;
    int flag_wl = 0, flag_dt = 0;

    // Checking whitelist
    flag_wl = Filter::checkInList(whitelist, url);

    if (flag_wl == 1) {
        // Url is in white list
        log("[Filter] URL na whitelist ( " + url + " ), resposta autorizada.");
        return OK;
    }

    // Checking deny_terms
    const vector<char> body = response.getBody();
    string str(body.begin(), body.end());

    Filter::readDenyTerms(deny_terms);

    flag_dt = Filter::checkDenyTerms(str, deny_terms);

    if (flag_dt == -1) {
        return INVALID_TERM;
    }

    log("[Filter] Resposta autorizada ( " + url + " ).");

    return OK;
}