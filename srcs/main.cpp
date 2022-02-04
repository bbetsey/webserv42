#include "Network/Network.hpp"
#include "Config/Config.hpp"
#include "Config/Parser.hpp"
#include "Handler/Request.hpp"

int main( int argc, char **argv )
{
    if (argc != 2)
    {
        LOG("Wrong arguments!", ERROR, 0);
        exit(1);
    }

    LOG("wtf", DEBUG, 0);
    Parser  parser(argv[1]);
    parser.parse();
    LOG("wtf", DEBUG, 0);

    Network network( parser.getConfig() );
    network.start();
}
