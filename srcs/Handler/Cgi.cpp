#include "Cgi.hpp"

// https://www6.uniovi.es/~antonio/ncsa_httpd/cgi/env.html

Cgi::Cgi(Request &req) : _req(req), _method(req.getMethod()), _uri(req.getUri()), _body(req.getBody()), _headers(req.getHeaders()), _cfg(req.getConfig())
{
    this->_env["SERVER_SOFTWARE"] = "webserv/0.1";
    this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
    this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_env["SERVER_PORT"] = this->_cfg._port;
    this->_env["REQUEST_METHOD"] = this->_req.getMethod();
    this->_env["PATH_INFO"] = this->_uri._path;
    this->_env["PATH_TRANSLATED"] = this->_uri._path;
    this->_env["SCRIPT_NAME"] = this->_cfg.getLocation(this->_uri._path)._cgi_path;
	this->_env["SCRIPT_FILENAME"] = this->_cfg.getLocation(this->_uri._path)._cgi_path;
    this->_env["QUERY_STRING"] = this->_uri._qString;
    this->_env["REMOTE_ADDR"] = this->_cfg._host;
	this->_env["REMOTEaddr"] = this->_cfg._host;
	this->_env["REQUEST_URI"] = this->_uri._path + this->_uri._qString;
	this->_env["REDIRECT_STATUS"] = "200";

    if (this->_headers.find("Hostname") != this->_headers.end())
        this->_env["SERVER_NAME"] = this->_headers["Hostname"];
    else
        this->_env["SERVER_NAME"] = this->_env["REMOTE_ADDR"];

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
	char **ret = new char* [map.size() + 1];
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
		return ("Status: 500\r\n\r\n");
		freeCa(env);
	}
	else if (pid == 0)
	{
		dup2(fdIn, 0);
		dup2(fdOut, 1);
		char **null = NULL;
		execve(this->_cfg.getLocation(this->_uri._path)._cgi_path.c_str(), null, env);
		write(1, "Status: 500\r\n\r\n", 15);
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
