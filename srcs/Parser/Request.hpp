#pragma once

#include <map>
#include <string>

#define CRLF "\r\n"

class Request
{
    public:
        Request(const std::string &str);
        ~Request(void);

    private:
        std::string _method;
        std::string _body;
        std::map<std::string, std::string> _headers;
};
