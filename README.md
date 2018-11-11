Projeto de Programação Concorrente
====================

    Gustavo Henrique Fernandes Carvalho (14/0021671)

Instruções de execução
----------------------

Primeiramente, compile o projeto, digitando:

    $ make

Para executar o programa em modo padrão, digite:

    $ make run

Ou, para executar o programa em modo daemon, onde o programa será executado em background no 
sistema operacional, digite:

    $ make run_daemon


Para limpar arquivo gerados no build, digite:

    $ make clean

Para acompanhar o preenchimento do arquivo `Log_proxy.txt`, gerado pelo logger, execute em um terminal
separado:

    $ tail -f Log_proxy.txt