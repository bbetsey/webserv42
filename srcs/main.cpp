#include "Network/Network.hpp"
#include "Config/Config.hpp"
#include "Handler/Request.hpp"

int main(void)
{
    std::string getMethod =
"POST /test HTTP/1.1\r\n\
Host: foo.example\r\n\
Content-Type: application/x-www-form-urlencoded\r\n\
Content-Length: 27\r\n\
\r\n\
field1=value1&field2=value2\r\n";

    Config cfg;
    std::cout << Request(getMethod, cfg._servers[0]).response();
}
