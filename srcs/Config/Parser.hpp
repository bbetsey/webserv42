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
		void	readerTest( std::vector< std::vector< std::string > > file );

	
	public:

		Parser( const std::string &path );
		~Parser( void );

		Config	getConfig( void );

		std::vector< std::vector< std::string > > reader( void );

};


#endif
