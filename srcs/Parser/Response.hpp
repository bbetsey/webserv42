#pragma once

#include "Request.hpp"

class Response
{
    public:
        Response(const Request &req);
        ~Response(void);
    private:

};

std::ostream& operator<<(std::ostream &os, const Response &rep);