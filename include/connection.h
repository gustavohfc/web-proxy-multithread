/*!
 * \file connection.h
 * \author Gustavo Henrique Fernandes Carvalho
 */

enum ConnectionStatus {OK, TIMEOUT, FILTER_BLOCKED};

class Connection
{
private:
    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_addr_length;

public:
    ConnectionStatus status;

    Connection(int server_socket);
};