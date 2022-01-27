#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <sys/time.h>
#include <sys/stat.h>

void split(const std::string &str, std::vector<std::string> &tokens, const std::string &delimiters);
std::string getDate(void);
std::string getLastModified(std::string path);
void setMimeTypes(void);
std::string itos(const int &n);
long getFileSize(const std::string &filename);
std::string getMimeType(const std::string &ext);
