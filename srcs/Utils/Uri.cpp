#include "Uri.hpp"

Uri::Uri(std::string uri)
{
    if (uri.length() == 0)
        return;

    std::string::iterator uriEnd = uri.end();

    std::string::iterator queryStart = std::find(uri.begin(), uriEnd, '?');

    std::string::iterator protocolStart = uri.begin();
    std::string::iterator protocolEnd = std::find(protocolStart, uriEnd, ':');  

    if (protocolEnd != uriEnd)
    {
        std::string prot = &*(protocolEnd);
        if ((prot.length() > 3) && (prot.substr(0, 3) == "://"))
        {
            this->_protocol = std::string(protocolStart, protocolEnd);
            protocolEnd += 3;
        }
        else
            protocolEnd = uri.begin();
    }
    else
        protocolEnd = uri.begin();

    std::string::iterator hostStart = protocolEnd;
    std::string::iterator pathStart = std::find(hostStart, uriEnd, '/');
    std::string::iterator hostEnd = std::find(protocolEnd, (pathStart != uriEnd) ? pathStart : queryStart, L':');

    this->_host = std::string(hostStart, hostEnd);

    if ((hostEnd != uriEnd) && ((&*(hostEnd))[0] == ':'))
    {
        hostEnd++;
        std::string::iterator portEnd = (pathStart != uriEnd) ? pathStart : queryStart;
        this->_port = std::string(hostEnd, portEnd);
    }

    if (pathStart != uriEnd)
        this->_path = std::string(pathStart, queryStart);

    if (queryStart != uriEnd)
        this->_qString = std::string(queryStart, uri.end());
}
