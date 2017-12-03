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

	infile.open("./files/whitelist");
	

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

	infile.open("./files/blacklist");
	

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

	infile.open("./files/deny_terms");


	 if(!infile) {
    	cout << "Cannot open input file.\n";	
  	}

  	//Reading file
	while(infile)
	{
		infile.getline(str,255);
		deny_terms.push_back(str);   	
	}

	deny_terms.pop_back();
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


// Checking if string has terms in deny_term list
int Filter::checkDenyTerms(string body, vector<string> &deny_terms){

	std::size_t i = 0;


	if(body.size() != 0){

		for (i = 0; i < deny_terms.size(); i++){

			if(body.find(deny_terms[i]) != std::string::npos){
				
				cout << "INVALID TERM FOUND\n\n";
				return -1;
			}

		}
	}

	return 1;
}


// Filtering request message
ConnectionStatus Filter::filteringRequest(HTTPMessage clientRequest){

	vector<string> whitelist, blacklist, deny_terms;
	string url;
	int flag_wl = 0, flag_bl = 0, flag_dt = 0;


	Filter::readWhiteList(whitelist);
	Filter::readBlackList(blacklist);
	Filter::readDenyTerms(deny_terms); 

	url = clientRequest.getHost();

	// Checking whitelist
	flag_wl = Filter::checkInList(whitelist,url);

	if (flag_wl == 1){
		
		// Url is in white list
		cout << "URL OK\n\n";
		return OK;
	}

	// Checking blacklist
	flag_bl = Filter::checkInList(blacklist,url);	

	if (flag_bl == 1){
		
		// Url is in black list
		cout << "URL BLOCKED\n\n";
		return URL_BLOCKED;
	}

	// Checking deny_terms
	const vector<char> body = clientRequest.getBody();
	string str(body.begin(),body.end());
	
	flag_dt = Filter::checkDenyTerms(str,deny_terms);

	if (flag_dt == -1){

		return INVALID_TERM;
	}

	return OK;

}


// Filtering response message
ConnectionStatus Filter::filteringResponse(HTTPMessage response){

	vector<string> deny_terms;
	int flag_dt = 0;

	// Checking deny_terms
	const vector<char> body = response.getBody();
	string str(body.begin(),body.end());
	
	Filter::readDenyTerms(deny_terms); 

	flag_dt = Filter::checkDenyTerms(str,deny_terms);

	if (flag_dt == -1){

		return INVALID_TERM;
	}

	return OK;

}