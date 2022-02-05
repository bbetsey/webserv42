#pragma once

#include <iostream>
#include <dirent.h>
#include "../Includes/webserv.hpp"
#include "Logger.hpp"

class AutoIndexGen
{
    public:
        AutoIndexGen(const std::string &path);

        std::string getOutput(void) const;
    private:
        std::string _path;
        std::string _output;
};
