#pragma once

#include "Request.hpp"

class Cgi
{
    public:
        Cgi(Request &req, std::string &cgi_path);

        std::string execute(void);
    private:
        std::map<std::string, std::string> _env;

        Request &_req;
        const std::string &_method;
        const Uri &_uri;
        const std::string &_body;
        std::map<std::string, std::string> &_headers;
        const ServerConfig &_cfg;
};
