#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

#include "Network.hpp"
#include "../Config/Config.hpp"
#include "../Includes/webserv.hpp"

#define PORT "3490"
#define HOST "localhost"
#define BACKLOG 10


int	main( void ) {
	Config conf = Config::getTestConfig();

	Network	network( conf );
	network.start();
}
