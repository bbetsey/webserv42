#include "Network/Network.hpp"
#include "Config/Config.hpp"
#include "Config/Parser.hpp"
#include "Handler/Request.hpp"

int main( void )
{
    Parser  parser( "configs/configFile1.file" );
    parser.parse();

    Network network( parser.getConfig() );
    network.start();
}
