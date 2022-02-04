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


		std::vector< std::string >	reader( void );

		bool	is_whitespace( char c );
		bool	check_key_word( const std::string &word );

		void	getServerConfig( const std::vector< std::string > &file, size_t &i );
		void	getLocation( std::vector< Location > &locations, const std::vector< std::string > &file, size_t &i );
		void	getErrorPage( std::map< int, std::string > &error_pages, const std::vector< std::string > &file, size_t &i );
		void	getSingleField( std::string &data, const std::vector< std::string > &line, size_t &i );
		void	getMultipleField( std::vector< std::string > &data, const std::vector< std::string > &file, size_t &i );
		void	getAutoIndex( int &autoindex, const std::vector< std::string > &file, size_t &i );
		void	getBodySize( int &size, const std::vector< std::string > &file, size_t &i );

		void	inherit( void );
		void	inherit( ServerConfig &server );
		void	inherit( Location &loc );

		void	readerTest( const std::vector< std::string > &file );
	
	public:

		Parser( const std::string &path );
		~Parser( void );

		Config	getConfig( void );
		void	parse( void );

};


#endif
