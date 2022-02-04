#pragma once

#include "Cgi.hpp"
#include "../Utils/Utils.hpp"
#include "../Utils/Uri.hpp"
#include "../Config/Config.hpp"
#include "../Includes/webserv.hpp"

#include <iostream>
#include <map>
#include <string>

#define CRLF "\r\n"

class Request
{
    public:
        Request(const ServerConfig cfg);
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
        // Config
        const ServerConfig _cfg;

        // Request irst line arguments
        std::string _method;
        Uri _uri;
        std::string _httpVersion;

        // Request parts
        std::string _reqWhole;
        std::string _reqBody;
        std::string _reqHeader;
        std::map<std::string, std::string> _reqHeaders;
        

        // Response parts
        std::string _resHeader;
        std::string _resBody;

        // Util vars
        bool _isReady;
        bool _parseStatus;

        bool _headerWasRead;
        size_t _contentLength;
        size_t _reqHeaderEndPos;

        std::string _cgiResponse;
        int _cgiStatus;
        std::string _cgiType;

        // Parser
        void parse(void);
        void parseFirstLine(void);
        void parseHeaders(void);
        void parseBody(void);
        void parseCgiResponse(void);

        // Handlers
        void genHeader(std::string path);
        std::string handleGet(void);
        std::string handleErr(void);
};
