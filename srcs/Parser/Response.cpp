#include "Response.hpp"

Response::Response(const Request &req) : _req(req)
{
    if (req.getMethod() == "GET")
        this->getMethod();
    else
        this->errMethod();

    this->_headers["Date"] = getDate();
    this->_headers["Server"] = "webserv42";
}

void Response::getMethod(void)
{
    std::string path = this->_req.getUri();
    std::string tmp;
    std::ifstream file(path.c_str());

    if (!file.is_open())
    {
        if (access(path.c_str(), F_OK) == -1)
            this->_code = 404;
        else
            this->_code = 403;
    }

    while (std::getline(file, tmp))
        this->_body += tmp;

    this->_headers["Content-Length"] = itos(this->_body.length());
    this->_headers["Content-Type"] = "text/html";
    file.close();
    this->_code = 200;
}

void Response::errMethod(void)
{
    this->_code = 501;
}

std::string Response::toStr(void) const
{
    std::string ret("HTTP/1.1");

    ret += " " + itos(this->_code) + " " + codePhrase(this->_code) + CRLF;
    for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); it++)
        ret += it->first + ": " + it->second + CRLF;
    ret += CRLF;
    ret += this->_body;

    return ret;
}