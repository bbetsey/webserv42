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
	loc.index.push_back( "index.html" );
	loc.cgi_path = "cgi_tester";
	loc.cgi_ext = "";
	loc.autoindex = 0;
	loc.alias = "";

	return loc;
}


// MARK: - ServerConfig Struct Methods

static std::string dirnameOf(const std::string& fname)
{
     size_t pos = fname.find_last_of("\\/");
     return (std::string::npos == pos) ? "" : fname.substr(0, pos);
}

Location	ServerConfig::getLocation( const std::string &path ) const {
	if (this->locations.size() == 0)
	{
		LOG("No locations in config!", ERROR, 0);
		exit(1);
	}
	std::string dir = dirnameOf(path);
	if (dir == "")
		dir = "/";

	for (size_t i = 0; i < this->locations.size(); i++)
	{
		if (this->locations[i].path == path)
			return (this->locations[i]);
	}
	return (this->locations[0]);
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

	cfg.root = "/";
	cfg.servers.push_back( ServerConfig::getTestConfig( "3490" ) );
	cfg.servers.push_back( ServerConfig::getTestConfig( "3300" ) );
	cfg.servers.push_back( ServerConfig::getTestConfig( "3000" ) );

	return cfg;
}


// MARK; - OSTREAM Overloads

std::ostream	&operator << ( std::ostream &out, const Config &conf ) {
	out << BOLDWHITE << "\nCONFIGURATION\n" << std::endl;
	if ( conf.root.length() > 0 )
		out << "root: " << conf.root << ";" << std::endl;
	if ( conf.index.size() > 0 ) {
		out << "index: ";
		std::vector< std::string >::const_iterator	it = conf.index.begin();
		for ( ; it != conf.index.end(); ++it )
			out << *it << " ";
		out << std::endl;
	}
	if ( conf.error_pages.size() > 0 ) {
		std::map< int, std::string >::const_iterator	it = conf.error_pages.begin();
		for ( ; it != conf.error_pages.end(); ++it )
			out << "error_page " << it->first << " " << it->second << ";" << std::endl;
	}
	if ( conf.autoindex > 0 ) out << "autoindex: on" << std::endl;
	if ( conf.max_body_size > 0 ) out << "max_body_size: " << conf.max_body_size << std::endl;
	if ( conf.servers.size() > 0 ) {
		std::vector< ServerConfig >::const_iterator	it = conf.servers.begin();
		for ( ; it != conf.servers.end(); ++it )
			out << *it;
	}
	out << std::endl;
	return out;
}

std::ostream	&operator << ( std::ostream &out, const ServerConfig &conf ) {
	out << BOLDWHITE << "\nSERVER" << RESET << std::endl;
	if ( conf.host.length() > 0 )
		out << "host: " << conf.host << ";" << std::endl;
	if ( conf.root.length() > 0 )
		out << "root: " << conf.root << ";" << std::endl;
	if ( conf.port.length() > 0 )
		out << "port: " << conf.port << ";" << std::endl;
	if ( conf.name.length() > 0 )
		out << "server_name: " << conf.name << ";" << std::endl;
	if ( conf.error_pages.size() > 0 ) {
		std::map< int, std::string >::const_iterator	it = conf.error_pages.begin();
		for ( ; it != conf.error_pages.end(); ++it )
			out << "error_page " << it->first << " " << it->second << ";" << std::endl;
	}
	if ( conf.autoindex > 0 ) out << "autoindex: on" << std::endl;
	if ( conf.max_body_size > 0 ) out << "max_body_size: " << conf.max_body_size << std::endl;
	if ( conf.index.size() > 0 ) {
		out << "index: ";
		std::vector< std::string >::const_iterator	it = conf.index.begin();
		for ( ; it != conf.index.end(); ++it )
			out << *it << " ";
		out << std::endl;
	}
	if ( conf.locations.size() > 0 ) {
		std::vector< Location >::const_iterator	it = conf.locations.begin();
		for ( ; it != conf.locations.end(); ++it )
			out << *it;
	}
	out << std::endl;
	return out;
}

std::ostream	&operator << ( std::ostream &out, const Location &loc ) {
	out << BOLDWHITE << "\nLOCATION " << RESET;

	if ( loc.path.length() )
		out << loc.path;
	out << std::endl;
	if ( loc.root.length() > 0 )
		out << "root: " << loc.root << ";" << std::endl;
	if ( loc.index.size() > 0 ) {
		out << "index: ";
		std::vector< std::string >::const_iterator	it = loc.index.begin();
		for ( ; it != loc.index.end(); ++it )
			out << *it << " ";
		out << std::endl;
	}
	if ( loc.cgi_path.length() > 0 )
		out << "cgi: " << loc.cgi_path << ";" << std::endl;
	if ( loc.cgi_ext.length() > 0 )
		out << "cgi_extension: " << loc.cgi_ext << ";" << std::endl;
	if ( loc.alias.length() > 0 )
		out << "alias: " << loc.alias << ";" << std::endl;
	if ( loc.methods.size() > 0 ) {
		out << "allow: ";
		std::vector< std::string >::const_iterator	it = loc.methods.begin();
		for ( ; it != loc.methods.end(); ++it )
			out << *it << " ";
		out << std::endl;
	}
	if ( loc.autoindex > 0 ) out << "autoindex: on" << std::endl;
	if ( loc.max_body_size > 0 ) out << "max_body_size: " << loc.max_body_size << std::endl;
	if ( loc.error_pages.size() > 0 ) {
		std::map< int, std::string >::const_iterator	it = loc.error_pages.begin();
		for ( ; it != loc.error_pages.end(); ++it )
			out << "error_page " << it->first << " " << it->second << ";" << std::endl;
	}
	if ( loc.locations.size() > 0 ) {
		std::vector< Location >::const_iterator	it = loc.locations.begin();
		for ( ; it != loc.locations.end(); ++it )
			out << *it;
	}
	out << std::endl;
	return out;
}
