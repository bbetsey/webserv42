#include "Request.hpp"

Request::Request(std::string str, ServerConfig cfg) : _cfg(cfg)
{
    std::vector<std::string> lines;
    split(str, lines, "\r\n");

    try
    {
        this->parseFirstLine(lines.at(0));
        this->parseHeaders(lines);
    }
    catch (std::exception &e)
    {
        LOG("Looks like we got only one line", ERROR);
    }
}

Request::~Request(void)
{
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
        LOG("Wrong first line!", ERROR);
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

std::string Request::genGetBody(void)
{
    std::string cgiResponse = Cgi(*this).execute();
    std::string response;
    int status = atoi(cgiResponse.substr(8, 3).c_str());

    if (status == 200)
        response = this->readFile();
    else
        response = "<!DOCTYPE html>\n<html><title>Ooops</title><body><h1>Look like your request was an error. Or ur life?</h1></body></html>\n";

    return (this->genResponse(cgiResponse, response, status));
}

std::string Request::genResponse(std::string header, std::string body, int &status)
{
    header = header.substr(0, header.length() - 4);
    std::string ret = "HTTP/1.1 " + itos(status) + " OK" + CRLF
                    + "Content-Length: " + itos(body.length()) + CRLF
                    + "Date: " + getDate() + CRLF
                    + "Server: webserv/0.1\r\n"
    //header += "Content-Type: " + getMimeType(this->_uri._path.substr(this->_uri._path.find_last_of('.') + 1, this->_uri._path.length())) + CRLF;
                    + "Last-Modified: " + getLastModified(this->_uri._path) + CRLF + CRLF
                    + body;

    return (ret);
}

std::string Request::response(void)
{
    if (this->_method == "GET")
        return (this->genGetBody());
    return ("wtf");
}

const std::string &Request::getMethod(void) const { return this->_method; }
const Uri &Request::getUri(void) const { return this->_uri; }
const std::string &Request::getBody(void) const { return this->_body; }
std::map<std::string, std::string> &Request::getHeaders(void) { return this->_headers; }
const ServerConfig &Request::getConfig(void) const {return this->_cfg; }
