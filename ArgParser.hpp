#ifndef ARGPARSER_HPP
#define ARGPARSER_HPP

#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include "ErrorCode.hpp"

class ArgParser {
private:
	int			port;
	std::string	password;
public:
	ArgParser();
	ArgParser(int ac, char **av);
	~ArgParser();

	/* arg */
	void		setArg(int ac, char **av);

	/* port */
	int			getPort() const;
	void		setPort(char *s);
	int			checkPort(char *s);

	/* password */
	std::string	getPassword() const;
	void		setPassword(char *s);
	std::string	checkPassword(char *s);

	/* exception */
	class ArgException : public std::exception {
	private:
		ErrorCode code;
	public:
		ArgException(ErrorCode code) : code(code) {}
		virtual const char*	 what() const throw() {
			switch (code) {
				case ERR_ARG_COUNT:
					return "Invalid argument count. (only 3)";
				case ERR_PORT_NULL:
					return "Port number is NULL or empty.";
				case ERR_PORT_DIGIT:
					return "Port number can only contain number.";
				case ERR_PORT_RANGE:
					return "Port number is out of valid range.";
				case ERR_PASSWORD_NULL:
					return "Password is NULL or empty.";
				case ERR_PASSWORD_SIZE:
					return "Password must be less than 10 characters.";
				case ERR_PASSWORD_ALNUM:
					return "Password can only contain alphabet or number.";
				default:
					return "Invaid arguments";
			}
		}
	};
};

#endif