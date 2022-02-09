#include "../Includes/webserv.hpp"
#include "Logger.hpp"
#include <stdio.h>

void	Logger::writeLog( std::string log, LogLevel level, int port ) {
	(void)log; (void)level; (void)port;
	struct tm	*tm;
    time_t		rawtime;
    char		buf[32];

	fflush(NULL);
    time(&rawtime);
    tm = localtime (&rawtime);
    int ret = strftime(buf, 32, "%T", tm);
    buf[ret] = '\0';

	std::cout << "[" << buf << "] ";

	switch ( level ) {
		case INFO:
			if ( port ) std::cout << GREEN << "INFO:  " << BOLDGREEN << "[port: " << port << "] " << RESET << GREEN;
			else std::cout << GREEN << "INFO:  ";
			break;
		case DEBUG:
			if ( port ) std::cout << CYAN << "DEBUG: " << BOLDCYAN << "[port: " << port << "] " << RESET << CYAN;
			else std::cout << CYAN << "DEBUG: ";
			break;
		default:
			if ( port ) std::cout << RED << "ERROR: " << BOLDRED << "[port: " << port << "] " << RESET << RED;
			else std::cout << RED << "ERROR: ";
			break;
	}

	std::cout << log << RESET << std::endl;
	fflush(NULL);
}

void	Logger::check( int result, std::string error ) {
	if ( result == -1 )
		std::cout << RED << "ERROR: " << error << RESET << std::endl;
}
