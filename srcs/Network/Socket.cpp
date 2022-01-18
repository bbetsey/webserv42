#include "Socket.hpp"


// MARK: - Class Construtor

Socket::Socket( std::string host, std::string port ) : _host( host ), _port( port ), _res( NULL ) {}


// MARK: - Class Distructor

Socket::~Socket( void ) { if ( _res ) freeaddrinfo( _res ); }


// MARK: - Class Methods

int	Socket::start( void ) {
	int				status;
	struct addrinfo	hints;

	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	errno = 0;

	if ( ( status = getaddrinfo( _host.c_str(), _port.c_str(), &hints, &_res ) ) != 0 ) {
		std::cout << "socket getaddrinfo error: " << gai_strerror( status ) << std::endl;
		return 0;
	}

	if ( ( _sockfd = socket( _res->ai_family, _res->ai_socktype, _res->ai_protocol ) ) == -1 ) {
		std::cout << "socket fd error: " << std::string( strerror( errno ) ) << std::endl;
		return 0;
	}

	if ( bind( _sockfd, _res->ai_addr, _res->ai_addrlen ) == - 1 ) {
		std::cout << "socket bind error: " << std::string( strerror( errno ) ) << std::endl;
		return 0;
	}

	if ( listen( _sockfd, BACKLOG ) == -1 ) {
		std::cout << "socket listen error: " << std::string( strerror( errno ) ) << std::endl;
		return 0;
	}

}


// MARK: - Getters

int			Socket::get_sock_fd( void )	{ return _sockfd; }
std::string	Socket::get_host( void )	{ return _host; }
std::string	Socket::get_port( void )	{ return _host; }
