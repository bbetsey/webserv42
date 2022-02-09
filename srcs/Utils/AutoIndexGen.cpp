#include "AutoIndexGen.hpp"

AutoIndexGen::AutoIndexGen(const std::string &path, const std::string &ori_path) : _path(path)
{
    struct dirent *entry;
    DIR *dir = opendir(path.c_str());
    this->_output += "<!DOCTYPE html><html><head><title>" + path + "</title></head>";
    this->_output += "<body><h1>Autoindex</h1>";

    if (!dir)
    {
        LOG("Can't open " + path + " directory", ERROR, 0);
        return;
    }
    this->_output += "<ul>";
    while ((entry = readdir(dir)))
        if (strcmp(entry->d_name, "..") && strcmp(entry->d_name, "."))
            this->_output += "<li><a href=\"" + ori_path + (ori_path[ori_path.length() - 1] == '/' ? "" : "/") + std::string(entry->d_name) + "\">" + entry->d_name + "</a></li>";

    this->_output += "</ul></body>";
    closedir(dir);
}

std::string AutoIndexGen::getOutput(void) const
{
    return this->_output;
}
