#include "Handler/Response.hpp"

int main(int argc, char **argv)
{
    std::string req = (argc == 2) ? argv[1] : "GET / HTTP/1.1";

    std::cout << Request(req).response();
}
