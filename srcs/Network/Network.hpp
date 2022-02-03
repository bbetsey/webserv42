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
#include "../Config/Config.hpp"
#include "../Includes/webserv.hpp"
#include "../Handler/Request.hpp"

# define BUFFER_READ	4096
# define RESPONSE		"HTTP/1.1 200 OK\r\nServer: webserv\r\nContent-Type: text/html\r\nContent-Length: 48\r\nConnectioin: keep-alive\r\n\r\n<html><body><h1>Hello World!</h1></body></html>\r\n"


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

	int					flag;
	int					is_send;
	std::string			msg;
	Request				*req;
	struct sockaddr_in	*addr;

}						t_udata;

class Network {

	private:

		Config	_conf;

		std::string rcvdMsg;

		// MARK: - Private Methods

		void	watch_loop( int kq, struct kevent *kset, int len );
		void	recv_msg( struct kevent &event, t_udata *data );
		void	send_msg( struct kevent &event, t_udata *data );

		void	accept_new_client( int kq, int fd );
		void	read_socket( int kq, struct kevent &event );
		void	write_socket( int kq, struct kevent &event );

		int		is_listen_socket( struct kevent *kset, int fd, int len );
		t_udata	*init_udata( struct sockaddr_in *addr );


	public:

		// MARK: - Class Constructor

		Network( const Config &conf );


		// MARK: - Class Distructor

		~Network( void );


		// MARK: - Class Methods

		void	start( void );


		// MARK: - Getters

		const Config	get_conf( void ) const;

};

#endif
