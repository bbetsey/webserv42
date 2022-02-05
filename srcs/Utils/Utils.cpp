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
	if (ext == "ico")
		return "image/x-icon";
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

std::string getStatusName(const int &code)
{
	switch (code)
	{
		//####### 1xx - Informational #######
		case 100: return "Continue";
		case 101: return "Switching Protocols";
		case 102: return "Processing";
		case 103: return "Early Hints";

		//####### 2xx - Successful #######
		case 200: return "OK";
		case 201: return "Created";
		case 202: return "Accepted";
		case 203: return "Non-Authoritative Information";
		case 204: return "No Content";
		case 205: return "Reset Content";
		case 206: return "Partial Content";
		case 207: return "Multi-Status";
		case 208: return "Already Reported";
		case 226: return "IM Used";

		//####### 3xx - Redirection #######
		case 300: return "Multiple Choices";
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 303: return "See Other";
		case 304: return "Not Modified";
		case 305: return "Use Proxy";
		case 307: return "Temporary Redirect";
		case 308: return "Permanent Redirect";

		//####### 4xx - Client Error #######
		case 400: return "Bad Request";
		case 401: return "Unauthorized";
		case 402: return "Payment Required";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 406: return "Not Acceptable";
		case 407: return "Proxy Authentication Required";
		case 408: return "Request Timeout";
		case 409: return "Conflict";
		case 410: return "Gone";
		case 411: return "Length Required";
		case 412: return "Precondition Failed";
		case 413: return "Content Too Large";
		case 414: return "URI Too Long";
		case 415: return "Unsupported Media Type";
		case 416: return "Range Not Satisfiable";
		case 417: return "Expectation Failed";
		case 418: return "I'm a teapot";
		case 421: return "Misdirected Request";
		case 422: return "Unprocessable Content";
		case 423: return "Locked";
		case 424: return "Failed Dependency";
		case 425: return "Too Early";
		case 426: return "Upgrade Required";
		case 428: return "Precondition Required";
		case 429: return "Too Many Requests";
		case 431: return "Request Header Fields Too Large";
		case 451: return "Unavailable For Legal Reasons";

		//####### 5xx - Server Error #######
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway";
		case 503: return "Service Unavailable";
		case 504: return "Gateway Timeout";
		case 505: return "HTTP Version Not Supported";
		case 506: return "Variant Also Negotiates";
		case 507: return "Insufficient Storage";
		case 508: return "Loop Detected";
		case 510: return "Not Extended";
		case 511: return "Network Authentication Required";

		default: return std::string();
	}
}
