#include "Request.hpp"

#include <iostream>

int main(void)
{
	std::string tmp =   "GET /index.html HTTP/1.1\r\n" \
						"User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n" \
						"Host: www.tutorialspoint.com\r\n" \
						"Accept-Language: en-us\r\n" \
						"Accept-Encoding: gzip, deflate\r\n" \
						"Connection: Keep-Alive\r\n";

	std::cout << Request(tmp).debug();
}
