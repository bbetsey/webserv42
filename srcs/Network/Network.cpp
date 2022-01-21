#include "Network.hpp"


// MARK: - Class Constructor

Network::Network( const Config &conf ) : _conf( conf ) {}


// MARK: - Class Distructor

Network::~Network( void ) {}


// MARK: - Private Methods

void	Network::watch_loop( int kq, struct kevent *kset, int len ) {
	struct kevent			events[1024];
	int						new_event;
	struct sockaddr_storage	addr;
	socklen_t				socklen = sizeof( addr );

	while ( 1 ) {
		new_event = kevent( kq, NULL, 0, events, len, NULL );
		if ( new_event < 1 ) {
			std::cout << "Network: kevent loop" << std::endl;
			break;
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
				int client_fd = accept( events[i].ident, ( struct sockaddr * )&addr, &socklen );
				
				struct kevent event_set;
				t_udata	new_data[1];
				new_data->is_send = 0;
				new_data->listen_socket = events[i].ident;
				event_set.udata = new_data;
				EV_SET( &event_set, client_fd, EVFILT_READ, EV_ADD, 0, 0, 0 );
				if ( kevent( kq, &event_set, 1, NULL, 0, NULL ) == -1 ) {
					std::cout << "Network: kevent add new client" << std::endl;
					break;
				}

				std::cout << "New connection accepted" << std::endl;
			}
			else if ( events[i].filter == EVFILT_READ ) {
				std::cout << "In read filter" << std::endl;
				recv_msg( events[i] );
				struct kevent event_set;
				EV_SET( &event_set, event_fd, EVFILT_WRITE, EV_ADD, 0, 0, 0 );
				if ( kevent( kq, &event_set, 1, NULL, 0, NULL ) == -1 ) {
					std::cout << "Network: kevent add new client" << std::endl;
					break;
				}
			}
			else if ( events[i].filter == EVFILT_WRITE ) {
				std::cout << "Check udata flag" << std::endl;
				t_udata *data = ( t_udata * )events[i].udata;

				if ( !data->is_send ) {
					std::cout << "In write filter" << std::endl;
					send_msg( events[i] );
					data->is_send = 1;
				}
			}
		}
	}

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
	std::cout << "\n========== END MSG ==========" << std::endl;
}

void	Network::send_msg( struct kevent &event ) {
	std::string msg = "Hello World!";

	send( event.ident, msg.c_str(), msg.length(), 0 );
}


// MARK: - Class Methods

void	Network::start( void ) {
	int 			kq;
	struct kevent	kset[ _conf._servers.size() ];

	kq = kqueue();

	std::vector< ServerConfig >::iterator	it = _conf._servers.begin();
	for ( int i = 0; it != _conf._servers.end(); ++it, ++i ) {
		Socket	socket = Socket( it->_host, it->_port );
		if ( socket.start() ) {
			EV_SET( kset + i, socket.get_sock_fd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL );
		}
	}
	if ( kevent( kq, kset, _conf._servers.size(), NULL, 0, NULL ) == -1 )
		std::cout << "Network: kevent error" << std::endl;

	watch_loop( kq, kset, _conf._servers.size() );
}