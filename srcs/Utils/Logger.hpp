#ifndef LOGGER_HPP
# define LOGGER_HPP

#include <iostream>

enum LogLevel {
	INFO,
	DEBUG,
	ERROR
};


class Logger {

	public:

		static void		writeLog( std::string line, LogLevel level, int port = 0 );
		static void		check( int result, std::string error );

};

#endif
