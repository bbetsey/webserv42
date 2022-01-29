#include "Config.hpp"
#include "Parser.hpp"


int	main( void ) {
	Parser	parser( "../../configs/configFile1.file" );

	parser.reader();
	return 0;
}
