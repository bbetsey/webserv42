#include "Request.hpp"

Request::Request(std::string &str)
{
    std::vector<std::string> lines;
    split(str, lines, "\r\n");

    this->parseFirstLine(lines[0]);
    this->parseHeaders(lines);
}

Request::~Request(void)
{
}

void Request::parseFirstLine(std::string &line)
{
    std::vector<std::string> tokens;
    split(line, tokens, " ");

    try
    {
        this->_method = tokens.at(0);
        this->_uri = tokens.at(1);
        this->_httpVersion = tokens.at(2);
    }
    catch (std::exception &e)
    {
        std::cout << "Wrong first line! " << e.what() << std::endl; 
    }
}

void Request::parseHeaders(std::vector<std::string> &lines)
{
    size_t i;
    for (i = 1; i < lines.size(); i++)
    {
        if (lines[i] == CRLF)
            break;
        size_t pos = lines[i].find_first_of(':');
        this->_headers[lines[i].substr(0, pos)] = lines[i].substr(pos + 2, lines[i].length());
    }
    this->parseBody(lines, i);
}

void Request::parseBody(std::vector<std::string> &lines, size_t &i)
{
    if (lines.size() <= i + 1)
        return;
    for (i = i + 1; i < lines.size(); i++)
        this->_body += lines[i];
}

std::string Request::response(void) { return Response(*this).toStr(); }
const std::string &Request::getMethod(void) const { return this->_method; }
std::string Request::getUri(void) const { return this->_uri; }
const std::string &Request::getBody(void) const { return this->_body; }
const std::map<std::string, std::string> &Request::getHeaders(void) const { return this->_headers; }
