/*!
 * \file filter.cpp
 * \author Andre Luis Souto
 */

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>

#include "filter.h"
#include "server.h"

using namespace std;


// Reading whitelist file and saving it in a vector<string>
void Filter::readWhiteList(vector<string> &whitelist){


	ifstream infile;
	char str[255];

	infile.open("/home/andre/Documents/UnB/TR2/TrabFinal/web-proxy/files/whitelist");
	

	 if(!infile) {
    	cout << "Cannot open input file.\n";	
  	}

  	//Reading file
	while(infile)
	{
		infile.getline(str,255);
		whitelist.push_back(str);   
	}

	infile.close();
}


// Reading blacklist file and saving it in a vector<string>
void Filter::readBlackList(vector<string> &blacklist){


	ifstream infile;
	char str[255];

	infile.open("/home/andre/Documents/UnB/TR2/TrabFinal/web-proxy/files/blacklist");
	

	 if(!infile) {
    	cout << "Cannot open input file.\n";	
  	}

  	//Reading file
	while(infile)
	{
		infile.getline(str,255);
		blacklist.push_back(str);    	
	}

	infile.close();
}


// Reading deny_terms file and saving it in a vector<string>
void Filter::readDenyTerms(vector<string> &deny_terms){


	ifstream infile;
	char str[255];

	infile.open("/home/andre/Documents/UnB/TR2/TrabFinal/web-proxy/files/deny_terms");
	

	 if(!infile) {
    	cout << "Cannot open input file.\n";	
  	}

  	//Reading file
	while(infile)
	{
		infile.getline(str,255);
		deny_terms.push_back(str);    	
	}

	infile.close();
}


// Checking if list has url
int Filter::checkInList(vector<string> &list, string &url){


	int flag = 0;
	std::size_t i = 0;


	for (i = 0; i < list.size(); i++){

		if(strcmp(list[i].c_str(), url.c_str()) == 0){
			
			flag = 1;
		}

	}

	return flag;
} 


// Filtering request message
ConnectionStatus Filter::filteringRequest(HTTPMessage clientRequest){


	vector<string> whitelist, blacklist, deny_terms;
	string url;
	int flag_wl = 0, flag_bl = 0;


	Filter::readWhiteList(whitelist);
	Filter::readBlackList(blacklist);
	Filter::readDenyTerms(deny_terms); 

	url = clientRequest.getHost();

	// Checking whitelist
	flag_wl = Filter::checkInList(whitelist,url);

	if (flag_wl == 1){
		
		// Url is in white list
		return OK;
	}

	// Checking blacklist
	if (flag_wl == 0){

		flag_bl = Filter::checkInList(blacklist,url);	
	}

	if (flag_bl == 1){
		
		// Url is in black list
		return FILTER_BLOCKED;
	}


	// Checking deny_terms
	//......
	return OK;

}


// Filtering response message
ConnectionStatus Filter::filteringResponse(HTTPMessage response){

	//Deny_terms

	return OK;
}