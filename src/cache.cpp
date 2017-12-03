
#include "cache.h"
#include "server.h"

#define BUFFER_SIZE 10000

	// load cache folder?
	// Cache::Cache() {}
	// Cache::~Cache() {}
	
	// bool Cache::find(size_t key) {
	// 	for (unsigned i = 0; i < cache.size(); ++i) 
	// 		if(cache[i].key == key) return true;
		
	// 	return false;
	// }
	// void Cache::set(size_t key, HTTPMessage data) {}
	// HTTPMessage Cache::get(size_t key) {}

void getResponseMessage(Connection& connection)
{
    // TODO: consult cache
    // IF found THEN verify ELSE request

    std::hash<std::string> hasher;
    std::stringstream ss;
    std::string filename;
    std::vector<char> data;

    // GET url from connection
    // calculate hash
    size_t url_hash = hasher(connection.client_request.getPath());
    ss << CACHE_PATH << "/" << url_hash;
    ss >> filename;

    FILE* fp;
    // search for it
    if((fp = fopen(filename.c_str(), "r")) != NULL) {
    	// TODO: load cache
    	log("[Cache] Pagina encontrada em cache!");

    	char *buffer = new char[BUFFER_SIZE];
    	do 
    	{
    	    int n_bytes = fread(buffer, sizeof(char), BUFFER_SIZE-1, fp);
    	    if (connection.response.addMessageData(buffer, n_bytes) != OK)
    	        break;

    	} while (!connection.response.is_message_complete());

    	delete[] buffer;

		// TODO: verify valid cache
		// IF cache is valid THEN return cache ELSE request

    	std::string head_request ("HEAD ");
    	head_request.append(connection.client_request.getPath());
    	head_request.append("\r\n\r\n");
    	ConnectionStatus status;
    	// ss << "HEAD " << connection.client_request.getPath() << "\r\n\r\n";
    	// ss >> head_request;


		int verify_socket = connectToHost(connection.client_request.getHost(), connection.status);
		if(connection.status != OK) {
			log("[Cache] Falha ao conectar com servidor");
			connection.status = FAIL_CONNECT_CACHE;
			return;
		}
		// std::cout << head_request << std::endl << head_request.size();
    	send_buffer(verify_socket, (unsigned char *) &head_request[0], head_request.size());

    	HTTPMessage response = HTTPMessage(RESPONSE);

    	log("[Cache] Recebendo resposta HEAD do servidor");

    	receiveMessage(verify_socket, response, status);
    	if(status == OK) {
    		// check valid: if up to date
    		if((response.getHeaders().find("Last-Modified")->second != connection.response.getHeaders().find("Last-Modified")->second)
    			|| (response.getHeaders().find("Date")->second != connection.response.getHeaders().find("Date")->second)) {
    			log("[Cache] Cache up-to-date");

    			fclose(fp);
    			return;
    		}
		}
		else
			log("[Cache] Falha ao receber resposta HEAD do servidor, requisição não suportada");
		
    }

    // Request message from external server
    connection.server_socket = connectToHost(connection.client_request.getHost(), connection.status);

    std::vector<char> message = connection.client_request.getMessage();
    send_buffer(connection.server_socket, (unsigned char *) &message[0], message.size());

    connection.receiveServerResponse();

    // TODO: save new cache
    // calculate hash and save for other requests
    // connection.response é onde a mensagem sera retornada de acordo com a funçao "receiveMessage" em "server.cpp"
    // usar o HTTPMessage.path como endereço completo

    log("[Cache] Salvando resposta do servidor");

    if((fp = fopen(filename.c_str(), "wb")) == NULL) {
    	printf("*ERRO: Can't open/create file cache \"%s\"\n", filename.c_str());
    	exit(1);
    }

    data = connection.response.getMessage();
    fwrite(&data[0], sizeof(char), data.size(), fp);

	fclose(fp);
}