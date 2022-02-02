#include "Parser.hpp"


// MARK: - Class Constructor

Parser::Parser( const std::string &path ) : path( path ) {}


// MARK: - Class Distructor

Parser::~Parser( void ) {}


// MARK: - Class Public Methods

Config	Parser::getConfig( void ) {
	return conf;
}

void	Parser::parse( void ) {
	std::vector< std::string >	file = reader();
	
	for ( size_t i = 0; i != file.size(); ++i ) {
		if ( file[i] == "server" || file[i] == "?server" )
			getServerConfig( file, i );
		else if ( file[i] == "error_page" )
			getErrorPage( conf.error_pages, file, i );
		else if ( file[i] == "root" )
			getSingleField( conf.root, file, i );
	}

}


// MARK: - Class Private Methods

std::vector< std::string >	Parser::reader( void ) {
	int							fd;
	char						buf[1];
	int							ret;
	std::vector< std::string >	result;

	if ( ( fd = open( path.c_str(), O_RDONLY ) ) <= 0) {
		LOG( "parser: can't open file", ERROR, 0 );
		exit( 0 );
	}

	std::string	word;
	read( fd, buf, 1 );
	for ( ret = 1; ret > 0; ret = read( fd, buf, 1 ) ){
		if ( !is_whitespace( buf[0] ) ) {
			if ( buf[0] == ';' || buf[0] == '{' || buf[0] == '}' ) {
				if ( word.length() > 0 ) {
					result.push_back( word ); word.clear();
				}
				word += buf[0];
				result.push_back( word ); word.clear();
			} else word += buf[0];
		} else {
			if ( word.length() > 0 ) {
				result.push_back( word );
				word.clear();
			}
		}
	}

	return result;
}


// MARK: - Server Config

void	Parser::getServerConfig( const std::vector< std::string > &file, size_t &i ) {
	ServerConfig	svr_conf;

	if ( file[++i] != "{" ) {
		LOG( "parser: wrong config", ERROR, 0 );
		exit( 0 );
	}

	for ( ++i ; file[i] != "}"; ++i ) {
		if ( file[i] == "ip" )
			getSingleField( svr_conf.host, file, i );
		else if ( file[i] == "port" )
			getSingleField( svr_conf.port, file, i );
		else if ( file[i] == "server_name" )
			getSingleField( svr_conf.name, file, i );
		else if ( file[i] == "error_page" )
			getErrorPage( svr_conf.error_pages, file, i );
		else if ( file[i] == "location" )
			getLocation( svr_conf.locations, file, i );
	}

	conf.servers.push_back( svr_conf );
}


// MARK: - Location Config

void	Parser::getLocation( std::vector< Location > &locations, const std::vector< std::string > &file, size_t &i ) {
	Location	loc;

	loc.max_body_size = -1;
	loc.autoindex = 0;

	for ( ++i; file[i] != "{"; ++i )
		loc.path += file[i];

	for ( ++i; file[i] != "}"; ++i ) {
		if ( file[i] == "root" )
			getSingleField( loc.root, file, i );
		else if ( file[i] == "cgi" )
			getSingleField( loc.cgi_path, file, i );
		else if ( file[i] == "cgi_extension" )
			getSingleField( loc.cgi_ext, file, i );
		else if ( file[i] == "alias" )
			getSingleField( loc.alias, file, i );
		else if ( file[i] == "autoindex" )
			getAutoIndex( loc.autoindex, file, i );
		else if ( file[i] == "max_body_size" )
			getBodySize( loc.max_body_size, file, i );
		else if ( file[i] == "index" )
			getMultipleField( loc.index, file, i );
		else if ( file[i] == "allow" )
			getMultipleField( loc.methods, file, i );
		else if ( file[i] == "location" )
			getLocation( loc.locations, file, i );
	}
	locations.push_back( loc );
}


// MARK: - Different Fields

void	Parser::getSingleField( std::string &data, const std::vector< std::string > &file, size_t &i ) {
	data = file[++i];
	if ( file[++i] != ";" ) {
		LOG( "parser: wrong config", ERROR, 0 );
		exit( 0 );
	}
}

void	Parser::getErrorPage( std::map< int, std::string > &error_pages, const std::vector< std::string > &file, size_t &i ) {
	++i;

	int j = i;
	while ( file[j] != ";" && !check_key_word( file[j] ) )
		++j;
	std::string path = file[--j];
	int			key;

	while ( 1 ) {
		try {
			key = std::stoi( file[i++] );
			error_pages[key] = path;
		} catch ( std::exception ) {
			break;
		}
	}
}

void	Parser::getMultipleField( std::vector< std::string > &data, const std::vector< std::string > &file, size_t &i ) {
	for ( ++i; file[i] != ";"; ++i )
		data.push_back( file[i] );
}

void	Parser::getAutoIndex( int &autoindex, const std::vector< std::string > &file, size_t &i ) {
	
	++i;
	if ( file[i] == "on" ) autoindex = 1;
	else if ( file[i] == "off" ) autoindex = 0;
	else {
		LOG( "parser: wrong config", ERROR, 0 );
		exit( 0 );
	}
	if ( file[++i] != ";" ) {
		LOG( "parser: wrong config", ERROR, 0 );
		exit( 0 );
	}
}

void	Parser::getBodySize( int &size, const std::vector< std::string > &file, size_t &i ) {

	++i;
	try {
		size = std::stoi( file[i] );
		if ( file[++i] != ";" ) {
			LOG( "parser: wrong config", ERROR, 0 );
			exit( 0 );
		}
	} catch ( std::exception ) {
		return;
	}
}


// MARK: - Utils

bool	Parser::is_whitespace( char c ) {
	if ( c == ' ' || c == '\t' || c == '\n' )
		return true;
	return false;
}

bool	Parser::check_key_word( const std::string &word ) {
	if ( word == "ip" || word == "root" || word == "index" || word == "cgi" || word == "cgi_extension" || word == "alias" || word == "allow" || word == "location" || word == "autoindex" || word == "max_body_size" || word == "port" || word == "server_name" )
		return true;
	return false;
}

void	Parser::readerTest( const std::vector< std::string > &file ) {
	std::vector< std::string >::const_iterator it = file.begin();

	for( ; it != file.end(); ++it ) {
		std::cout << *it << " ";
	}
}
