#include "Cgi.hpp"

// https://www6.uniovi.es/~antonio/ncsa_httpd/cgi/env.html

Cgi::Cgi(Request &req, std::string &cgi_path) : _req(req), _method(req.getMethod()), _uri(req.getUri()), _body(req.getBody()), _headers(req.getHeaders())
{
    this->_env["SERVER_SOFTWARE"] = "webserv/0.1";
    this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
    this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_env["SERVER_PORT"] = "80"; // CHANGE TO CFG!
    this->_env["REQUEST_METHOD"] = this->_req.getMethod();
    this->_env["PATH_INFO"] = this->_uri._path;
    this->_env["PATH_TRANSLATED"] = this->_uri._path;
    this->_env["SCRIPT_NAME"] = ""; // CHANGE TO CFG!
    this->_env["QUERY_STRING"] = this->_uri._qString;
    this->_env["REMOTE_ADDR"] = "127.0.0.1"; //CHANGE TO CFG!

    if (this->_headers.find("Hostname") != this->_headers.end())
        this->_env["SERVER_NAME"] = this->_headers["Hostname"];
    else
        this->_env["SERVER_NAME"] = this->_env["REMOTE_ADDR"];

    if (this->_headers.find("Auth-Scheme") != this->_headers.end())
    {
        this->_env["AUTH_TYPE"] = this->_headers["Authorization"];
        this->_env["REMOTE_USER"] = this->_headers["Authorization"];
        this->_env["REMOTE_IDENT"] = this->_headers["Authorization"];
    }

    this->_env["CONTENT_TYPE"] = this->_headers["Content-Type"];
    this->_env["CONTENT_LENGTH"] = itos(this->_req.getBody().length());
}

std::string Cgi::execute(void)
{

}