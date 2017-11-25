
#include "cache.h"
#include "server.h"

void getResponseMessage(Connection& connection)
{
    // TODO: lock file
    // TODO: consult cache

    // Request message from external server
    connection.server_socket = connectToHost(connection.client_request.getHost(), connection.status);
    send_buffer(connection.server_socket, (unsigned char *) connection.client_request.getMessage(), connection.client_request.getMessageLength());
    connection.receiveServerResponse();

    // TODO: unlock file
}