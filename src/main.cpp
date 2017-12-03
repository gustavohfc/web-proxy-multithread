/*!
 * \file main.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <iostream>
#include <stdlib.h>

#include "log.h"
#include "server.h"

int main(int argc, char **argv)
{
    if(!startLog())
    {
        std::cout << "Não foi possivel abrir o arquivo de log" << std::endl;
        exit(EXIT_FAILURE);
    }

    runProxyServer();

    closeLog();

    return 0;
}