/*!
 * \file cache.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include "cache.h"
#include "server.h"

#define BUFFER_SIZE 10000

void Cache::getResponseMessage(Connection &connection) {
    bool updateCache = false;

    pthread_mutex_lock(&mutex);

    CachedPage &cachedPage = pages[connection.client_request.getPath()];

    pthread_mutex_lock(&cachedPage.pageReadMutex);

    pthread_mutex_unlock(&mutex);

    if (!cachedPage.data.empty()) {
        log("[Cache] Pagina encontrada em cache!");

        HTTPMessage response = HTTPMessage(RESPONSE);
        ConnectionStatus cache_request_status = OK;
        std::string cache_request;

        int verify_socket = connectToHost(connection.client_request.getHost(), connection.status);
        if (connection.status != OK) {
            log("[Cache] Falha ao conectar com servidor");
            connection.status = FAIL_CONNECT_CACHE;
            return;
        }

        // load cache
        char *buffer = &cachedPage.data[0];
        connection.response.addMessageData(buffer, cachedPage.data.size());

        pthread_mutex_unlock(&cachedPage.pageReadMutex);

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
            } else if (response.getStatusCode() == "200") {  // modified ok
                log("[Cache] Requisição GET CONDITIONAL retornou atualização");
                connection.response = response;
                log("[Cache] Salvando resposta do servidor");
                updateCache = true;
            } else {
                // TODO: handle when the server doesn't support "If-Modified-Since: ",
                // for now the cache is considered to be up-to-date in that case
                log("[Cache] Cache up-to-date (If-Modified-Since not supported by the server or file was deleted)");
            }
        } else {
            log("[Cache] Falha na requisição GET CONDITIONAL ao servidor");
            connection.status = FAIL_CONNECT_CACHE;
        }

        close(verify_socket);
    } else {
        pthread_mutex_unlock(&cachedPage.pageReadMutex);

        // File not in cache, request message from the server
        connection.server_socket = connectToHost(connection.client_request.getHost(), connection.status);

        std::vector<char> message = connection.client_request.getMessage();
        send_buffer(connection.server_socket, (unsigned char *)&message[0], message.size());

        connection.receiveServerResponse();

        log("[Cache] Salvando resposta do servidor");

        updateCache = true;
    }

    if (updateCache) {
        pthread_mutex_lock(&mutex);
        pthread_mutex_lock(&cachedPage.pageReadMutex);
        cachedPage.data = connection.response.getMessage();
        pthread_mutex_unlock(&cachedPage.pageReadMutex);
        pthread_mutex_unlock(&mutex);
    }
}

void Cache::saveToCache(CachedPage &cachedPage, std::vector<char> data) {
    log("[Cache] Salvando resposta do servidor");

    cachedPage.data = data;
}