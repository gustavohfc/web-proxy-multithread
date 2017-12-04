/*!
 * \file HTTPMessage.h
 * \author Ismael Coelho Medeiros
 * \author Gustavo Henrique Fernandes Carvalho
 */

#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

#include <string>
#include <vector>
#include <map>

enum ConnectionStatus {OK, INVALID_REQUEST, URL_BLOCKED, INVALID_TERM, CANNOT_CONNECT_TO_HOST, FAIL_CONNECT_CACHE, BLOCKED_BY_INSPECTOR};

enum HTTPMessageType {REQUEST, RESPONSE};

class HTTPMessage
{
private:
    bool header_complete, body_complete;
    HTTPMessageType type;

    // Used on request messages
    std::string path;
    std::string method;

    // Used on response messages
    std::string status_code;
    std::string status_phrase;

    std::string version;

    std::map<std::string, std::string> headers;
    std::vector<char> body;

public:
    HTTPMessage(HTTPMessageType type);
    ~HTTPMessage();

    ConnectionStatus addMessageData(const char *buffer, int n_bytes);
    void parseHeaders();
    std::vector<char> getMessage() const;
    const std::vector<char> & getBody() const;
    int getBodyLength() const;
    std::string getHost() const;
    void changeHeader(std::string header_name, std::string value);

    const bool is_message_complete() const { return header_complete && body_complete; };
    const std::string& getMethod() const { return method; };
    const std::string& getPath() const { return path; };
    const std::string& getStatusCode() const { return status_code; };
    const std::string& getStatusPhrase() const { return status_phrase; };
    const std::string& getVersion() const { return version; };
    const std::map<std::string, std::string>& getHeaders() const { return headers; };
};

#endif // HTTP_MESSAGE_H