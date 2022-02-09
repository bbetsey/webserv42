#include "Request.hpp"

Request::Request(const ServerConfig cfg) : _cfg(cfg), _isReady(0), _headerWasRead(0), _contentLength(0), _reqHeaderEndPos(0), _locWasFound(0)
{
    for (size_t i = 0; i < this->_cfg.locations.size(); i++)
        this->_cfg.locationdirs.push_back(this->_cfg.locations[i].path);
}

Request::~Request(void) {}

void Request::checkIfChunked(void)
{
    if (this->_reqWhole.find("Transfer-Encoding: chunked") != std::string::npos)
        this->_isChunked = 1;
    else
        this->_isChunked = 0;
}

void Request::add_msg(const std::string &msg)
{
    this->_reqWhole += msg;

    if (!this->_headerWasRead && (this->_reqHeaderEndPos = hasDoubleCRLF(this->_reqWhole)))
    {
        this->_headerWasRead = 1;
        this->checkIfChunked();
        this->_contentLength = getContentLength(this->_reqWhole);
    }
    if (this->_headerWasRead && !this->_isChunked && this->_reqWhole.length() - this->_reqHeaderEndPos + 2 >= this->_contentLength)
    {
        this->_isReady = 1;
    }
    if (this->_headerWasRead && this->_isChunked)
    {
        if (this->_reqWhole.find("0\r\n\r\n") != std::string::npos)
            this->_isReady = 1;
    }
}

bool Request::isReady(void) const
{
    return (this->_isReady);
}

void Request::formatPath(void)
{
    std::vector<std::string> tokens;
    std::string formattedPath;
    this->_ori_path = this->_uri._path;

    split(this->_uri._path, tokens, "/");
    for (size_t i = 0; i < tokens.size(); i++)
    {
        if (!this->_locWasFound)
        {
            std::vector<std::string>::iterator it = std::find(this->_cfg.locationdirs.begin(), this->_cfg.locationdirs.end(), "/" + tokens[i]);
            
            if (it != this->_cfg.locationdirs.end())
            {
                this->_locWasFound = 1;
                this->_loc = this->_cfg.getLocation(*it);
                tokens[i] = this->_loc.root;
                tokens[i].pop_back();
            }
        }
        formattedPath += tokens[i] + "/";
    }
    if (pathType(formattedPath) != 2)
        formattedPath.pop_back();

    this->_uri._path = formattedPath;
}

void Request::parseFirstLine()
{
    std::string firstLine = this->_reqWhole.substr(0, this->_reqWhole.find_first_of('\r') - 1);
    std::vector<std::string> tokens;
    split(firstLine, tokens, " \r\n");

    try
    {
        this->_method = tokens.at(0);
        this->_uri = Uri(tokens.at(1));
        if (this->_uri._path != "/")
            formatPath();
        if (!this->_locWasFound)
        {
            this->_loc = this->_cfg.getLocation("/");
            this->_uri._path = this->_loc.root + this->_uri._path;
        }

        this->_httpVersion = tokens.at(2);
    }
    catch (std::exception &e)
    {
        LOG("Wrong first line!", ERROR, atoi(this->_cfg.port.c_str()));
        this->_parseStatus = 0;
    }
}

void Request::parseHeaders()
{
    this->_reqHeader = this->_reqWhole.substr(0, this->_reqHeaderEndPos + 2);
    std::vector<std::string> lines;
    split(this->_reqHeader, lines, "\r\n");

    if (lines.size() > 1)
    {
        for (size_t i = 1; i < lines.size(); i++)
        {
            if (lines[i] == CRLF)
                break;
            size_t pos = lines[i].find_first_of(':');
            this->_reqHeaders[lines[i].substr(0, pos)] = lines[i].substr(pos + 2, lines[i].length());
        }
    }
}

void Request::parseBody(void)
{
    this->_reqBody = this->_reqWhole.substr(this->_reqHeaderEndPos + 2, this->_reqWhole.length());

    if (this->_isChunked)
    {
        std::vector<std::string> lines;
        std::string newBody;
        split(this->_reqBody, lines, "\r\n");
        for (size_t i = 0; i < lines.size(); i++)
            if (i % 2 != 0)
                newBody += lines[i];
        
        this->_reqBody = newBody;
    }
}

void Request::parse(void)
{
    this->_parseStatus = 1;
    this->parseFirstLine();
    this->parseHeaders();
    this->parseBody();
}

