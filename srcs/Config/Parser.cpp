#include "Parser.hpp"

Parser::Parser( const std::string &path ) : path( path ) {}

Parser::~Parser( void ) {}


std::vector< std::vector< std::string > >	Parser::reader( void ) {
	int			fd;
	char		buf[1];
	int			ret;
	std::vector< std::vector< std::string > >	result;

	if ( ( fd = open( path.c_str(), O_RDONLY ) ) <= 0)
		LOG( "parser: can't open file", ERROR, 0 );

	std::vector< std::string >	line;
	std::string	word = "";

	for ( ret = 1; ret > 0; ret = read( fd, buf, 1 ) ) {
		if ( !is_whitespace( buf[0] ) ) {
			if ( buf[0] == '\n' ) {
				line.push_back( word );
				result.push_back( line );
				line.clear(); word.clear();
			} else if ( buf[0] == '{' || buf[0] == '}' ) {
				if ( word.length() > 0 )
					line.push_back( word );
				word = buf[0]; line.push_back( word );
				result.push_back( line );
				line.clear(); word.clear();
			} else {
				word += buf[0];
			}
		} else {
			if ( word.length() > 0 ) {
				line.push_back( word );
				word.clear();
			}
		}
	}

	CHECK( ret, "reading file error" );
	readerTest( result );

	return result;
}


// MARK: - Utils

bool	Parser::is_whitespace( char c ) {
	if ( c == ' ' || c == '\t' )
		return true;
	return false;
}

void	Parser::readerTest( std::vector< std::vector< std::string > > file ) {
	std::vector< std::vector< std::string > >::iterator it = file.begin();

	for( ; it != file.end(); ++it ) {
		std::vector<std::string>::iterator line = it.base()->begin();
		for( ; line != it.base()->end(); ++line )
			std::cout << *line << " ";
		std::cout << std::endl;
	}
}
