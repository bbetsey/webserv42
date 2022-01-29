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

ServerConfig	ServerConfig::getTestConfig( std::string port ) {
	ServerConfig	srvCfg;
	Location		loc = Location::getTestConfig();

	srvCfg.host = "localhost";
	srvCfg.port = port;
	srvCfg.name = "webserv";
	srvCfg.locations.push_back( loc );

	return srvCfg;
}



// MARK: - Config Static Struct Methods

Config	Config::getTestConfig( void ) {
	Config			cfg;

	cfg.error_page = "";
	cfg.root = "/";
	cfg.servers.push_back( ServerConfig::getTestConfig( "3490" ) );
	cfg.servers.push_back( ServerConfig::getTestConfig( "3300" ) );
	cfg.servers.push_back( ServerConfig::getTestConfig( "3000" ) );

	return cfg;
}
