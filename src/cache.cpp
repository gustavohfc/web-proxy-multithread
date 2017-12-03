
#include "cache.h"
#include "server.h"

void getResponseMessage(Connection& connection)
{
    /******************************************************************************/
    // TODO: consult cache
    // GET url from connection
    // calculate hash
    // search for it
    // IF found THEN verify ELSE request

    /******************************************************************************/
	// TODO: verify valid cache
	// IF cache is valid THEN return cache ELSE request

    /******************************************************************************/
    // Request message from external server
    connection.server_socket = connectToHost(connection.client_request.getHost(), connection.status);

    std::vector<char> message = connection.client_request.getMessage();
    send_buffer(connection.server_socket, (unsigned char *) &message[0], message.size());

    connection.receiveServerResponse();

    /******************************************************************************/
    // TODO: save new cache
    // calculate hash and save for other requests
    // connection.response é onde a mensagem sera retornada de acordo com a funçao "receiveMessage" em "server.cpp"
    // usar o HTTPMessage.path como endereço completo

    std::stringstream ss;
    std::string filename;
    std::hash<std::string> hasher;  

    ss << CACHE_PATH << "/" << hasher(connection.client_request.getPath());
    ss >> filename;

    FILE* fp;
    if((fp = fopen(filename.c_str(), "wb")) == NULL) {
    	printf("*ERRO: Can't open file (%s)\n", filename.c_str());
    	exit(1);
    }

    std::vector<char> data;
    data = connection.response.getMessage();
    fwrite(&data[0], sizeof(char), data.size(), fp);
    fclose(fp);
}