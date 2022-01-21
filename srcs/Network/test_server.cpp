#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

#include "Network.hpp"

#define PORT "3490"
#define HOST "localhost"
#define BACKLOG 10


int	main( void ) {
	Config conf;

	std::cout << conf._servers[0]._port << std::endl;
	Network	network( conf );
	network.start();
}
