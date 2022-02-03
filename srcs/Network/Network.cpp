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
			LOG( "kevent new events error", ERROR, 0 );
			break;
		} 

		for ( int i = 0; i < new_events; ++i ) {

			if ( is_listen_socket( kset, events[i].ident, len ) )
				accept_new_client( kq, events[i].ident );

			else if ( events[i].filter == EVFILT_READ )	
				read_socket( kq, events[i] );
			
			else if ( events[i].filter == EVFILT_WRITE )
				write_socket( kq, events[i] );
		}
	}

	close(kq);
}


// MARK: - Accept

void	Network::accept_new_client( int kq, int fd ) {
	struct sockaddr_in	*new_addr = new sockaddr_in;
	socklen_t			socklen = sizeof( new_addr );
	int 				client_fd;
	
	client_fd = accept( fd, ( struct sockaddr * )new_addr, &socklen );

	int opt = 1;
	setsockopt( client_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof( opt ) );

	struct kevent		new_event[2];
	t_udata				*new_data = init_udata( new_addr );

	EV_SET( &new_event[0], client_fd, EVFILT_READ, EV_ADD, 0, 0, new_data );
	EV_SET( &new_event[1], client_fd, EVFILT_WRITE, EV_ADD, 0, 0, new_data );

	CHECK( kevent( kq, new_event, 2, NULL, 0, NULL ), "kevent: can't init new connection events" );

	LOG( "Connection", INFO, new_data->addr->sin_port );

}


// MARK: - Read

void	Network::read_socket( int kq, struct kevent &event ) {

	t_udata	*data = ( t_udata * )(event.udata);

	if ( event.flags & EV_EOF || data->flag ) {

		LOG( "EOF READ", INFO, data->addr->sin_port );
		EV_SET( &event, event.ident, EVFILT_READ, EV_DELETE, 0, 0, data );
		CHECK( kevent( kq, &event, 1, NULL, 0, NULL ), "kevent: can't init EV_DELETE flag for EVFILT_READ filter" );
		
		if ( data->flag )
			close( event.ident );
		else data->flag = 1;

	} else {
		recv_msg( event, data );
	}

}

void	Network::recv_msg( struct kevent &event, t_udata *data ) {
	char	buf[ BUFFER_READ ];
	size_t	read_bytes;

	read_bytes = recv( event.ident, buf, sizeof( buf ), 0 );

	if ( read_bytes > 0 ) {
		buf[ read_bytes ] = '\0';

		data->req->add_msg( buf );						// Добавляю полученный запрос

		LOG( "Read: " + std::to_string( read_bytes ) + "b\n", INFO, data->addr->sin_port );
		LOG( "Request:\n" + std::string( buf ), DEBUG, data->addr->sin_port );
	}

}


// MARK: - Write

void	Network::write_socket( int kq, struct kevent &event ) {

	t_udata	*data = ( t_udata * )(event.udata);

	if ( event.flags & EV_EOF || data->flag ) {	
		
		LOG( "EOF WRITE", INFO, data->addr->sin_port );
		EV_SET( &event, event.ident, EVFILT_WRITE, EV_DELETE, 0, 0, data );
		CHECK( kevent( kq, &event, 1, NULL, 0, NULL ), "kevent: can't init EV_DELETE flag for EVFILT_WRITE filter" );
		
		if ( data->flag )
			close( event.ident );
		else data->flag = 1;
		
	} else { 

		if ( !data->req->isReady() ) return;			// Спрашиваю готов ли ответ
		send_msg( event, data );

	}

}

void	Network::send_msg( struct kevent &event, t_udata *data ) {

	std::string response = data->req->getResponse();	// Получаю ответ для отправки

	send( event.ident, response, response.length(), 0 );
	LOG( "Write: " + std::to_string( response ) + "b\n", INFO, data->addr->sin_port );
	LOG( "Response:\n" + msg, DEBUG, data->addr->sin_port );

}


// MARK: - Utils

int	Network::is_listen_socket( struct kevent *kset, int fd, int len ) {
	for ( int i = 0; i < len; ++i, ++kset )
		if ( kset->ident == ( unsigned long )fd ) return 1;
	return 0;
}

t_udata	*Network::init_udata( struct sockaddr_in *addr ) {
	t_udata				*udata = new t_udata;

	udata->flag = 0;
	udata->addr = addr;
	udata->req = new Request( _conf );					// Инициализирую Request( conf )
	return udata;
}

// MARK: - Class Methods

void	Network::start( void ) {
	int 			kq;
	int				len = _conf.servers.size();
	struct kevent	*kset;

	kq = kqueue();

	kset = ( struct kevent * )malloc( sizeof( struct kevent ) * len );
	std::vector< ServerConfig >::iterator	it = _conf.servers.begin();
	for ( int i = 0; it != _conf.servers.end(); ++it, ++i ) {
		Socket	socket = Socket( it->host, it->port );
		if ( socket.start() ) {
			EV_SET( kset + i, socket.get_sock_fd(), EVFILT_READ, EV_ADD, 0, 0, NULL );
		}
	}
	CHECK( kevent( kq, kset, _conf.servers.size(), NULL, 0, NULL ), "kevent: can't init listen sockets events" );

	watch_loop( kq, kset, _conf.servers.size() );
}
