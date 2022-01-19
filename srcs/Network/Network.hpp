#ifndef NETWORK_HPP
# define NETWORK_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <vector>


enum Method {
	GET,
	POST,
	DELETE
};


typedef struct s_location {

	std::string				uri;
	std::string				root;
	std::string				index;
	std::string				cgi_path;
	std::string				cgi_ext;
	std::string				alias;
	std::vector< Method >	methods;
	int						autoindex;
	int						max_body_size;

}							t_location;


typedef struct s_server {

	std::string					host;
	std::string					name;
	std::string					port;
	std::string					error_page;
	std::vector< t_location >	locations;

}							t_server;


#endif
