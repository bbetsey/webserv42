#ifndef LOGGER_HPP
# define LOGGER_HPP

#include <iostream>

enum LogLevel {
	INFO,
	WARN,
	ERROR
};


class Logger {

	public:

		// MARK: - Singleton methods

		static void		writeLog( std::string line, LogLevel level );

};

#endif
