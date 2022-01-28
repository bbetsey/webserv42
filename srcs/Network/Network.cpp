#include "Network.hpp"

// Обрабатыывать сигнал выхода с помощью SIGINT https://www.tutorialspoint.com/cplusplus/cpp_signal_handling.htm
// https://habr.com/ru/post/600123/
// https://cpp.hotexamples.com/ru/examples/-/-/kevent/cpp-kevent-function-examples.html

// MARK: - Class Constructor

Network::Network( const Config &conf ) : _conf( conf ) {}


// MARK: - Class Distructor

Network::~Network( void ) {}


// MARK: - Private Methods

void	Network::watch_loop( int kq, struct kevent *kset, int len ) {

	struct kevent			events[1024];
	int						new_events;

	while (1) {

		new_events = kevent( kq, NULL, 0, events, len, NULL );

		if ( new_events < 1 && errno == EINTR ) {
			std::cout << "Network: kevent loop" << std::endl;
			break;
		} 

		for ( int i = 0; i < new_events; ++i ) {
			
			t_udata	*data = ( t_udata * )(events[i].udata);

			if ( is_listen_socket( kset, events[i].ident, len ) )
				accept_new_client( kq, events[i].ident );

			else if ( events[i].filter == EVFILT_READ )	
				read_socket( kq, events[i], data );
			
			else if ( events[i].filter == EVFILT_WRITE )
				write_socket( kq, events[i], data );
		}
	}

	close(kq);
}


// MARK: - RECV, SEND

void	Network::recv_msg( struct kevent &event, t_udata *data) {
	char	buf[ BUFFER_READ ];
	size_t	read_bytes;

	read_bytes = recv( event.ident, buf, sizeof( buf ), 0 );
	
	std::cout << "\n========== REQUEST MSG ==========" << std::endl;
	if ( read_bytes > 0 ) {
		buf[ read_bytes ] = '\0';
		data->msg = buf;
	}
}

void	Network::send_msg( struct kevent &event, t_udata *data ) {

	if (data->msg.length() > 0)
	{
		std::string msg = Request(data->msg, this->_conf.servers[0]).response();
		LOG("SEND:\n" + msg, INFO);
		send( event.ident, msg.c_str(), msg.length(), 0 );
	}
}

// void	Network::recv_msg( struct kevent &event, t_udata *data ) {
// 	char	buf[ BUFFER_READ ];
// 	size_t	read_bytes;

// 	read_bytes = recv( event.ident, buf, sizeof( buf ), 0 );
	
// 	std::cout << "\n========== REQUEST MSG ==========" << std::endl;
// 	if ( read_bytes > 0 ) {
// 		buf[ read_bytes ] = '\0';
// 		std::cout << buf << std::endl;
// 		std::string msg = "HTTP/1.1 200 OK\r\nServer: webserv\r\nContent-Type: text/html\r\nContent-Length: 48\r\nConnectioin: Closed\r\n\r\n";
// 		msg += "<html><body><h1>Hello World!</h1></body></html>\r\n";
// 		data->msg = msg;
// 		LOG( data->msg, INFO );
// 	}

// }

// void	Network::send_msg( struct kevent &event, t_udata *data ) {

// 	std::cout << "Length: " << data->msg.length() << std::endl;

// 	if ( data->msg.length() > 0 ) {
// 		LOG( "in send handler", INFO );
// 		LOG( data->msg, INFO );
// 		send( event.ident, data->msg.c_str(), data->msg.length(), 0 );
// 		data->msg = "";
// 	}

// 	// std::string msg = "HTTP/1.1 200 OK\r\nServer: webserv\r\nContent-Type: text/html\r\nContent-Length: 48\r\nConnectioin: Closed\r\n\r\n";

// 	// msg += "<html><body><h1>Hello World!</h1></body></html>\r\n";

// 	// send( event.ident, msg.c_str(), msg.length(), 0 );
// }


// MARK - Accept, Read, Write

void	Network::accept_new_client( int kq, int fd ) {
	struct sockaddr_in	new_addr;
	socklen_t			socklen = sizeof( new_addr );
	int 				client_fd;
	
	client_fd = accept( fd, ( struct sockaddr * )&new_addr, &socklen );

	int opt = 1;
	setsockopt( client_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof( opt ) );

	struct kevent		new_event[2];
	t_udata				new_data[1];
				
	new_data->is_send = 0;
	new_data->flag = 0;
	new_data->addr = &new_addr;

	EV_SET( &new_event[0], client_fd, EVFILT_READ, EV_ADD, 0, 0, new_data );
	EV_SET( &new_event[1], client_fd, EVFILT_WRITE, EV_ADD, 0, 0, new_data );

	if ( kevent( kq, new_event, 2, NULL, 0, NULL ) == -1 )
		std::cout << "Network: kevent add new client" << std::endl;

	LOG( "new connection", INFO );
}

void	Network::read_socket( int kq, struct kevent &event, t_udata *data ) {

	if ( event.flags & EV_EOF ) {
		std::cout << "EOF READ" << std::endl;
		EV_SET( &event, event.ident, EVFILT_READ, EV_DELETE, 0, 0, 0 );
		kevent( kq, &event, 1, NULL, 0, NULL );
		if ( data->flag ) close( event.ident );
		else data->flag = 1;
	} else {
		recv_msg( event, data );
		data->is_send = 0;
	}

}

void	Network::write_socket( int kq, struct kevent &event, t_udata *data ) {

	if ( event.flags & EV_EOF ) {
		std::cout << "EOF WRITE" << std::endl;
		EV_SET( &event, event.ident, EVFILT_WRITE, EV_DELETE, 0, 0, 0 );
		kevent( kq, &event, 1, NULL, 0, NULL );
		if ( data->flag ) close( event.ident );
		else data->flag = 1;
	} else {
		if ( data->is_send ) return;
		send_msg( event, data );
		data->is_send = 1;
	}

}


// MARK: - Utils

int	Network::is_listen_socket( struct kevent *kset, int fd, int len ) {
	for ( int i = 0; i < len; ++i, ++kset )
		if ( kset->ident == ( unsigned long )fd ) return 1;
	return 0;
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
