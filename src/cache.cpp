/*!
 * \file cache.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include "cache.h"
#include "server.h"

#define BUFFER_SIZE 10000

void getResponseMessage(Connection &connection) {
    std::hash<std::string> hasher;
    std::stringstream ss;
    std::string filename;
    std::vector<char> data;

    // GET url from connection
    // calculate hash
    size_t url_hash = hasher(connection.client_request.getPath());
    ss << CACHE_PATH << "/" << url_hash;
    ss >> filename;

    FILE *fp;
    // search for it
    if ((fp = fopen(filename.c_str(), "r")) != NULL) {
        log("[Cache] Pagina encontrada em cache!");

        HTTPMessage response = HTTPMessage(RESPONSE);
        ConnectionStatus cache_request_status;
        std::string cache_request;

        int verify_socket = connectToHost(connection.client_request.getHost(), connection.status);
        if (connection.status != OK) {
            log("[Cache] Falha ao conectar com servidor");
            connection.status = FAIL_CONNECT_CACHE;
            return;
        }

        // load cache
        char *buffer = new char[BUFFER_SIZE];
        do {
            int n_bytes = fread(buffer, sizeof(char), BUFFER_SIZE - 1, fp);
            if (connection.response.addMessageData(buffer, n_bytes) != OK)
                break;

        } while (!connection.response.is_message_complete());
        delete[] buffer;

        cache_request = "GET " + connection.client_request.getPath() + " HTTP/1.1\r\n";
        cache_request += "Host: " + connection.client_request.getHost() + "\r\n";
        if (connection.response.getHeaders().find("Last-Modified") != connection.response.getHeaders().end())
            cache_request += "If-Modified-Since: " + connection.response.getHeaders().find("Last-Modified")->second + "\r\n";
        if (connection.response.getHeaders().find("ETag") != connection.response.getHeaders().end())
            cache_request += "If-None-Match: " + connection.response.getHeaders().find("ETag")->second + "\r\n";
        cache_request += "\r\n";

        log("[Cache] Enviando requisição GET CONDITIONAL ao servidor");
        send_buffer(verify_socket, (unsigned char *)&cache_request[0], cache_request.size());

        receiveMessage(verify_socket, response, cache_request_status);
        if (cache_request_status == OK) {
            if (response.getStatusCode() == "304") {  // Not Modified
                log("[Cache] Cache up-to-date");

                fclose(fp);
                return;
            } else if (response.getStatusCode() == "200") {  // modified ok
                log("[Cache] Requisição GET CONDITIONAL retornou atualização");
                // TODO: reaproveitar a resposta do get e retornar a pagina
                connection.response = response;
            }
        } else {
            log("[Cache] Falha na requisição GET CONDITIONAL ao servidor");
            connection.status = FAIL_CONNECT_CACHE;
        }

        close(verify_socket);
    } else {
        // Request message from external server
        connection.server_socket = connectToHost(connection.client_request.getHost(), connection.status);

        std::vector<char> message = connection.client_request.getMessage();
        send_buffer(connection.server_socket, (unsigned char *)&message[0], message.size());

        connection.receiveServerResponse();
    }
    // TODO: save new cache
    // calculate hash and save for other requests
    // connection.response é onde a mensagem sera retornada de acordo com a funçao "receiveMessage" em "server.cpp"
    // usar o HTTPMessage.path como endereço completo

    if (connection.status == OK && connection.response.getStatusCode() == "200") {
        log("[Cache] Salvando resposta do servidor");

        if ((fp = fopen(filename.c_str(), "wb")) == NULL) {
            printf("*ERRO: Can't open/create file cache \"%s\"\n", filename.c_str());
            exit(1);
        }

        data = connection.response.getMessage();
        fwrite(&data[0], sizeof(char), data.size(), fp);

        fclose(fp);
    }
}