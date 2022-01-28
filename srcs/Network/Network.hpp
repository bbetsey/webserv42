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

# define BUFFER_READ 4096


typedef struct s_udata {

	int					flag;
	int					is_send;
	struct sockaddr_in	*addr;
	int					msg_size;
	int					cur_size;
	int					ready;
	std::string			response;

}						t_udata;

class Network {

	private:

		Config	_conf;


		// MARK: - Private Methods

		void	watch_loop( int kq, struct kevent *kset, int len );
		int		is_listen_socket( struct kevent *kset, int fd, int len );
		void	recv_msg( struct kevent &event, t_udata *data );
		void	send_msg( struct kevent &event, t_udata *data );

		void	accept_new_client( int kq, int fd );
		void	read_socket( int kq, struct kevent &event, t_udata *data );
		void	write_socket( int kq, struct kevent &event, t_udata *data );
	

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
