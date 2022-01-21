#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>

int	main( int argc, char **argv ) {
	struct addrinfo	hints, *res, *p;
	int				status;
	char			ipstr[INET6_ADDRSTRLEN];

	if ( argc != 2 ) {
		std::cout << "usage: ./showip hostname" << std::endl;
		return 1;
	}

	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ( ( status = getaddrinfo( argv[1], NULL, &hints, &res ) ) != 0 ) {
		std::cout << "getaddrinfo error: " << gai_strerror( status ) << std::endl;
		return 2;
	}

	std::cout << "IP address for " << argv[1] << std::endl;

	for ( p = res; p != NULL; p = p->ai_next ) {
		void		*addr;
		std::string ipver;

		if ( p->ai_family == AF_INET ) {
			struct sockaddr_in	*ipv4 = ( struct sockaddr_in * )p->ai_addr;
			addr = &( ipv4->sin_addr );
			ipver = "IPv4";
		} else {
			struct sockaddr_in6	*ipv6 = ( struct sockaddr_in6 * )p->ai_addr;
			addr = &( ipv6->sin6_addr );
			ipver = "IPv6";
		}

		inet_ntop( p->ai_family, addr, ipstr, sizeof( ipstr ) );
		std::cout << ipver << ": " << ipstr << std::endl;
	}

	freeaddrinfo( res );
	return 0;
}
