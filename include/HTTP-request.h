/*!
 * \file HTTP-request.h
 * \author Ismael Coelho Medeiros
 */

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <map>

class HTTPRequest
{
private:
    std::string method;
    std::string url;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;

public:
    HTTPRequest(const std::string& message);
    HTTPRequest(
        const std::string method,
        const std::string& url,
        const std::string& version,
        const std::map<std::string, std::string>& headers,
        const std::string& body
    );

    std::string toMessage();

    const std::string& getMethod() const { return method; };
    const std::string& getUrl() const { return url; };
    const std::string& getVersion() const { return version; };
    const std::map<std::string, std::string>& getHeaders() const { return headers; };
    const std::string& getBody() const { return body; };
};


#endif // HTTP_REQUEST_H