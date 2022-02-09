#pragma once

#include "Cgi.hpp"
#include "../Utils/Utils.hpp"
#include "../Utils/Uri.hpp"
#include "../Config/Config.hpp"
#include "../Includes/webserv.hpp"
#include "../Utils/AutoIndexGen.hpp"

#include <iostream>
#include <map>
#include <string>
#include <sys/stat.h>

#define CRLF "\r\n"

enum ResponseType { REGFILE, PLAINHTML, NOTHING };

class Request
{
    public:
        Request(const ServerConfig cfg);
        ~Request(void);

        std::string getResponse(void);
        void add_msg(const std::string &msg);
        bool isReady(void) const;
        void checkIfChunked(void);

        const std::string &getMethod(void) const;
        const Uri &getUri(void) const;
        const std::string &getBody(void) const;
        const ServerConfig &getConfig(void) const;
        std::map<std::string, std::string> &getHeaders(void);

    private:
        // Config
        ServerConfig _cfg;
        Location    _loc;

        // Request irst line arguments
        std::string _method;
        Uri _uri;
        std::string _httpVersion;
        std::string _ori_path;

        // Request parts
        std::string _reqWhole;
        std::string _reqBody;
        std::string _reqHeader;
        std::map<std::string, std::string> _reqHeaders;
        

        // Response parts
        std::string _resHeader;
        std::string _resBody;
        int _resStatus;

        // Util vars
        bool _isChunked;
        bool _isReady;
        bool _parseStatus;

        bool _headerWasRead;
        size_t _contentLength;
        size_t _reqHeaderEndPos;

        std::string _cgiResponse;
        int _cgiStatus;
        std::string _cgiType;

        ResponseType _resType;

        // char _pathToReturn[100];

        bool _locWasFound;

        // Parser
        void parse(void);
        void formatPath(void);
        void parseFirstLine(void);
        void parseHeaders(void);
        void parseBody(void);
        void parseCgiResponse(void);

        // Handlers
        void genHeader(void);
        std::string handleGet(void);
        std::string handlePost(void);
        std::string handleHead(void);
        std::string handleDelete(void);
        std::string handlePut(void);
        std::string handleErr(const std::string &err);

        bool readContent(const std::string &path);

};
