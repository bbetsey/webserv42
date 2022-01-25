#include "Network/Network.hpp"
#include "Config/Config.hpp"
#include "Handler/Request.hpp"

int main(void)
{
    std::string getMethod =
"GET /index.html HTTP/1.1\r\n\
\r\n";

    Config cfg;
    std::cout << Request(getMethod, cfg._servers[0]).response();
}
