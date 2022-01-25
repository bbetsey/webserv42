#include "Network.hpp"


// MARK: - Class Constructor

Network::Network( const Config &conf ) : _conf( conf ) {}


// MARK: - Class Distructor

Network::~Network( void ) {}


// MARK: - Private Methods

void	Network::watch_loop( int kq, struct kevent *kset, int len ) {
	struct kevent			events[1024];
	int						new_event;

	errno = 0;

	while ( 1 ) {
		new_event = kevent( kq, NULL, 0, events, len, NULL );
		if ( new_event < 1 && errno == EINTR ) {
			std::cout << "Network: kevent loop" << std::endl;
			break;
			// Обрабатыывать сигнал выхода с помощью SIGINT https://www.tutorialspoint.com/cplusplus/cpp_signal_handling.htm
			// https://habr.com/ru/post/600123/
			// https://cpp.hotexamples.com/ru/examples/-/-/kevent/cpp-kevent-function-examples.html
		} 
		for ( int i = 0; i < new_event; ++i ) {
			int event_fd = events[i].ident;
			if ( events[i].flags & EV_EOF ) {
				std::cout << "Disconnect request" << std::endl;
				EV_SET( &events[i], event_fd, EVFILT_READ, EV_DELETE, 0, 0, 0 );
				if ( kevent( kq, &events[i], 1, NULL, 0, NULL ) == -1 ) {
					std::cout << "Network: kevent delete error" << std::endl;
					break;
				}
				std::cout << "Disconnected" << std::endl;
				close( event_fd );
			}
			else if ( is_listen_socket( kset, events[i].ident, len ) ) {
				std::cout << "New connection request" << std::endl;
				accept_new_client( kq, events[i].ident );
			}
			else if ( events[i].filter == EVFILT_READ ) {
				recv_msg( events[i] );

				t_udata *data = ( t_udata * )(events[i].udata);
				data->is_send = 0;
				
				EV_SET( &events[i], event_fd, EVFILT_WRITE, EV_ADD, 0, 0, events[i].udata );
				if ( kevent( kq, &events[i], 1, NULL, 0, NULL ) == -1 ) {
					std::cout << "Network: kevent add new client" << std::endl;
					break;
				}
			}
			else if ( events[i].filter == EVFILT_WRITE ) {
				t_udata *data = ( t_udata * )(events[i].udata);

				if ( !data->is_send ) {
					send_msg( events[i] );
					data->is_send = !data->is_send;
				}

			}
		}
	}
	close(kq);

}

int	Network::is_listen_socket( struct kevent *kset, int fd, int len ) {
	for ( int i = 0; i < len; ++i, ++kset )
		if ( kset->ident == ( unsigned long )fd ) return 1;
	return 0;
}

void	Network::recv_msg( struct kevent &event ) {
	char	buf[ BUFFER_READ ];
	size_t	read_bytes;

	read_bytes = recv( event.ident, buf, sizeof( buf ), 0 );
	
	std::cout << "\n========== REQUEST MSG ==========" << std::endl;
	if ( read_bytes > 0 ) {
		buf[ read_bytes ] = '\0';
		std::cout << buf << std::endl;
	}
}

void	Network::send_msg( struct kevent &event ) {
	std::string msg = "HTTP/1.1 200 OK\r\nServer: webserv\r\nContent-Type: text/html\r\n\r\n";
	msg += "<html><body><h1>Hello World!</h1></body></html>";

	send( event.ident, msg.c_str(), msg.length(), 0 );
}


// MARK - Accept, Disconnect

void	Network::accept_new_client( int kq, int fd ) {
	struct sockaddr_in	new_addr;
	socklen_t			socklen = sizeof( new_addr );
	int 				client_fd;
	
	client_fd = accept( fd, ( struct sockaddr * )&new_addr, &socklen );

	struct kevent		new_event;
	t_udata				new_data[1];
				
	new_data->is_send = 0;
	new_data->listen_socket = fd;
	new_data->addr = &new_addr;

	EV_SET( &new_event, client_fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, new_data );

	if ( kevent( kq, &new_event, 1, NULL, 0, NULL ) == -1 ) {
		std::cout << "Network: kevent add new client" << std::endl;
	}
}

// MARK: - Class Methods

void	Network::start( void ) {
	int 			kq;
	struct kevent	kset[ _conf.servers.size() ];

	kq = kqueue();

	std::vector< ServerConfig >::iterator	it = _conf.servers.begin();
	for ( int i = 0; it != _conf.servers.end(); ++it, ++i ) {
		Socket	socket = Socket( it->host, it->port );
		if ( socket.start() ) {
			EV_SET( kset + i, socket.get_sock_fd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL );
		}
	}
	if ( kevent( kq, kset, _conf.servers.size(), NULL, 0, NULL ) == -1 )
		std::cout << "Network: kevent error" << std::endl;

	watch_loop( kq, kset, _conf.servers.size() );
}
