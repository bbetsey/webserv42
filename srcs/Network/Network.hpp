#ifndef NETWORK_HPP
# define NETWORK_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/event.h>
#include <netdb.h>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <sstream>

#include "Socket.hpp"

# define BUFFER_READ 4096


enum Method {
	GET,
	POST,
	DELETE
};


typedef struct s_location {

	std::string							uri;
	std::string							root;
	std::string							index;
	std::string							cgi_path;
	std::string							cgi_ext;
	std::string							alias;
	std::vector< Method >				methods;
	int									autoindex;
	int									max_body_size;
	std::vector< struct s_location >	locations;

}										t_location;


typedef struct s_server {

	std::string					host;
	std::string					name;
	std::string					port;
	std::string					error_page;
	std::vector< t_location >	locations;

}								t_server;


typedef struct s_conf {

	std::vector< s_server >		servers;
	std::string					error_page;
	std::string					root;

}								t_conf;


typedef struct s_udata {

	int		is_send;
	int		listen_socket;
	int		msg_size;

}			t_udata;

class Network {

	private:

		t_conf	_conf;


		// MARK: - Private Methods

		void	watch_loop( int kq, struct kevent *kset, int len );
		int		is_listen_socket( struct kevent *kset, int fd, int len );
		void	recv_msg( struct kevent &event );
		void	send_msg( struct kevent &event );
	

	public:

		// MARK: - Class Constructor

		Network( t_conf conf );


		// MARK: - Class Distructor

		~Network( void );


		// MARK: - Class Methods

		void	start( void );


		// MARK: - Getters

		t_conf	get_conf( void );

};

#endif
