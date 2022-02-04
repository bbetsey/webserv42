#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <sys/time.h>
#include <sys/stat.h>
#include <sstream>
#include <fstream>

void split(std::string str, std::vector<std::string> &tokens, std::string delimiters);
std::string getDate(void);
std::string getLastModified(std::string path);
void setMimeTypes(void);
std::string itos(const int &n);
long getFileSize(const std::string &filename);
std::string getMimeType(const std::string &ext);
size_t getContentLength(const std::string &msg);
size_t hasDoubleCRLF(const std::string &msg);
std::string readFile(std::string &path);
std::string getStatusName(const int &status);
