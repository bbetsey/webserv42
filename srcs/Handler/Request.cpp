#include "Request.hpp"

Request::Request(const ServerConfig &cfg) : _cfg(cfg)
{
}

Request::~Request(void)
{
}

void Request::add_msg(const std::string &msg)
{
    (void)msg;
}

bool Request::isReady(void) const
{
    return 1;
}

void Request::parseFirstLine(std::string line)
{
    std::vector<std::string> tokens;
    split(line, tokens, " ");

    try
    {
        this->_method = tokens.at(0);
        this->_uri = Uri(tokens.at(1));
        if (this->_uri._path == "/")
            this->_uri._path = "/index.html";
        this->_httpVersion = tokens.at(2);
    }
    catch (std::exception &e)
    {
        std::cout << "Wrong first line! " << e.what() << std::endl;
        LOG("Wrong first line!", ERROR, 0);
    }
}

void Request::parseHeaders(std::vector<std::string> lines)
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

void Request::parseBody(std::vector<std::string> lines, size_t i)
{
    if (lines.size() <= i + 1)
        return;
    for (i = i + 1; i < lines.size(); i++)
        this->_body += lines[i];
}

std::string Request::readFile(void)
{
    std::ifstream file((this->_uri._path.substr(1, this->_uri._path.length())).c_str());
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string Request::getResponse(void)
{
    std::vector<std::string> lines;
    split(this->_body, lines, "\r\n");

    try
    {
        this->parseFirstLine(lines.at(0));
        this->parseHeaders(lines);
    }
    catch (std::exception &e)
    {
        LOG("Looks like we got only one line", ERROR, 0);
    }

    return ("wtf");
}

const std::string &Request::getMethod(void) const { return this->_method; }
const Uri &Request::getUri(void) const { return this->_uri; }
const std::string &Request::getBody(void) const { return this->_body; }
std::map<std::string, std::string> &Request::getHeaders(void) { return this->_headers; }
const ServerConfig &Request::getConfig(void) const {return this->_cfg; }
