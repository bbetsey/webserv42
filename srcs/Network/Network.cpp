#include "Network.hpp"


// MARK: - Class Constructor

Network::Network( t_conf conf ) : _conf( conf ) {}


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
				std::cout << "Disconnect" << std::endl;
				EV_SET( &events[i], event_fd, EVFILT_READ, EV_DELETE, 0, 0, 0 );
				if ( kevent( kq, &events[i], 1, NULL, 0, NULL ) == -1 ) {
					std::cout << "Network: kevent delete error" << std::endl;
					break;
				}
				close( event_fd );
			}
			else if ( is_listen_socket( kset, events[i].ident ) ) {
				int client_fd = accept( events[i].ident, ( struct sockaddr * )&addr, &socklen );
				struct kevent event_set;
				EV_SET( &event_set, client_fd, EVFILT_READ, EV_ADD, 0, 0, 0 );
				if ( kevent( kq, &event_set, 1, NULL, 0, NULL ) == -1 ) {
					std::cout << "Network: kevent add new client" << std::endl;
					break;
				}
			}
			else if ( events[i].filter == EVFILT_READ ) {
				recv_msg( events[i].ident );
				
			}
			else if ( events[i].filter == EVFILT_WRITE ) {
				send_msg( events[i].ident );
			}
		}
	}

}


// MARK: - Class Methods

void	Network::start( void ) {
	int 			kq;
	struct kevent	kset[ _conf.servers.size() ];

	kq = kqueue();

	std::vector< s_server >::iterator	it = _conf.servers.begin();
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