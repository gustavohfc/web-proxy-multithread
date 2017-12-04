/*!
 * \file main.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "server.h"

int main(int argc, char **argv)
{
    bool enable_gui;

    if(!startLog())
    {
        std::cout << "Não foi possivel abrir o arquivo de log" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (argc > 1 && strcmp("-i", argv[1]) == 0)
    {
        log("Interface para inspecao de cabecalhos HTTP abilitada.");
        enable_gui = true;
    }
    else
    {
        enable_gui = false;
    }

    runProxyServer(enable_gui);

    closeLog();

    return 0;
}