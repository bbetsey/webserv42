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

        std::string response(void)
        {
            if (this->_method == "GET")
                return this->getMethod();
            else if (this->_method == "POST")
                return this->postMethod();
            else if (this->_method == "DELETE")
                return this->deleteMethod();
            else
                return this->errMethod();
        }

    private:
        std::string _method;
        std::string _uri;
        std::string _httpVersion;
        std::string _body;
        std::map<std::string, std::string> _headers;

        void parseFirstLine(std::string &str);
        void parseHeaders(std::vector<std::string> &lines);

        std::string getMethod(void) const;
        std::string postMethod(void) const;
        std::string deleteMethod(void) const;
        std::string errMethod(void) const;
};
