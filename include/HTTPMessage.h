/*!
 * \file HTTPMessage.h
 * \author Ismael Coelho Medeiros
 * \author Gustavo Henrique Fernandes Carvalho
 */

#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

#include <string>
#include <map>

enum HTTPMessageType {REQUEST, RESPONSE, QWEASDZXC};

class HTTPMessage
{
private:
    HTTPMessageType type;

    // Used on request messages
    std::string url;
    std::string method;

    // Used on response messages
    int status_code;
    std::string status_phrase;

    std::string version;
    std::map<std::string, std::string> headers;
    char *body;

public:
    // Request constructors
    HTTPMessage(const std::string method, const std::string& url, const std::string& version, 
                const std::map<std::string, std::string>& headers, char *body);

    // Response constructors
    HTTPMessage(const int status_code, const std::string& status_phrase, const std::string& version, 
                const std::map<std::string, std::string>& headers, char *body);

    HTTPMessage(HTTPMessageType type);

    ~HTTPMessage();

    std::string toMessage() const;

    const std::string& getMethod() const { return method; };
    const std::string& getUrl() const { return url; };
    const std::string& getVersion() const { return version; };
    const std::map<std::string, std::string>& getHeaders() const { return headers; };
    const char * getBody() const { return body; };
};

#endif // HTTP_MESSAGE_H