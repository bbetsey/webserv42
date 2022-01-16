#pragma once

#include "../Utils/Utils.hpp"
#include <iostream>
#include <map>
#include <string>

#define CRLF "\r\n"

class Request
{
    public:
        Request(std::string &str);
        ~Request(void);

        std::string debug(void);
    
    private:
        std::string _method;
        std::string _uri;
        std::string _httpVersion;
        std::string _body;
        std::map<std::string, std::string> _headers;

        void parseFirstLine(std::string &str);
        void parseHeaders(std::vector<std::string> &lines);
};

std::ostream& operator<<(std::ostream &os, Request &req);
