#pragma once

#include "../Utils/Utils.hpp"
#include "Response.hpp"
#include <iostream>
#include <map>
#include <string>

class Request
{
    public:
        Request(std::string &str);
        ~Request(void);

        std::string debug(void);
        std::string response(void);

        const std::string getMethod(void) const;
        const std::string getUri(void) const;
        const std::string getBody(void) const;
        const std::map<std::string, std::string> getHeaders(void) const;
    private:
        std::string _method;
        std::string _uri;
        std::string _httpVersion;
        std::string _body;
        std::map<std::string, std::string> _headers;

        void parseFirstLine(std::string &str);
        void parseHeaders(std::vector<std::string> &lines);
};
