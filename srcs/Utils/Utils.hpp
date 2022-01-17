#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <sys/time.h>
#include <sys/stat.h>

void split(const std::string& str, std::vector<std::string>& tokens, const std::string &delimiters);
std::string codePhrase(const int &code);
std::string getDate(void);
void setMimeTypes(void);
std::string itos(int n);
long getFileSize(std::string filename);
std::string getMimeType(std::string ext);
