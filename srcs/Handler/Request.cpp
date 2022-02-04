#include "Request.hpp"

Request::Request(const ServerConfig cfg) : _cfg(cfg), _isReady(0), _headerWasRead(0), _contentLength(0), _reqHeaderEndPos(0)
{
}

Request::~Request(void)
{
}

void Request::add_msg(const std::string &msg)
{
    this->_reqWhole += msg;

    if (!this->_headerWasRead && (this->_reqHeaderEndPos = hasDoubleCRLF(this->_reqWhole)))
    {
        this->_headerWasRead = 1;
        this->_contentLength = getContentLength(this->_reqWhole);
    }
    if (this->_headerWasRead)
    {
        if (this->_reqWhole.length() - this->_reqHeaderEndPos + 2 >= this->_contentLength)
            this->_isReady = 1;
    }
}

bool Request::isReady(void) const
{
    return (this->_isReady);
}

void Request::parseFirstLine()
{
    std::string firstLine = this->_reqWhole.substr(0, this->_reqWhole.find_first_of('\r') - 1);
    std::vector<std::string> tokens;
    split(firstLine, tokens, " ");

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
        LOG("Wrong first line!", ERROR, 0);
        this->_parseStatus = 0;
    }
}

void Request::parseHeaders()
{
    this->_reqHeader = this->_reqWhole.substr(0, this->_reqHeaderEndPos + 2);
    std::vector<std::string> lines;
    split(this->_reqHeader, lines, "\r\n");

    if (lines.size() > 1)
    {
        for (size_t i = 1; i < lines.size(); i++)
        {
            if (lines[i] == CRLF)
                break;
            size_t pos = lines[i].find_first_of(':');
            this->_reqHeaders[lines[i].substr(0, pos)] = lines[i].substr(pos + 2, lines[i].length());
        }
    }
}

void Request::parseBody(void)
{
    this->_reqBody = this->_reqWhole.substr(this->_reqHeaderEndPos + 2, this->_reqWhole.length());
}

void Request::parse(void)
{
    this->_parseStatus = 1;
    this->parseFirstLine();
    this->parseHeaders();
    this->parseBody();
}

void Request::genHeader(std::string path)
{
    this->_resHeader += "HTTP/1.1 " + itos(this->_cgiStatus) + " " + getStatusName(this->_cgiStatus) + CRLF;
    this->_resHeader += "Date: " + getDate() + CRLF;
    this->_resHeader += "Server: " + this->_cfg.name + CRLF;
    this->_resHeader += "Content-Length: " + itos(this->_resBody.size()) + CRLF;
    this->_resHeader += "Content-Type: " + (this->_cgiStatus == 200 ? this->_cgiType : "text/html") + CRLF;
    if (path.length() > 0)
    {
        this->_resHeader += "Last-Modified: " + getLastModified(this->_uri._path) + CRLF;
    }

    this->_resHeader += CRLF;
}

std::string Request::handleGet(void)
{
    this->_resBody = readFile(this->_uri._path);
    this->genHeader(this->_uri._path);\

    return (this->_resHeader + this->_resBody);
}

std::string Request::handleErr(void)
{
    this->_resBody = "<!DOCTYPE html>\n<html><title>500</title><body><h1>yo wtf u did?</h1></body></html>";
    this->genHeader("");

    return (this->_resHeader + this->_resBody);
}

void Request::parseCgiResponse(void)
{
    std::vector<std::string> lines;
    size_t tmpPos;

    split(this->_cgiResponse, lines, "\r\n");
    for (size_t i = 0; i < lines.size(); i++)
    {
        if ((tmpPos = lines[i].find("Status: ")) != std::string::npos)
            this->_cgiStatus = atoi(lines[i].substr(tmpPos + 7, lines[i].length()).c_str());
        if ((tmpPos = lines[i].find("Content-type: ")) != std::string::npos)
            this->_cgiType = lines[i].substr(tmpPos + 13, lines[i].length());
    }
}

std::string Request::getResponse(void)
{
    this->parse();

    this->_isReady = 0;
    this->_reqWhole = "";
    this->_headerWasRead = 0;
    this->_contentLength = 0;
    this->_reqHeaderEndPos = 0;

    if (!this->_parseStatus)
        return (this->handleErr());

    this->_cgiResponse = Cgi(*this).execute();

    this->parseCgiResponse();
    if (this->_cgiStatus == 500)
        return (this->handleErr());

    if (this->_method == "GET")
        return (this->handleGet());


    return (this->handleErr());
}

const std::string &Request::getMethod(void) const { return this->_method; }
const Uri &Request::getUri(void) const { return this->_uri; }
const std::string &Request::getBody(void) const { return this->_reqBody; }
std::map<std::string, std::string> &Request::getHeaders(void) { return this->_reqHeaders; }
const ServerConfig &Request::getConfig(void) const {return this->_cfg; }
