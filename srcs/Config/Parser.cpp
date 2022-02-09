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

	conf.autoindex = -1;
	conf.max_body_size = -1;
	
	for ( size_t i = 0; i != file.size(); ++i ) {
		if ( file[i] == "server" || file[i] == "?server" )
			getServerConfig( file, i );
		else if ( file[i] == "error_page" )
			getErrorPage( conf.error_pages, file, i );
		else if ( file[i] == "root" )
			getSingleField( conf.root, file, i );
		else if ( file[i] == "index" )
			getMultipleField( conf.index, file, i );
		else if ( file[i] == "autoindex" )
			getAutoIndex( conf.autoindex, file, i );
		else if ( file[i] == "max_body_size" )
			getBodySize( conf.max_body_size, file, i );
	}
	inherit();
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

	svr_conf.autoindex = -1;
	svr_conf.max_body_size = -1;

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
		else if ( file[i] == "root" )
			getSingleField( svr_conf.root, file, i );
		else if ( file[i] == "index" )
			getMultipleField( svr_conf.index, file, i );
		else if ( file[i] == "autoindex" )
			getAutoIndex( svr_conf.autoindex, file, i );
		else if ( file[i] == "max_body_size" )
			getBodySize( svr_conf.max_body_size, file, i );
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

	loc.autoindex = -1;
	loc.max_body_size = -1;
	loc.authorization = 0;

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
		else if ( file[i] == "authorization" )
			getAutoIndex( loc.authorization, file, i );
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

static bool isParam (std::string line)
{
    if (line.length() > 0 && isdigit(atoi(line.c_str())))
        return true;
    return false;
}

void	Parser::getErrorPage( std::map< int, std::string > &error_pages, const std::vector< std::string > &file, size_t &i ) {
	++i;

	int j = i;
	while ( file[j] != ";" && !check_key_word( file[j] ) )
		++j;
	std::string path = file[--j];

	while ( 1 )
	{
		if (isParam(file[i]))
			error_pages[std::atoi(file[i++].c_str())] = path;
		else
			break;
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
		size = atoi( file[i].c_str() );
		if ( file[++i] != ";" ) {
			LOG( "parser: wrong config", ERROR, 0 );
			exit( 0 );
		}
	} catch ( std::exception ) {
		return;
	}
}

void	Parser::inherit( void ) {
	std::vector< ServerConfig >::iterator	it = conf.servers.begin();
	for ( ; it != conf.servers.end(); ++it ) {
		if ( conf.error_pages.size() > 0 ) {
			std::map< int, std::string >::iterator itconf = conf.error_pages.begin();
			for ( ; itconf != conf.error_pages.end(); ++itconf )
				it->error_pages[itconf->first] = itconf->second;
		}
		if ( it->root.length() == 0 && conf.root.length() > 0 )
			it->root = conf.root;
		if ( it->autoindex == -1 && conf.autoindex != -1 )
			it->autoindex = conf.autoindex;
		if ( it->max_body_size == -1 && conf.max_body_size != -1 )
			it->max_body_size = conf.max_body_size;
		if ( it->index.size() == 0 && conf.index.size() > 0 )
			it->index = conf.index;
		inherit( *it );
	}
}

void	Parser::inherit( ServerConfig &server ) {
	std::vector< Location >::iterator it = server.locations.begin();
	for ( ; it != server.locations.end(); ++it ) {
		if ( it->root.length() == 0 && server.root.length() > 0 )
			it->root = server.root;
		if ( server.error_pages.size() > 0 ) {
			std::map< int, std::string >::iterator itsvr = server.error_pages.begin();
			for ( ; itsvr != server.error_pages.end(); ++itsvr )
				it->error_pages[itsvr->first] = itsvr->second;
		}
		if ( it->autoindex == -1 && server.autoindex != -1 )
			it->autoindex = server.autoindex;
		if ( it->max_body_size == -1 && server.max_body_size != -1 )
			it->max_body_size = server.max_body_size;
		if ( it->index.size() == 0 && server.index.size() > 0 )
			it->index = server.index;
		if ( it->locations.size() > 0 )
			inherit( *it );
	}
}

void	Parser::inherit( Location &loc ) {
	std::vector< Location >::iterator it = loc.locations.begin();
	for ( ; it != loc.locations.end(); ++it ) {
		if ( it->root.length() == 0 && loc.root.length() > 0 )
			it->root = loc.root;
		if ( it->index.size() == 0 && loc.index.size() > 0 )
			it->index = loc.index;
		if ( it->methods.size() == 0 && loc.methods.size() > 0 )
			it->methods = loc.methods;
		if ( it->autoindex == -1 && loc.autoindex != -1 )
			it->autoindex = loc.autoindex;
		if ( it->max_body_size == -1 && loc.max_body_size != -1 )
			it->max_body_size = loc.max_body_size;
		if ( it->cgi_path.length() == 0 && loc.cgi_path.length() > 0 )
			it->cgi_path = loc.cgi_path;
		if ( it->cgi_ext.length() == 0 && loc.cgi_ext.length() > 0 )
			it->cgi_ext = loc.cgi_ext;
		if ( it->alias.length() == 0 && loc.alias.length() > 0 )
			it->alias = loc.alias;
		if ( loc.error_pages.size() > 0 ) {
			std::map< int, std::string >::iterator itconf = loc.error_pages.begin();
			for ( ; itconf != loc.error_pages.end(); ++itconf )
				it->error_pages[itconf->first] = itconf->second;
		}
		if ( it->locations.size() > 0 )
			inherit( *it );
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
