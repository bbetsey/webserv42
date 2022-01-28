#include "Network/Network.hpp"
#include "Config/Config.hpp"
#include "Handler/Request.hpp"

int main(void)
{
	Config conf = Config::getTestConfig();

	Network	network( conf );
	network.start();

}
