/*!
 * \file filter.cpp
 * \author Andre Luis Souto
 */

#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include "filter.h"
#include "server.h"

using namespace std;


// Reading whitelist file and saving it in a vector<string>
void Filter::readWhiteList(vector<string> &whitelist){

	ifstream infile("./files/whitelist.txt");


	for(string line; getline( infile, line );)
	{
		whitelist.push_back(line);    	
	}

}


// Reading blacklist file and saving it in a vector<string>
void Filter::readBlackList(vector<string> &blacklist){

	ifstream infile("./files/blacklist.txt");


	for(string line; getline( infile, line );)
	{
		blacklist.push_back(line);    	
	}

}


// Reading deny_terms file and saving it in a vector<string>
void Filter::readDenyTerms(vector<string> &deny_terms){

	ifstream infile("./files/deny_terms.txt");


	for(string line; getline( infile, line );)
	{
		deny_terms.push_back(line);    	
	}

}


// Filtering the request
void Filter::filteringRequest(HTTPMessage clientRequest){

	std::size_t i = 0;
	vector<string> whitelist, blacklist, deny_terms;
	string url;
	int flag_wl = 0;// flag_bl = 0;


	Filter::readWhiteList(whitelist);
	Filter::readBlackList(blacklist);
	Filter::readDenyTerms(deny_terms);


	url = clientRequest.getHost();

	//TODO : separar em funcoes
	// Checking whitelist
	for (i = 0; i < whitelist.size(); i++){

		if(strcmp(whitelist[i].c_str(), url.c_str()) == 0){
			//........... OK
			flag_wl = 1;
		}

	}


	// Checking blacklist
	if (flag_wl == 0){
		
		for (i = 0; i < blacklist.size(); i++){

			if(strcmp(blacklist[i].c_str(), url.c_str()) == 0){
				//........... OK
				//flag_bl = 1;
			}

		}
	}



	// Checking deny_terms
	//......
}