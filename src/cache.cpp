
#include "cache.h"
#include "server.h"

void getResponseMessage(Connection& connection)
{
    // TODO: lock file
    // TODO: consult cache

    // Request message from external server
    connection.server_socket = connectToHost(connection.client_request.getHost(), connection.status);

    std::vector<char> message = connection.client_request.getMessage();
    send_buffer(connection.server_socket, (unsigned char *) &message[0], message.size());

    connection.receiveServerResponse();

    // TODO: unlock file
}