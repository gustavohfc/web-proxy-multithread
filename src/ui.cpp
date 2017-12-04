#include "ui.h"
#include <iostream>


void UI::cores(int opcao)
{

	bkgd(COLOR_PAIR(opcao));
}


void UI::editMessage(std::string &newField, std::string &fieldContent)
{	
	char buffer[50];

	clear();

	move(4,1);
	move(1,0);
	printw(" ---------------------------\n");
	printw(" \tSERVIDOR PROXY \n");
	printw(" ---------------------------\n");
	
	curs_set(0);


	printw("Nome do campo: ");
	scanw("%s",buffer);
	
	newField = std::string(buffer);

	printw("\n\n");
	printw("Conteudo do campo: ");
	scanw("%s",buffer);
	
	fieldContent = std::string(buffer);

	//endwin();

}


void UI::sendMessage()
{
	clear();
	
	move(4,1);
	move(1,0);
	printw(" ---------------------------\n");
	printw(" \tSERVIDOR PROXY \n");
	printw(" ---------------------------\n");

	printw(" Mensagem encaminhada.");

	getch();

	endwin();
}


void UI::blockMessage()
{
	clear();
	
	move(4,1);
	move(1,0);
	printw(" ---------------------------\n");
	printw(" \tSERVIDOR PROXY \n");
	printw(" ---------------------------\n");

	printw(" Mensagem bloqueada.");

	getch();

	endwin();
}


int UI::showUI(HTTPMessage *message/*std::vector<char> &messageHttp*/)
{
	clear();

	int opcao = 0;
	int tecla;
	std::vector<char> messageHttp;
	std::string newField, fieldContent;


	messageHttp = message->getMessage();

	initscr();  

	keypad(stdscr, true); 
	
	curs_set(0);

	start_color();

	init_pair(1,COLOR_GREEN,COLOR_WHITE);
	init_pair(2,COLOR_BLUE,COLOR_BLACK);
	init_pair(3,COLOR_YELLOW,COLOR_RED);
	init_pair(4,COLOR_BLACK,COLOR_GREEN);

	cores(2);

	std::size_t i = 0;
	

	do{
		

		move(4,1);
		move(1,0);
		printw(" ---------------------------\n");
		printw(" \tSERVIDOR PROXY \n");
		printw(" ---------------------------\n");
		
		move(6,0);
		(opcao == 0) ? printw(" >") : printw("  ");
		printw("ENVIAR\n");
		(opcao == 1) ? printw(" >") : printw("  ");
		printw("BLOQUEAR\n");
		(opcao ==2 ) ? printw(" >") : printw("  ");
		printw("ALTERAR CABECALHO\n\n\n");


		for(i=0; i < messageHttp.size(); i++){
			
			if(messageHttp[i] != '\r'){

				printw("%c",messageHttp[i]);	
			}
			
		}
		
		tecla = getch();

		if(tecla == baixo)
			(opcao == 2) ? opcao = 0: opcao++;
		if(tecla == cima)
			(opcao == 0) ? opcao = 2: opcao--;
		
		clear();

	} while(tecla != enter);

	switch(opcao)
	{
		case 0:
		
			UI::sendMessage();
			return 1;
		
			break;
		case 1:

			UI::blockMessage();
			return -1;
		
			break;
		case 2:
		
			UI::editMessage(newField,fieldContent);
			message->changeHeader(newField,fieldContent);
			
			messageHttp = message->getMessage();

			return 0;
		
			break;	
		default:

			cores(1);
		
			break; 
	}

	return 0;
}


void UI::handleConnection(Connection *connection, HTTPMessageType t)
{
	int flag = 0;

	while(flag == 0)
	{
		if(t == REQUEST)
		{
			flag = UI::showUI(&connection->client_request);
		}
		else if(t == RESPONSE)
		{
			flag = UI::showUI(&connection->response);
		}
    	
	}

	if(flag == -1)
	{
		connection->status = BLOCKED_BY_INSPECTOR;
	}

}