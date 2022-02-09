#pragma once

#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#include "../Includes/webserv.hpp"
#include "../Utils/Utils.hpp"
#include "Logger.hpp"

class AutoIndexGen
{
    public:
        AutoIndexGen(const std::string &path, const std::string &ori_path);

        std::string getOutput(void) const;
    private:
        std::string _path;
        std::string _output;
};
