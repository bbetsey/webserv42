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
    if (path == "/")
        path = "index.html";
    std::string tmp;
    std::ifstream file(path.c_str());

    if (!file.is_open())
    {
        if (access(path.c_str(), F_OK) == -1)
            this->_code = 404;
        else
            this->_code = 403;
        return;
    }

    while (std::getline(file, tmp))
        this->_body += tmp;

    this->_headers["Content-Length"] = itos(this->_body.length());
    this->_headers["Content-Type"] = getMimeType(path.substr(path.find_last_of('.') + 1, path.length()));
    this->_headers["Last-Modified"] = getLastModified(path);
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
