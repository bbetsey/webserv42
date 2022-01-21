#pragma once

#include <string>
#include <algorithm>

class Uri
{
    public:
        Uri(void) {}
        Uri(std::string uri);
        std::string _qString, _path, _protocol, _host, _port;
};
