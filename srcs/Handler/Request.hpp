#pragma once

#include "Cgi.hpp"
#include "../Utils/Utils.hpp"
#include "../Utils/Uri.hpp"
#include "../Config/Config.hpp"
#include "../Includes/webserv.hpp"

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <fstream>

#define CRLF "\r\n"

class Request
{
    public:
        Request(const ServerConfig &cfg);
        ~Request(void);

        std::string getResponse(void);
        void add_msg(const std::string &msg);
        bool isReady(void) const;

        const std::string &getMethod(void) const;
        const Uri &getUri(void) const;
        const std::string &getBody(void) const;
        const ServerConfig &getConfig(void) const;
        std::map<std::string, std::string> &getHeaders(void);

    private:
        const ServerConfig &_cfg;

        std::string _method;
        Uri _uri;
        std::string _httpVersion;
        std::string _body;
        std::map<std::string, std::string> _headers;

        void parseFirstLine(std::string str);
        void parseHeaders(std::vector<std::string> lines);
        void parseBody(std::vector<std::string> lines, size_t i);

        std::string readFile(void);

};
