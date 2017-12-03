/*!
 * \file HTTPMessage.cpp
 * \author Ismael Coelho Medeiros
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <string.h>
#include <string>
#include <vector>
#include <map>

#include "HTTPMessage.h"

HTTPMessage::HTTPMessage(const std::string method, const std::string& path, const std::string& version, 
                         const std::map<std::string, std::string>& headers, char *body)
    : header_complete(true), body_complete(true), type(REQUEST), path(path), method(method), version(version), headers(headers)
{
}


HTTPMessage::HTTPMessage(const int status_code, const std::string& status_phrase, const std::string& version, 
                         const std::map<std::string, std::string>& headers, char *body)
    : header_complete(true), body_complete(true), type(RESPONSE), status_code(status_code), status_phrase(status_phrase), version(version), headers(headers)
{
}


HTTPMessage::HTTPMessage(HTTPMessageType type)
    : header_complete(false), body_complete(false), type(type)
{
}


HTTPMessage::~HTTPMessage()
{
}


ConnectionStatus HTTPMessage::addMessageData(const char *buffer, int n_bytes)
{
    body.insert(body.end(), buffer, buffer + n_bytes);

    // Read header
    if (!header_complete && strstr(&body[0], "\r\n\r\n") != NULL)
    {
        parseHeaders();
        header_complete = true;
    }

    // Read the body
    if (header_complete)
    {
        if (headers.find("Content-Length") == headers.end() || stoi(headers.find("Content-Length")->second) == getBodyLength())
        {
            body_complete = true;
            return OK;
        }
    }

    return OK;
}


void HTTPMessage::parseHeaders()
{
    const char* next_line;

    if (type == RESPONSE)
    {
        // Read version field
        const char* first_space = strchr(&body[0], ' ');
        version = std::string(&body[0], first_space - &body[0]);

        // Read status code field
        const char* second_space = strchr(first_space + 1, ' ');
        status_code = stoi( std::string(first_space + 1, second_space - first_space - 1) );

        // Read status phrase  field
        const char* line_end = strchr(second_space + 1, '\r');
        status_phrase = std::string(second_space + 1, line_end - second_space - 1);
        
        next_line = line_end + 2;
    }
    else
    {
        // Read method field
        const char* first_space = strchr(&body[0], ' ');
        method = std::string(&body[0], first_space - &body[0]);

        // Read path field
        const char* second_space = strchr(first_space + 1, ' ');
        path = std::string(first_space + 1, second_space - first_space - 1);

        // Read version field
        const char* line_end = strchr(second_space + 1, '\r');
        version = std::string(second_space + 1, line_end - second_space - 1);
        
        next_line = line_end + 2;
    }

    while (*next_line != '\r')
    {
        const char* colon_position = strchr(next_line, ':');

        const char* value_position = colon_position + 1;
        // Jump white spaces
        for(; *value_position == ' '; value_position++);

        const char* line_end = strchr(colon_position, '\r');

        headers[ std::string(next_line, colon_position - next_line) ] = std::string(value_position, line_end - value_position);

        next_line = line_end + 2;
    }

    // Remove the header from the message body
    uint body_offset = strstr(&body[0], "\r\n\r\n") - &body[0] + 4;
    body.erase(body.begin(), body.begin() + body_offset);
}


const std::vector<char> HTTPMessage::getMessage() const
{
    // TODO: mount message
    // return &message[0];
    return std::vector<char>();
}


const std::vector<char> & HTTPMessage::getBody() const
{
    return body;
}


int HTTPMessage::getBodyLength() const
{
    return body.size();
}


std::string HTTPMessage::getHost() const
{
    if (headers.find("Host") != headers.end())
    {
        return headers.find("Host")->second;
    }

    size_t host_start = path.find("www.") + 4;
    if (path.find("www.") != std::string::npos)
    {
        size_t host_end = path.find("/") - 1;

        return path.substr(host_start, host_end);
    }

    return "";
}
