#include "Config.hpp"
#include "Parser.hpp"


int	main( void ) {
	Parser	parser( "../../configs/configFile1.file" );

	parser.parse();
	std::cout << parser.getConfig();
	return 0;
}
