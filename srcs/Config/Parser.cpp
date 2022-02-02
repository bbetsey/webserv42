#include "Parser.hpp"


// MARK: - Class Constructor

Parser::Parser( const std::string &path ) : path( path ) {}


// MARK: - Class Distructor

Parser::~Parser( void ) {}


// MARK: - Class Private Methods

std::vector< std::vector< std::string > >	Parser::reader( void ) {
	int			fd;
	char		buf[1];
	int			ret;
	std::vector< std::vector< std::string > >	result;

	if ( ( fd = open( path.c_str(), O_RDONLY ) ) <= 0) {
		LOG( "parser: can't open file", ERROR, 0 );
		exit( 0 );
	}

	std::vector< std::string >	line;
	std::string	word;

	for ( ret = 1; ret > 0; ret = read( fd, buf, 1 ) ) {
		if ( !is_whitespace( buf[0] ) ) {
			if ( buf[0] == '\n' ) {
				line.push_back( word );
				result.push_back( line );
				line.clear(); word.clear();
			} else if ( buf[0] == '{' || buf[0] == '}' || buf[0] == ';' ) {
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

	if ( ret < 0 ) {
		LOG( "parser: reading file error", ERROR, 0 );
		exit( 0 );
	}

	readerTest( result );
	return result;
}

void	Parser::parse( std::vector< std::vector< std::string > > line ) {
	std::vector< std::vector< std::string > >::iterator it = line.begin();

	for( ; it != line.end(); ++it ) {
		std::vector<std::string>::iterator word = it.base()->begin();
		for( int i = 0; word != it.base()->end(); ++word, ++i ) {
			if ( *word == "server" || *word == "?server" ) {
				if ( *(++word) == "{" && ++word == it.base()->end() )
					conf.servers.push_back( getServerConfig( line, ++i ));
				else {
					LOG( "parser: wrong config", ERROR, 0 );
					exit( 0 );
				}
			} else if ( *word == "error_page" && ++word != it.base()->end() ) {
				fillErrorPage( *it, conf.error_pages );
			} else if ( *word == "root" && ++word != it.base()->end() ) {
				fillRoot( *it, conf.root );
			} else {
				LOG( "parser: wrong config", ERROR, 0 );
				exit( 0 );
			}
		}
	}
}


// MARK: - Utils

bool	Parser::is_whitespace( char c ) {
	if ( c == ' ' || c == '\t' )
		return true;
	return false;
}

ServerConfig	Parser::getServerConfig( std::vector< std::vector< std::string > > file, int &i ) {
	ServerConfig	svrcfg;

	for ( ; i != (int)file.size(); ++i ) {
		std::vector< std::string >::iterator	it = file[i].begin();
		for ( ; it != file[i].end(); ++it ) {
			if ( *it == "ip" )
				fillSingleField( svrcfg.host, file[i] );
			else if ( *it == "port" )
				fillSingleField( svrcfg.port, file[i] );
			else if ( *it == "server_name" )
				fillSingleField( svrcfg.name, file[i] );
			else if ( *it == "location" )
				getLocationConfig( svrcfg.locations, file, i );
		}
	}
	return svrcfg;
}

void	Parser::fillSingleField( std::string &data, std::vector< std::string > line ) {
	if ( line.size() != 3 ) {
		LOG( "parser: wrong config", ERROR, 0 );
		exit( 0 );
	}
	data = line[1];
}

void	Parser::getLocationConfig( std::vector< Location > &locations, std::vector< std::vector< std::string > > file, int &i ) {
	Location	loc;

	for ( ; i != file.size(); ++i ) {
		std::vector< std::string >::const_iterator	it = file[i].begin();
		
	}
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
