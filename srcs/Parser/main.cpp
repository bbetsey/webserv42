#include "Request.hpp"

#include <iostream>

int main(void)
{
    setMimeTypes();

    std::string testRequest = "GET /index.html HTTP/1.1";

    std::cout << Request(testRequest).response();
}