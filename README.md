Projeto Final de TR2
====================

Integrantes do grupo:
1. André Luis Souto Ferreira (14/0016261)
2. Fabio Trevizolo de Souza (14/0019774)
3. Gustavo Henrique Fernandes Carvalho (14/0021671)
4. Ismael Coelho Medeiros (14/0083162)

Dependências
------------
Este projeto dependende da biblioteca `ncurses.h`. Para que o programa possa ser executado, essa biblioteca
precisa estar instalada na máquina.

Instruções de execução
----------------------

Primeiramente, compile o projeto, digitando:

    $ make

Para executar o programa em modo padrão, digite:

    $ make run

Ou, para executar o programa em modo inspeção, digite:

    $ make run_inspecao

Ou, para executar o programa em modo daemon, onde o programa será executado em background no 
sistema operacional, digite:

    $ make run_daemon


Para limpar arquivo gerados no build, digite:

    $ make clean

Para acompanhar o preenchimento do arquivo `Log_proxy.txt`, gerado pelo logger, execute em um terminal
separado:

    $ tail -f Log_proxy.txt