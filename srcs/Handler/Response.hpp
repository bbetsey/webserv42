#pragma once

#include "../Utils/Utils.hpp"
#include "Request.hpp"
#include <map>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>

#define CRLF "\r\n"

class Request;

class Response
{
    public:
        Response(const Request &req);

        std::string toStr(void) const;
    private:
        const Request &_req;
        int _code;
        std::map<std::string, std::string> _headers;
        std::string _body;

        void getMethod(void);
        void postMethod(void);
        void errMethod(void);
};
