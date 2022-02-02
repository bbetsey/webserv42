#include "Network/Network.hpp"
#include "Config/Config.hpp"
#include "Handler/Request.hpp"

int main( void )
{
    Config  conf = Config::getTestConfig();

    std::cout << conf;

    Network network( conf );
    network.start();
}
