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

#define CGI_BUFSIZE 65536

class Request;

class Cgi
{
    public:
        Cgi(Request &req);

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