void Request::genHeader()
{
    this->_resHeader += "HTTP/1.1 " + itos(this->_resStatus) + " " + getStatusName(this->_resStatus) + CRLF;
    this->_resHeader += "Date: " + getDate() + CRLF;
    this->_resHeader += "Server: " + this->_cfg.name + CRLF;
    if (this->_resType != NOTHING)
    {
        this->_resHeader += "Content-Length: " + itos(this->_resBody.size()) + CRLF;
        this->_resHeader += "Content-Type: " + (this->_resType == REGFILE ? getMimeType(this->_uri._path.substr(this->_uri._path.find_last_of('.') + 1, this->_uri._path.length())) : "text/html") + CRLF;
    }
    if (this->_resType == REGFILE)
    {
        this->_resHeader += "Last-Modified: " + getLastModified(this->_uri._path) + CRLF;
    }

    for (std::map<std::string, std::string>::iterator it = this->_resHeaders.begin(); it != this->_resHeaders.end(); it++)
        this->_resHeader += it->first + ": " + it->second + CRLF;

    this->_resHeader += CRLF;
}

static bool vectorContains(std::string elem, std::vector<std::string> vec)
{
    return (std::find(vec.begin(), vec.end(), elem) != vec.end());
}

std::string Request::handlePost(void)
{
    if (!vectorContains("POST", this->_loc.methods))
    {
        this->_resStatus = 405;
        this->_resType = NOTHING;
        return (this->handleErr("Not allowed method"));
    }

    this->_cgiResponse = Cgi(*this, this->_loc.cgi_path).execute();
    this->parseCgiResponse();

    if (this->_cgiStatus != 200)
    {
        this->_resStatus = this->_cgiStatus;
        return (this->handleErr("CGI ERR"));
    }
    this->_resStatus = this->_cgiStatus;
    this->_resType = NOTHING;
    this->genHeader();
    return (this->_resHeader);
}

std::string Request::handleHead(void)
{
    if (!vectorContains("HEAD", this->_loc.methods))
    {
        this->_resStatus = 405;
        this->_resType = NOTHING;
        return (this->handleErr("Not allowed method"));
    }

    std::string tmp = this->handleGet();
    this->_resStatus = 204;
    this->_resHeader = "";
    this->genHeader();
    return (this->_resHeader);
}

std::string Request::handleDelete(void)
{
    if (!vectorContains("DELETE", this->_loc.methods))
    {
        this->_resStatus = 405;
        return (this->handleErr("Not allowed method"));
    }

    if (pathType(this->_uri._path))
	{
		if (remove(this->_uri._path.c_str()) == 0)
			this->_resStatus = 204;
		else
			this->_resStatus = 403;
	}
	else
		this->_resStatus = 404;
	if (this->_resStatus == 403 || this->_resStatus == 404)
		return (this->handleErr("Can't delete"));
	
    this->genHeader();
    return (this->_resHeader);
}

bool Request::readContent(const std::string &path)
{
    if (readFile(path, this->_resBody))
    {
        this->_resType = REGFILE;
        this->_resStatus = 200;
        return (1);
    }
    else
    {
        this->_resType = NOTHING;
        this->_resStatus = 403;
        return (0);
    }
}

std::string Request::handleGet(void)
{
    if (!vectorContains("GET", this->_loc.methods))
    {
        this->_resStatus = 405;
        return (this->handleErr("Not allowed method"));
    }

    this->_cgiResponse = Cgi(*this, this->_loc.cgi_path).execute();

    this->parseCgiResponse();
    if (this->_cgiStatus != 200)
    {
        this->_resStatus = this->_cgiStatus;
        return (this->handleErr("CGI ERR"));
    }

    int tmp = pathType(this->_uri._path);
    if (tmp == 1)
    {
        if (!this->readContent(this->_uri._path))
            return (this->handleErr("Can't open file"));
    }
    else if (tmp == 2)
    {
        std::string indexPath = this->_uri._path + (this->_loc.index.size() > 0 ? this->_loc.index[0] : "index.html");
        switch (pathType(indexPath))
        {
            case 1:
                if (!this->readContent(indexPath))
                {
                    return (this->handleErr("Found index file but can't open it"));
                }
                break;
            default:
                if (this->_loc.autoindex == 1)
                {
                    this->_resStatus = 200;
                    this->_resType = PLAINHTML;
                    this->_resBody = AutoIndexGen(this->_uri._path, this->_ori_path).getOutput();
                }
                else
                {
                    this->_resStatus = 404;
                    return (this->handleErr("No/can't open index file and autoindex off"));
                }
                break;
        }
    }
    else
    {
        if (this->_locWasFound)
        {
            std::string indexPath = this->_uri._path + "/" + (this->_loc.index.size() > 0 ? this->_loc.index[0] : "index.html");
            switch (pathType(indexPath))
            {
                case 1:
                    if (!this->readContent(indexPath))
                    {
                        return (this->handleErr("Found index file but can't open it"));
                    }
                    break;
                default:
                    if (this->_loc.autoindex == 1)
                    {
                        this->_resStatus = 200;
                        this->_resType = PLAINHTML;
                        this->_resBody = AutoIndexGen(this->_uri._path, this->_ori_path).getOutput();
                    }
                    else
                    {
                        this->_resStatus = 404;
                        return (this->handleErr("No/can't open index file and autoindex off"));
                    }
                    break;
            }
        }
        else
        {
            this->_resStatus = 404;
            return (this->handleErr("No such file/directory"));
        }
    }

    this->genHeader();
    return (this->_resHeader + this->_resBody);
}

