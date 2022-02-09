#pragma once

#include "Request.hpp"
#include "../Utils/Uri.hpp"
#include "../Config/Config.hpp"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <map>

class Request;

class Cgi
{
    public:
        Cgi(Request &req, const std::string &cgiPath);

        std::string execute(void);
    private:
        std::map<std::string, std::string> _env;

        const std::string &_cgiPath;
        Request &_req;
        const std::string &_method;
        const Uri &_uri;
        const std::string &_body;
        std::map<std::string, std::string> &_headers;
        const ServerConfig &_cfg;
};
