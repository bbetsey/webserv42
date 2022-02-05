#include "Network/Network.hpp"
#include "Config/Config.hpp"
#include "Config/Parser.hpp"
#include "Handler/Request.hpp"

int main( void )
{
    // if (argc != 2)
    // {
    //     LOG("Wrong arguments!", ERROR, 0);
    //     exit(1);
    // }
    // Parser  parser(argv[1]);
    // parser.parse();

    // Network network( Config::getTestConfig() );
    // network.start();
    Config cfg = Config::getTestConfig();
    Network(cfg).start();
}