static int writeContent(const std::string &content, const std::string &path)
{
	std::ofstream	file;

	if (pathType(path))
	{
		file.open(path.c_str());
		file << content;
		file.close();
		return (204);
	}
	else
	{
		file.open(path.c_str(), std::ofstream::out | std::ofstream::trunc);
		if (file.is_open() == false)
			return (403);

		file << content;
		file.close();
		return (201);
	}
}

std::string Request::handlePut(void)
{
    this->_resStatus = writeContent(this->_reqBody, this->_uri._path);

    if (!(this->_resStatus == 201 || this->_resStatus == 204))
        return this->handleErr("Write failed");
    this->_resType = NOTHING;
    this->genHeader();
    return (this->_resHeader);
}

std::string Request::handleErr(const std::string &err)
{
    LOG(err, ERROR, atoi(this->_cfg.port.c_str()));

    int saveStatus = this->_resStatus;
    std::string path = this->_loc.error_pages[this->_resStatus];
    int type = pathType(path);
    if (!(type == 1 && this->readContent(path)))
        this->_resBody = "<!DOCTYPE html>\n<html><title>Generated errorpage</title><body><h1>yo wtf u did? " + getStatusName(this->_resStatus) + "</h1></body></html>";

    this->_resStatus = saveStatus;
    this->_resType = PLAINHTML;
    this->genHeader();

    return (this->_resHeader + this->_resBody);
}

void Request::parseCgiResponse(void)
{
    std::vector<std::string> lines;
    size_t tmpPos;

    split(this->_cgiResponse, lines, "\r\n");
    for (size_t i = 0; i < lines.size(); i++)
    {
        if ((tmpPos = lines[i].find("Status: ")) != std::string::npos)
            this->_cgiStatus = atoi(lines[i].substr(tmpPos + 7, lines[i].length()).c_str());
        else if ((tmpPos = lines[i].find("Content-type: ")) != std::string::npos)
            this->_cgiType = lines[i].substr(tmpPos + 14, lines[i].length());   
        else if ((tmpPos = lines[i].find(":")) != std::string::npos)
            this->_resHeaders[lines[i].substr(0, tmpPos)] = lines[i].substr(tmpPos + 2, lines[i].length());
    }
}

std::string Request::getResponse(void)
{
    LOG("RCVD\n" + this->_reqWhole, INFO, atoi(this->_cfg.port.c_str()));
    this->parse();

    if (!this->_parseStatus)
    {
        this->_resStatus = 500;
        return (this->handleErr("Parser fail"));
    }
    if (this->_reqBody.length() - 2> static_cast<size_t>(this->_loc.max_body_size))
    {
        this->_resStatus = 413;
        return (this->handleErr("Body too large"));
    }

    if (this->_method == "GET")
        return (this->handleGet());
    if (this->_method == "POST")
        return (this->handlePost());
    if (this->_method == "HEAD")
        return (this->handleHead());
    if (this->_method == "DELETE")
        return (this->handleDelete());
    if (this->_method == "PUT")
        return (this->handlePut());

    this->_resStatus = 501;
    return (this->handleErr(this->_method));
}

const std::string &Request::getMethod(void) const { return this->_method; }
const Uri &Request::getUri(void) const { return this->_uri; }
const std::string &Request::getBody(void) const { return this->_reqBody; }
std::map<std::string, std::string> &Request::getHeaders(void) { return this->_reqHeaders; }
const ServerConfig &Request::getConfig(void) const {return this->_cfg; }
