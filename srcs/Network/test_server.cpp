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
	t_server	server;
	t_conf		conf;

	server.host = HOST;
	server.port = PORT;
	server.name = HOST;
	conf.servers.push_back( server );

	Network	network( conf );
	network.start();
}
