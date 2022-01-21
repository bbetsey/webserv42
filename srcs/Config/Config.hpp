#pragma once

#include <iostream>
#include <vector>

class Location
{
	public:
		std::string							_path;
		std::string							_root;
		std::string							_index;
		std::string							_cgi_path;
		std::string							_cgi_ext;
		std::string							_alias;
		std::vector<std::string>			_methods;
		int									_autoindex;
		int									_max_body_size;
		std::vector<Location>				_locations;
};

class ServerConfig
{
	public:
		std::string					_host;
		std::string					_name;
		std::string					_port;
		std::string					_error_page;
		std::vector<Location>		_locations;

		Location getLocation(const std::string &path) const
		{
			(void)path;
			return this->_locations[0];
		}
};

class Config
{
    public:
        Config(std::string &path); // argv[1]
		Config(void) // debug
		{
			this->_error_page = "";
			this->_root = "/";
			
			ServerConfig srvCfg;
			srvCfg._host = "127.0.0.1";
			srvCfg._port = "3490";
			srvCfg._name = "webserv";
			
			Location loc;
			loc._path = "/";
			loc._root = "/";
			loc._methods.push_back("GET"); loc._methods.push_back("POST"); loc._methods.push_back("DELETE");
			loc._max_body_size = 8192;
			loc._index = "index.html";
			loc._cgi_path = "ubuntu_cgi_tester";
			loc._cgi_ext = "";
			loc._autoindex = 0;
			loc._alias = "";

			srvCfg._locations.push_back(loc);
			this->_servers.push_back(srvCfg);
		}

        std::vector<ServerConfig>	_servers;
		std::vector<ServerConfig>	_serversConfigs;
        std::string					_error_page;
        std::string					_root;
};
