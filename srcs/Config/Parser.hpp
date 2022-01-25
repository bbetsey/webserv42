#ifndef PARSER_HPP
# define PARSER_HPP

#include <iostream>
#include "Config.hpp"


class Parser {

	private:

		std::string const	path;
		Config				conf;

	
	public:

		Parser( const std::string &path );
		~Parser( void );

		Config	getConfig( void );

};


#endif
