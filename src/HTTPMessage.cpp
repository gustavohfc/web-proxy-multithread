/*!
 * \file HTTPMessage.cpp
 * \author Ismael Coelho Medeiros
 * \author Gustavo Henrique Fernandes Carvalho
 */

#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>

#include "HTTPMessage.h"

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
    body.push_back('\0');
    if (!header_complete && strstr(&body[0], "\r\n\r\n") != NULL)
    {
        parseHeaders();
        header_complete = true;
    }
    body.pop_back();

    // Read the body
    if (header_complete && !body_complete)
    {
        if (headers.find("Content-Length") == headers.end() || stoi(headers.find("Content-Length")->second) == getBodyLength())
        {
            body_complete = true;
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
        status_code = std::string(first_space + 1, second_space - first_space - 1);

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
    std::vector<char> message;

    if (type == RESPONSE)
    {
        // Write the version field
        std::copy(version.begin(), version.end(), std::back_inserter(message));
        message.push_back(' ');

        // Write the status code field
        std::copy(status_code.begin(), status_code.end(), std::back_inserter(message));
        message.push_back(' ');

        // Write the status phrase field
        std::copy(status_phrase.begin(), status_phrase.end(), std::back_inserter(message));
        message.push_back('\r');
        message.push_back('\n');
    }
    else
    {
        // Write the method field
        std::copy(method.begin(), method.end(), std::back_inserter(message));
        message.push_back(' ');

        // Write the path field
        std::copy(path.begin(), path.end(), std::back_inserter(message));
        message.push_back(' ');

        // Write the version field
        std::copy(version.begin(), version.end(), std::back_inserter(message));
        message.push_back('\r');
        message.push_back('\n');
    }

    // Write all headers
    for (auto const & header : headers)
    {
        std::copy(header.first.begin(), header.first.end(), std::back_inserter(message));

        message.push_back(':');
        message.push_back(' ');

        std::copy(header.second.begin(), header.second.end(), std::back_inserter(message));

        message.push_back('\r');
        message.push_back('\n');
    }

    // Write the message body
    message.push_back('\r');
    message.push_back('\n');

    std::copy(body.begin(), body.end(), std::back_inserter(message));

    return message;
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
