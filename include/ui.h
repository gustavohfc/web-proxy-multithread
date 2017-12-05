#ifndef UI_H
#define UI_H

#include "connection.h"

#include <ncurses.h>
#include <string>
#include <vector>


#define cima      259
#define baixo     258
#define esquerda  260
#define direita   261
#define enter     10


class UI
{
private:

	void cores(int opcao);
	
public:

	void handleConnection(Connection *connection, HTTPMessageType t);
	int showUI(HTTPMessage *message);
	void editMessage(std::string &newField, std::string &fieldContent, std::vector<char> message);

};


#endif // UI_H