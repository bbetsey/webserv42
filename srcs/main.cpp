#include "Parser/Response.hpp"

int main(int argc, char **argv)
{
	std::string tmp = (argc == 2) ? argv[1] : "GET test.html HTTP/1.1\r\n";
	std::cout << Request(tmp).response();
}
