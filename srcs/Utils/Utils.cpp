#include "Utils.hpp"
#include "../Includes/webserv.hpp"

void split(std::string str, std::vector<std::string> &tokens, const std::string delimiters)
{
	size_t pos, lastPos = 0, length = str.length();

	while (lastPos < length + 1)
	{
		pos = str.find_first_of(delimiters, lastPos);
		if (pos == std::string::npos)
			pos = length;
		if (pos != lastPos)
			tokens.push_back(std::string(str.data() + lastPos, pos - lastPos));
		lastPos = pos + 1;
	}
}

std::string getDate(void)
{
    std::string     ret;
	char			buffer[100];
	struct timeval	tv;
	struct tm		*tm;

	gettimeofday(&tv, NULL);
	tm = gmtime(&tv.tv_sec);
	strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S GMT", tm);
    ret = std::string(buffer);
	return ret;
}

std::string itos(const int &n)
{
    std::stringstream out;
    std::string ret;
    
    out << n;
    ret = out.str();

    return ret;
}

long getFileSize(const std::string &filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

std::string getLastModified(std::string path)
{
	char			buffer[100];
	struct stat		stats;
	struct tm		*tm;
    std::string     ret;

	if (path[0] == '/')
		path = path.substr(1, path.length());
	if (stat(path.c_str(), &stats) == 0)
	{
		tm = gmtime(&stats.st_mtime);
		strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S GMT", tm);
		ret = std::string(buffer);
	}
    return ret;
}

std::string getMimeType(const std::string &ext)
{
    if (ext == "html")
		return "text/html";
	if (ext == "css")
		return "text/css";
	if (ext == "js")
		return "text/javascript";
	if (ext == "jpeg" || ext == "jpg")
		return "image/jpeg";
	if (ext == "png")
		return "image/png";
	if (ext == "bmp")
		return "image/bmp";
	return "text/plain";
}

size_t hasDoubleCRLF(const std::string &msg)
{
    for (size_t i = 0; i < msg.length() && i + 3 < msg.length(); i++)
        if (msg[i] == '\r' && msg[i + 1] == '\n' && msg[i + 2] == '\r' && msg[i + 3] == '\n')
            return (i);
    return (0);
}

size_t getContentLength(const std::string &msg)
{
    std::vector<std::string> lines;
    size_t tmp;

    split(msg, lines, "\r\n");
    for (size_t i = 0; i < lines.size(); i++)
        if ((tmp = lines[i].find("Content-Length: ")) != std::string::npos)
            return (atoi(lines[i].substr(tmp + 15, lines[i].length()).c_str()));
    return (0);
}

std::string readFile(std::string &path)
{
	if (path[0] == '/')
		path = path.substr(1, path.length());

    std::ifstream file(path.c_str());
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
