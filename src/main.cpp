/*!
 * \file main.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include "server.h"

int main(int argc, char **argv)
{
    // TODO: Talvez receber o numero da porta e a pasta do cache como argumento

    runProxyServer();

    return 0;
}