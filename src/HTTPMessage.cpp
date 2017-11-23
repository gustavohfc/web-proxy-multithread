/*!
 * \file HTTPMessage.cpp
 * \author Ismael Coelho Medeiros
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <string>
#include <vector>
#include <map>

#include "HTTPMessage.h"

HTTPMessage::HTTPMessage(const std::string method, const std::string& url, const std::string& version, 
                         const std::map<std::string, std::string>& headers, char *body)
    : type(REQUEST), url(url), method(method), version(version), headers(headers), body(body)
{
}


HTTPMessage::HTTPMessage(const int status_code, const std::string& status_phrase, const std::string& version, 
                         const std::map<std::string, std::string>& headers, char *body)
    : type(RESPONSE), status_code(status_code), status_phrase(status_phrase), version(version), headers(headers), body(body)
{
}


HTTPMessage::HTTPMessage(HTTPMessageType type)
    : type(type), body(nullptr)
{
}


HTTPMessage::~HTTPMessage()
{
    delete[] body;
}


std::string HTTPMessage::toMessage() const
{
    std::string message;

    if (type == REQUEST)
    {
        message.append(method + " " + url + " " + version + "\r\n");
    }
    else
    {
        message.append(version + " " + std::to_string(status_code) + " " + status_phrase + "\r\n");
    }

    for (auto i : headers)
    {
        message.append(i.first + ": " + i.second + "\r\n");
    }

    message.append("\r\n");
    message.append(body);

    return message;
}