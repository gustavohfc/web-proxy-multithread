#include "ui.h"

void UI::cores(int opcao)
{

	bkgd(COLOR_PAIR(opcao));
}


void UI::editMessage()
{	
	/* Limpando a tela */
	clear();

	move(4,1);
	move(1,0);
	printw(" ---------------------------\n");
	printw(" \tSERVIDOR PROXY \n");
	printw(" ---------------------------\n");
	
	//int opcao = 0, tecla;


	/* Oculta o cursor na tela */
//	curs_set(0);


	// /* Selecionar a opcao de alteracao de campo */
	// do{
		

	
	// 	move(5,0);

	// 	(opcao == 0) ? printw(" >") : printw("  ");
	// 	printw("Alterar Nome \n");
	// 	(opcao == 1) ? printw(" >") : printw("  ");
	// 	printw("Alterar Senha \n");
	// 	(opcao == 2) ? printw(" >") : printw("  ");
	// 	printw("Alterar Email \n");
	// 	(opcao == 3) ? printw(" >") : printw("  ");
	// 	printw("Alterar Idade \n");
	
	// 	tecla = getch();

	// 	if(tecla == baixo)
	// 		(opcao == 3) ? opcao = 0: opcao++;
	// 	if(tecla == cima)
	// 		(opcao == 0) ? opcao = 3: opcao--;
		
	// 	clear();

	// } while(tecla != enter);

	// /*Mostra caracteres na tela e o curor*/
	// curs_set(1);
	// echo();

	// /* Alterando o campo desejado */
	// switch (opcao) {

	// 	case 0:	/* nome */

	// 		printw("\n\n\nDigite o novo nome: \n");
			
	// 		break;

	// 	case 1: /* senha */

	// 		noecho();
	// 		printw("\n\n\nDigite o novo senha: \n");
			
	// 		break;

	// 	case 2: /* email */

	// 		printw("\n\n\nDigite o novo email: \n");

	// 		break;

	// 	case 3: /* idade */

	// 		break;
	// }
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

void UI::showUI(std::vector<char> &messageHttp)
{
	clear();

	int opcao = 0;
	int tecla;

	//Inicializa a biblioteca
	initscr();  

	//Comando que habilita pegar teclas do teclado
	keypad(stdscr, true); 
	
	//Oculta o cursor na tela
	curs_set(0);

	//Inicializa o uso das cores
	start_color();

	//Pares de cor que usaremos, texto verde e fundo branco
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
			break;
		case 1:
			UI::blockMessage();
			break;
		case 2:
			UI::editMessage();
			break;	
		default:
			cores(1);
			break; 
	}

	clear();
	noecho();

}