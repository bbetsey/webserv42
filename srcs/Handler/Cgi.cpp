#include "Cgi.hpp"

// https://datatracker.ietf.org/doc/html/rfc3875#section-4.1

/*

strIsGood:
Status: 302
Location: http://localhost:3333/secret.html
Set-Cookies: secret

Cookie == 'secret':
Status: 302
Location: http://localhost:3333/secret.html

Cookie != 'secret':
Status: 302
Location: http://localhost:3333/login.html


*/
Cgi::Cgi(Request &req, const std::string &cgiPath) : _cgiPath(cgiPath),  _req(req), _method(req.getMethod()), _uri(req.getUri()), _body(req.getBody()), _headers(req.getHeaders()), _cfg(req.getConfig())
{
	
    this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
    this->_env["SERVER_SOFTWARE"] = this->_cfg.name;
    this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_env["SERVER_PORT"] = this->_cfg.port;
    this->_env["REQUEST_METHOD"] = this->_req.getMethod();
    this->_env["PATH_INFO"] = this->_uri._path;
    this->_env["PATH_TRANSLATED"] = this->_uri._path;
    this->_env["SCRIPT_NAME"] = this->_cgiPath;
	this->_env["SCRIPT_FILENAME"] = this->_cgiPath;
    this->_env["QUERY_STRING"] = this->_uri._qString;
    this->_env["REMOTE_ADDR"] = this->_cfg.host;
	this->_env["REMOTEaddr"] = this->_cfg.host;
	this->_env["REQUEST_URI"] = this->_uri._path + this->_uri._qString;
	this->_env["REDIRECT_STATUS"] = "200";

    if (this->_headers.find("Hostname") != this->_headers.end())
        this->_env["SERVER_NAME"] = this->_headers["Hostname"];
    else
        this->_env["SERVER_NAME"] = this->_env["REMOTE_ADDR"];

	if (this->_headers.find("Cookie") != this->_headers.end())
		this->_env["COOKIE"] = this->_headers["COOKIE"];

    if (this->_headers.find("Auth-Scheme") != this->_headers.end())
    {
        this->_env["AUTH_TYPE"] = this->_headers["Authorization"];
        this->_env["REMOTE_USER"] = this->_headers["Authorization"];
        this->_env["REMOTE_IDENT"] = this->_headers["Authorization"];
    }
    this->_env["CONTENT_TYPE"] = this->_headers["Content-Type"];
    this->_env["CONTENT_LENGTH"] = itos(this->_req.getBody().length());
}

static char **map2ca(std::map<std::string, std::string> &map) // std::map<std::string, std::string> to char array
{
	char **ret;

	try
	{
		ret = new char* [map.size() + 1];
	}
	catch (std::exception &e)
	{
		std::cout << e.what();
	}

	int i = 0;
	std::map<std::string, std::string>::iterator it = map.begin(), ite = map.end();

	for (; it != ite; it++)
	{
		ret[i] = new char[it->first.size() + it->second.size() + 2];
		strcpy(ret[i], (it->first + "=" + it->second).c_str());
		i++;
	}
	ret[i] = 0;
	return (ret);
}

static void freeCa(char **ca)
{
	for (size_t i = 0; ca[i]; i++)
		delete[] ca[i];
	delete[] ca;
}

std::string Cgi::execute(void)
{
	std::string ret;
	char **env = map2ca(this->_env);
	int tmpIn = dup(0), tmpOut = dup(1);
	FILE *fIn = tmpfile(), *fOut = tmpfile();
	long fdIn = fileno(fIn), fdOut = fileno(fOut);

	write(fdIn, this->_body.c_str(), this->_body.length());
	lseek(fdIn, 0, 0);

	pid_t pid = fork();

	if (pid == -1)
	{
		return ("Status: 501\r\n\r\n");
		freeCa(env);
	}
	else if (pid == 0)
	{
		char * const * _null = NULL;
		dup2(fdIn, 0);
		dup2(fdOut, 1);
		execve(this->_cgiPath.c_str(), _null, env);
		write(1, "Status: 502\r\n\r\n", 15);
	}
	else
	{
		char buff[65536] = {0};

		waitpid(-1, NULL, 0);
		lseek(fdOut, 0, 0);

		while (read(fdOut, buff, 65536) > 0)
		{
			ret += buff;
			memset(buff, 0, 65536);
		}
	}

	dup2(tmpIn, 0);
	dup2(tmpOut, 1);
	fclose(fIn);
	fclose(fOut);
	close(fdIn);
	close(fdOut);
	close(tmpIn);
	close(tmpOut);

	freeCa(env);
	if (!pid)
		exit(0);

	return (ret);
}
