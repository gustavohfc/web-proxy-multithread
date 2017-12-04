#ifndef UI_H
#define UI_H

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

	void showUI(std::vector<char> &messageHttp);
	void editMessage();
	void sendMessage();
	void blockMessage();

};


#endif // UI_H