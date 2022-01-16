#include "Parser/Response.hpp"

int main(void)
{
	std::string tmp =   "GET test.html HTTP/1.1\r\n";

	std::cout << Request(tmp).response();
}
