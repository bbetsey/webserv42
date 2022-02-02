#ifndef PARSER_HPP
# define PARSER_HPP

#include <iostream>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include "Config.hpp"
#include "../Includes/webserv.hpp"
#include "../Includes/webserv.hpp"


class Parser {

	private:

		std::string const	path;
		Config				conf;

		

		bool	is_whitespace( char c );
		void	parse( std::vector< std::vector< std::string > > file );
		void	fillSingleField( std::string &data, std::vector< std::string > line );
		void	getLocationConfig( std::vector< Location > &locations, std::vector< std::vector< std::string > > file, int &i );

		ServerConfig	getServerConfig( std::vector< std::vector< std::string > > file, int &i );
		void			fillErrorPage( std::vector< std::string > line, std::map< int, std::string > error_pages );
		void			fillRoot( std::vector< std::string > line, std::string &root );

		void	readerTest( std::vector< std::vector< std::string > > file );
		void	parserTest( const Config &conf );
	
	public:

		Parser( const std::string &path );
		~Parser( void );

		Config	getConfig( void );

		std::vector< std::vector< std::string > > reader( void );

};


#endif
