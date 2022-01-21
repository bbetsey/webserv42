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
	std::cout << "PATH_INFO=" << this->_env["PATH_INFO"] << std::endl;
    this->_env["PATH_TRANSLATED"] = this->_uri._path;
    this->_env["SCRIPT_NAME"] = this->_cfg.getLocation(this->_uri._path)._cgi_path;
    this->_env["QUERY_STRING"] = this->_uri._qString;
    this->_env["REMOTE_ADDR"] = this->_cfg._host;
 
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

static char	**getEnvAsCstrArray(const std::map<std::string, std::string> &_env)
{
	char	**ret = new char*[_env.size() + 1];
	int	j = 0;

	for (std::map<std::string, std::string>::const_iterator i = _env.begin(); i != _env.end(); i++)
    {
		std::string	element = i->first + "=" + i->second;
		ret[j] = new char[element.size() + 1];
		ret[j] = strcpy(ret[j], (const char*)element.c_str());
		j++;
	}
	ret[j] = NULL;
	return ret;
}

std::string Cgi::execute(void)
{
	pid_t		pid;
	int			saveStdin;
	int			saveStdout;
	char		**env;
	std::string	newBody;

	try {
		env = getEnvAsCstrArray(this->_env);
	}
	catch (std::bad_alloc &e) {
		std::cerr << e.what() << std::endl;
	}

	// SAVING STDIN AND STDOUT IN ORDER TO TURN THEM BACK TO NORMAL LATER
	saveStdin = dup(STDIN_FILENO);
	saveStdout = dup(STDOUT_FILENO);

	FILE	*fIn = tmpfile();
	FILE	*fOut = tmpfile();
	long	fdIn = fileno(fIn);
	long	fdOut = fileno(fOut);
	int		ret = 1;

	write(fdIn, _body.c_str(), _body.size());
	lseek(fdIn, 0, SEEK_SET);

	pid = fork();

	if (pid == -1)
	{
		std::cerr << "Fork crashed." << std::endl;
		return ("Status: 500\r\n\r\n");
	}
	else if (!pid)
	{
		char * const * nll = NULL;

		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);
		execve(this->_cfg.getLocation(this->_uri._path)._cgi_path.c_str(), nll, env);
		std::cerr << "Execve crashed. " << this->_cfg.getLocation(this->_uri._path)._cgi_path.c_str() << std::endl;
		write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
	}
	else
	{
		char	buffer[CGI_BUFSIZE] = {0};

		waitpid(-1, NULL, 0);
		lseek(fdOut, 0, SEEK_SET);

		ret = 1;
		while (ret > 0)
		{
			memset(buffer, 0, CGI_BUFSIZE);
			ret = read(fdOut, buffer, CGI_BUFSIZE - 1);
			newBody += buffer;
		}
	}

	dup2(saveStdin, STDIN_FILENO);
	dup2(saveStdout, STDOUT_FILENO);
	fclose(fIn);
	fclose(fOut);
	close(fdIn);
	close(fdOut);
	close(saveStdin);
	close(saveStdout);

	for (size_t i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;

	if (!pid)
		exit(0);

	return (newBody);
}

// std::string Cgi::execute(void)
// {
//     char        **env;
//     std::string newBody;
//     try
//     {
//         env = getEnvAsCstrArray(this->_env);
//     }
//     catch (std::exception &e)
//     {
//         std::cout << e.what();
//         return ("Status: 500\r\n\r\n");
//     }

// 	int			saveStdin = dup(STDIN_FILENO);
// 	int			saveStdout = dup(STDOUT_FILENO);

//     FILE *fIn = tmpfile();
//     FILE *fOut = tmpfile();
//     long fdIn = fileno(fIn);
//     long fdOut = fileno(fOut);
//     int ret = 1;

//     write(fdIn, this->_body.c_str(), this->_body.size());
//     lseek(fdIn, 0, SEEK_SET);

//     pid_t pid = fork();

//     if (pid == -1)
//     {
//         std::cout << "Fork crashed" << std::endl;
//         return ("Status: 500\r\n\r\n");
//     }
//     else if (pid == 0)
//     {
//         char * const * nll = NULL;

//         dup2(fdIn, STDIN_FILENO);
//         dup2(fdOut, STDOUT_FILENO);
//         execve(this->_cfg.getLocation(this->_uri._path)._cgi_path.c_str(), nll, env);
//         std::cout << "Execve crashed" << std::endl;
//         return ("Status: 500\r\n\r\n");
//     }
//     else
//     {
//         char	buffer[65536] = {0};

//         waitpid(-1, NULL, 0);
//         lseek(fdOut, 0, SEEK_SET);

//         ret = 1;
// 		while (ret > 0)
// 		{
// 			memset(buffer, 0, 65536);
// 			ret = read(fdOut, buffer, 65536 - 1);
// 			newBody += buffer;
// 		}
//     }

//     dup2(saveStdin, STDIN_FILENO);
// 	dup2(saveStdout, STDOUT_FILENO);
// 	fclose(fIn);
// 	fclose(fOut);
// 	close(fdIn);
// 	close(fdOut);
// 	close(saveStdin);
// 	close(saveStdout);

//     for (size_t i = 0; env[i]; i++)
// 		delete[] env[i];
// 	delete[] env;

//     if (!pid)
// 		exit(0);

// 	return (newBody);
// }
