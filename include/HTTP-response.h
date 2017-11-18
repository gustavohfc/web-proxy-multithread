/*!
 * \file HTTP-response.h
 * \author Ismael Coelho Medeiros
 */

#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <map>

class HTTPResponse
{
private:
    std::string protocol;
    int status_code;
    std::string status_phrase;
    std::map<std::string, std::string> headers;
    std::string body;

public:
    HTTPResponse(const std::string& message);
    HTTPResponse(
        const std::string& protocol,
        int status_code,
        const std::string& status_phrase,
        const std::map<std::string, std::string>& headers,
        const std::string& body
    );

    std::string toMessage();

    const std::string& getProtocol() const { return protocol; };
    const int getStatusCode() const { return status_code; };
    const std::string& getStatusPhrase() const { return status_phrase; };
    const std::map<std::string, std::string>& getHeaders() const { return headers; };
    const std::string& getBody() const { return body; };
};


#endif // HTTP_RESPONSE_H