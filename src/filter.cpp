/*!
 * \file filter.cpp
 * \author Andre Luis Souto
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
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