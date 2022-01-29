#include "../Includes/webserv.hpp"
#include "Logger.hpp"


// Logger	*Logger::getInstance( void ) {
// 	if ( !logger )
// 		logger = new Logger();
// 	return logger;
// }

void	Logger::writeLog( std::string log, LogLevel level, int port ) {
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
			if ( port ) std::cout << GREEN << "INFO:  [port: " << port << "] ";
			else std::cout << GREEN << "INFO:  ";
			break;
		case DEBUG:
			if ( port ) std::cout << CYAN << "DEBUG: [port: " << port << "] ";
			else std::cout << CYAN << "DEBUG: ";
			break;
		default:
			if ( port ) std::cout << RED << "ERROR: [port: " << port << "] ";
			else std::cout << RED << "ERROR: ";
			break;
	}

	std::cout << log << RESET << std::endl;
}
