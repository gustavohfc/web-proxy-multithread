/*!
 * \file HTTP-response.cpp
 * \author Ismael Coelho Medeiros
 */

#include <string>
#include <vector>
#include <map>

#include "HTTP-response.h"
#include "debug.h"

HTTPResponse::HTTPResponse(const std::string& message) 
{
    // Get end position of the headers
    size_t headersEnd = message.find("\r\n\r\n");
    
    // Read header
    std::vector<std::string> lines;
    std::string delimiter = "\r\n";
    size_t prev = 0, pos = 0;
    do
    {
        pos = message.find(delimiter, prev);
        if (pos == headersEnd) { pos = headersEnd; }
        lines.push_back(message.substr(prev, pos - prev));
        prev = pos + delimiter.size();
    }
    while (pos < headersEnd && prev < headersEnd);

    // Read protocol field
    size_t firstSpace = lines[0].find(" ", 0);
    this->protocol = lines[0].substr(0, firstSpace);
    PRINT_DEBUG("----------------------------------------------------\n");
    PRINT_DEBUG("%s: Parsing request message:\n", __PRETTY_FUNCTION__);
    PRINT_DEBUG("Protocol: %s\n", this->protocol.c_str());

    // Read status code field
    size_t secondSpace = lines[0].find(" ", firstSpace + 1);
    this->status_code = stoi( lines[0].substr(firstSpace + 1, secondSpace - firstSpace - 1)  );
    PRINT_DEBUG("Status Code: %d\n", this->status_code);

    // Read status phrase  field
    this->status_phrase = lines[0].substr(secondSpace + 1, lines[0].size());
    PRINT_DEBUG("Status Phrase: %s\n", this->status_phrase.c_str());

    // Read headers
    std::vector<std::string>::iterator it = lines.begin() + 1;
    this->headers = std::map<std::string, std::string>();
    PRINT_DEBUG("Headers:\n");
    for (; it != lines.end(); ++it)
    {
        // Find separator postion
        size_t separatorPos = (*it).find(":");
        // Get position of the value, eliminating the spaces
        size_t start = separatorPos + 1;
        for (; (*it).at(start) == ' '; ++start);
        // Store header (Key => header's key, Value = header's value)
        this->headers[(*it).substr(0, separatorPos)] = (*it).substr(start, (*it).size());
        PRINT_DEBUG("  %s => %s\n", (*it).substr(0, separatorPos).c_str(), (*it).substr(start, (*it).size()).c_str());
    }

    // Read body
    if (headersEnd + sizeof("\r\n\r\n") >= std::string::npos)
    {
        this->body = message.substr(headersEnd + sizeof("\r\n\r\n"), message.size() - headersEnd - sizeof("\r\n\r\n"));
        PRINT_DEBUG("Body: %s\n", this->body.c_str());
    }
    PRINT_DEBUG("----------------------------------------------------\n");
}

HTTPResponse::HTTPResponse(
    const std::string& protocol,
    int status_code,
    const std::string& status_phrase,
    const std::map<std::string, std::string>& headers,
    const std::string& body
) {
    this->protocol = protocol;
    this->status_code = status_code;
    this->status_phrase = status_phrase;
    this->headers = headers;
    this->body = body;
}

std::string HTTPResponse::toMessage()
{
    std::string message;
    message.append(protocol + " " + std::to_string(status_code) + " " + status_phrase + "\r\n");
    std::map<std::string, std::string>::iterator it = headers.begin();
    for (; it != headers.end(); ++it)
    {
        message.append(it->first + ": " + it->second + "\r\n");
    }
    message.append("\r\n");
    message.append(body);
    return message;
}