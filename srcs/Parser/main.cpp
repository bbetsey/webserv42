#include "Response.hpp"

#include <iostream>

int main(void)
{
    std::string tmp = "GET /index.html HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.tutorialspoint.com\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: Keep-Alive\r\n";

    std::cout << Request(tmp).debug();
}
