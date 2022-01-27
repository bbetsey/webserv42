#include "../Includes/webserv.hpp"
#include "Logger.hpp"


// Logger	*Logger::getInstance( void ) {
// 	if ( !logger )
// 		logger = new Logger();
// 	return logger;
// }

void	Logger::writeLog( std::string log, LogLevel level ) {
	struct tm	*tm;
    time_t		rawtime;
    char		buf[32];

    time(&rawtime);
    tm = localtime (&rawtime);
    int ret = strftime(buf, 32, "%T", tm);
    buf[ret] = '\0';

	std::cout << "[" << buf << "] ";

	switch ( level ) {
		case INFO:
			std::cout << GREEN << "INFO: ";
			break;
		case WARN:
			std::cout << CYAN << "WARN: ";
			break;
		default:
			std::cout << RED << "ERROR: ";
			break;
	}

	std::cout << log << RESET << std::endl;
}
