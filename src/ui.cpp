#include "ui.h"
#include "log.h"
#include <iostream>


void UI::cores(int opcao)
{
	bkgd(COLOR_PAIR(opcao));
}


void UI::editMessage(std::string &newField, std::string &fieldContent, std::vector<char> messageHeader)
{	
	char buffer[50];

	clear();

	move(4,1);
	move(1,0);
	printw("|--------------------------------------------------------------------|\n");
	printw("|                           SERVIDOR PROXY                           |\n");
	printw("|--------------------------------------------------------------------|\n");
	
	curs_set(0);

	// Mostra cabeçalho original
	printw("\n ");
	for (std::size_t i = 0; i < messageHeader.size(); i++) {
		if (messageHeader[i] == '\r') {
			continue;
		}
		if (messageHeader[i] == '\n') {
			printw("\n ");
			continue;
		}
		printw("%c", messageHeader[i]);
	}

	printw("\n Escreva o campo que deseja modificar ou criar: ");
	scanw("%s", buffer);
	newField = std::string(buffer);

	printw(" Conteudo do campo: ");
	scanw("%s", buffer);
	fieldContent = std::string(buffer);
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
	init_pair(2,COLOR_WHITE,COLOR_BLACK);
	init_pair(3,COLOR_YELLOW,COLOR_RED);
	init_pair(4,COLOR_BLACK,COLOR_GREEN);

	cores(2);

	do {
		

		move(4,1);
		move(1,0);
		printw("|--------------------------------------------------------------------|\n");
		printw("|                           SERVIDOR PROXY                           |\n");
		printw("|--------------------------------------------------------------------|\n");
		
		move(5,0);
		(opcao == 0) ? printw(" > ") : printw("  ");
		printw("ENVIAR\n");
		(opcao == 1) ? printw(" > ") : printw("  ");
		printw("BLOQUEAR\n");
		(opcao == 2) ? printw(" > ") : printw("  ");
		printw("ALTERAR CABECALHO\n\n\n");

		// Mostra messagem atual
		printw(" ");
		for (std::size_t i = 0; i < messageHttp.size(); i++) {
			if (messageHttp[i] == '\r') {
				continue;
			}
			if (messageHttp[i] == '\n') {
				printw("\n ");
				continue;
			}
			printw("%c", messageHttp[i]);
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
			endwin();
			log("[Inspecao] Mensagem encaminhada");
			return 1;

		case 1:
			endwin();
			log("[Inspecao] Mensagem bloqueada");
			return -1;

		case 2:
			UI::editMessage(newField,fieldContent, message->getHeader());
			message->changeHeader(newField,fieldContent);
			messageHttp = message->getMessage();
			return 0;

		default:
			cores(1);
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