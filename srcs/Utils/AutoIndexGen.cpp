#include "AutoIndexGen.hpp"

AutoIndexGen::AutoIndexGen(const std::string &path) : _path(path)
{
    struct dirent *entry;
    DIR *dir = opendir(path.c_str());
    struct stat st;
    this->_output += "<!DOCTYPE html><html><head><title>" + path + "</title></head>";
    this->_output += "<body><h1>Autoindex, path = " + path + "</h1>";

    if (!dir)
    {
        LOG("Can't open " + path + " directory", ERROR, 0);
        return;
    }
    this->_output += "<ul>";
    while ((entry = readdir(dir)))
    {
        if (stat(entry->d_name, &st) != 0)
            LOG("Stat err", ERROR, 0);
        if (strcmp(entry->d_name, "..") && strcmp(entry->d_name, "."))
            this->_output += "<li><a href=\"" + std::string(entry->d_name) + ((st.st_mode & S_IFDIR) ? "/" : "") + "\">" + entry->d_name + "</a></li>";
        memset(&st, 0, sizeof(st));
    }
        
    this->_output += "</ul></body>";
    closedir(dir);
}

std::string AutoIndexGen::getOutput(void) const
{
    return this->_output;
}
