#include "Response.hpp"

Response::Response(const Request &req)
{
    (void)req;
}

Response::~Response(void)
{
}

std::ostream& operator<<(std::ostream &os, const Response &rep)
{
    (void)rep;
    return os;
}
