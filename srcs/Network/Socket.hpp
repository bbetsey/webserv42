#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "../Includes/webserv.hpp"

# define BACKLOG 10


class Socket {


	private:

		std::string		_host;
		std::string		_port;
		struct addrinfo	*_res;
		int				_sockfd;


	public:

		// MARK: - Class Constructor

		Socket( std::string host, std::string port );


		// MARK: - Class Distructor

		~Socket( void );


		// MARK: - Class Methods

		int			start( void );


		// MARK: - Getters

		int			get_sock_fd( void );
		std::string	get_host( void );
		std::string get_port( void );

};


#endif
