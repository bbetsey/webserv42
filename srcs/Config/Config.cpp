#include "Config.hpp"


// MARK: - Location Static Struct Methods

Location	Location::getTestConfig( void ) {
	Location		loc;

	loc.path = "/";
	loc.root = "/";
	loc.methods.push_back("GET");
	loc.methods.push_back("POST");
	loc.methods.push_back("DELETE");
	loc.max_body_size = 8192;
	loc.index = "index.html";
	loc.cgi_path = "ubuntu_cgi_tester";
	loc.cgi_ext = "";
	loc.autoindex = 0;
	loc.alias = "";

	return loc;
}


// MARK: - ServerConfig Struct Methods

Location	ServerConfig::getLocation( const std::string &path ) const {
	(void)path;
	return locations[0];
}


// MARK: - ServerConfig Static Struct Methods

ServerConfig	ServerConfig::getTestConfig( void ) {
	ServerConfig	srvCfg;
	Location		loc = Location::getTestConfig();

	srvCfg.host = "localhost";
	srvCfg.port = "3490";
	srvCfg.name = "webserv";
	srvCfg.locations.push_back( loc );

	return srvCfg;
}



// MARK: - Config Static Struct Methods

Config	Config::getTestConfig( void ) {
	Config			cfg;
	ServerConfig	srvCfg = ServerConfig::getTestConfig();

	cfg.error_page = "";
	cfg.root = "/";
	cfg.servers.push_back( srvCfg );

	return cfg;
}
