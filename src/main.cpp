/*!
 * \file main.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "log.h"
#include "server.h"

int main(int argc, char **argv) {
    bool enable_gui;

    if (!startLog()) {
        std::cout << "Não foi possivel abrir o arquivo de log" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Check the cmd options
    if (argc > 1 && strcmp("-i", argv[1]) == 0) {
        log("Interface para inspecao de cabecalhos HTTP abilitada.");
        enable_gui = true;
    } else if (argc > 1 && strcmp("-d", argv[1]) == 0) {
        log("Rodando o proxy como daemon.");

        // Run as daemon
        daemon(1, 0);

        // Redirect stdout and stderr to log files
        freopen("stdout_log.txt", "w", stdout);
        freopen("stderr_log.txt", "w", stderr);
    } else {
        enable_gui = false;
    }

    runProxyServer(enable_gui);

    closeLog();

    return 0;
}