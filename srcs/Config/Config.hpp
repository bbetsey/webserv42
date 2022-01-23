#pragma once

#include <iostream>
#include <vector>


struct Location {

	std::string					path;
	std::string					root;
	std::string					index;
	std::string					cgi_path;
	std::string					cgi_ext;
	std::string					alias;
	std::vector< std::string >	methods;
	std::vector< Location >		locations;
	int							autoindex;
	int							max_body_size;


	// MARK: - Static Struct Methods

	static Location	getTestConfig( void );

};


struct ServerConfig {
	
	std::string					host;
	std::string					name;
	std::string					port;
	std::string					error_page;
	std::vector<Location>		locations;


	// MARK: - struct methods

	Location	getLocation( const std::string &path ) const;


	// MARK: - static struct methods

	static ServerConfig	getTestConfig( void );

};


struct Config {

	std::vector< ServerConfig >	servers;
	std::string					error_page;
	std::string					root;


	// MARK: - Static Struct Methods

	static Config	getTestConfig( void );

};
