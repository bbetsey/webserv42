#include "Request.hpp"

Request::Request(const std::string &str, const ServerConfig &cfg) : _cfg(cfg)
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
        this->_uri = Uri(tokens.at(1));
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

static std::string readFile(std::string &path)
{
    std::ifstream t((path.substr(1, path.length())).c_str());
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

std::string Request::mGet(const std::string &header, const int &status)
{
    std::string body;

    if (status == 200)
        return header + readFile(this->_uri._path);
    return header;
}

std::string Request::response(void)
{
    std::string header = Cgi(*this).execute();
    int status = atoi(header.substr(8, 3).c_str());

    if (this->_method == "GET")
        return this->mGet(header, status);

    return header;
}


const std::string &Request::getMethod(void) const { return this->_method; }
const Uri &Request::getUri(void) const { return this->_uri; }
const std::string &Request::getBody(void) const { return this->_body; }
std::map<std::string, std::string> &Request::getHeaders(void) { return this->_headers; }
const ServerConfig &Request::getConfig(void) const {return this->_cfg; }
