/*!
 * \file cache.cpp
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include "cache.h"
#include "server.h"

CachedPage &Cache::getLockedCachedPage(std::string path) {
    log("[Cache] Esperando o lock para leitura do cache");

    pthread_mutex_lock(&pagesMutex);
    auto &cachedPage = pages[path];
    pthread_mutex_unlock(&pagesMutex);

    // Thread tentando atualizar o cache tem prioridade
    pthread_mutex_lock(&cachedPage.mutex);
    while (cachedPage.tryingToUpdate) {
        pthread_cond_wait(&cachedPage.updateCompleteCond, &cachedPage.mutex);
    }

    cachedPage.nReading++;

    pthread_mutex_unlock(&cachedPage.mutex);

    return cachedPage;
}

void Cache::unlockCachedPage(CachedPage &cachedPage) {
    log("[Cache] Encerrando a leitura do cache");

    pthread_mutex_lock(&cachedPage.mutex);

    if (--cachedPage.nReading == 0 && cachedPage.tryingToUpdate) {
        pthread_cond_broadcast(&cachedPage.startUpdateCond);
    }
    pthread_mutex_unlock(&cachedPage.mutex);
}

void Cache::updateCachedPage(CachedPage &cachedPage, std::vector<char> data) {
    log("[Cache] Atualizando página no cache");

    pthread_mutex_lock(&cachedPage.mutex);

    cachedPage.tryingToUpdate = true;

    while (cachedPage.nReading > 0) {
        pthread_cond_wait(&cachedPage.startUpdateCond, &cachedPage.mutex);
    }

    cachedPage.data = data;

    cachedPage.tryingToUpdate = false;

    pthread_cond_broadcast(&cachedPage.updateCompleteCond);

    pthread_mutex_unlock(&cachedPage.mutex);
}

void Cache::getResponseMessage(Connection &connection) {
    auto &cachedPage = getLockedCachedPage(connection.client_request.getPath());

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
        connection.response.addMessageData(&cachedPage.data[0], cachedPage.data.size());

        unlockCachedPage(cachedPage);

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
                updateCachedPage(cachedPage, connection.response.getMessage());
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
        unlockCachedPage(cachedPage);

        // File not in cache, request message from the server
        connection.server_socket = connectToHost(connection.client_request.getHost(), connection.status);

        std::vector<char> message = connection.client_request.getMessage();
        send_buffer(connection.server_socket, (unsigned char *)&message[0], message.size());

        connection.receiveServerResponse();

        log("[Cache] Salvando resposta do servidor");

        updateCachedPage(cachedPage, connection.response.getMessage());
    }
}
