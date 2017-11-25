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

enum ConnectionStatus {OK, INVALID_REQUEST, TIMEOUT, FILTER_BLOCKED, CANNOT_SEND_MESSAGE_TO_HOST, CANNOT_CONNECT_TO_HOST};

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
    int status_code;
    std::string status_phrase;

    std::string version;

    std::map<std::string, std::string> headers;
    std::vector<char> message;

public:
    // Request constructors
    HTTPMessage(const std::string method, const std::string& url, const std::string& version, 
                const std::map<std::string, std::string>& headers, char *body);

    // Response constructors
    HTTPMessage(const int status_code, const std::string& status_phrase, const std::string& version, 
                const std::map<std::string, std::string>& headers, char *body);

    HTTPMessage(HTTPMessageType type);

    ~HTTPMessage();

    ConnectionStatus addMessageData(const char *buffer, int n_bytes);
    void parseHeaders();
    const char * getMessage() const;
    int getMessageLength() const;
    const char * getBody() const;
    int getBodyLength() const;
    std::string getHost() const;

    const bool is_message_complete() const { return header_complete && body_complete; };
    const std::string& getMethod() const { return method; };
    const std::string& getPath() const { return path; };
    const int getStatusCode() const { return status_code; };
    const std::string& getStatusPhrase() const { return status_phrase; };
    const std::string& getVersion() const { return version; };
    const std::map<std::string, std::string>& getHeaders() const { return headers; };
};

#endif // HTTP_MESSAGE_H