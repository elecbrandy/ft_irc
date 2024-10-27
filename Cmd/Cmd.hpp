#ifndef CMD_HPP
# define CMD_HPP

#include "Server.hpp"

#define NICK_MAX_LEN 9

class Cmd {
private:
	IrcServer&	server;
	std::string	msg;
	std::string cmd;
	std::string cmdParams;
	int			client_fd;

public:
	/* Constructor & Destructor */
	Cmd(IrcServer &s);
	Cmd(IrcServer &s, std::string &msg, int client_fd);
	~Cmd();

	/* Extracter & Handler */
	std::string	extractCmd();
	std::string extractCmdParams();
	void		handleClientCmd();

	/* Commands */
	void		cmdUser(std::string &cmdParams, int client_fd);
	void		cmdNick(std::string &cmdParams, int client_fd);
	void		cmdPass(std::string &cmdParams, int client_fd);
	void		cmdPing(std::string &cmdParams, int client_fd);
	void		cmdJoin(std::string &cmdParams, int client_fd);

	/* Utils */
	void checkNick(const std::string& str);
	void checkPassword(const std::string& str);
	std::vector<std::string> joinSplit(std::string &msg);
	std::string makeMsg(std::string msg);
	void checkConnections();

	/* Exception */
	// :irc.example.com 461 guest NICK :Not enough parameters
	class CmdException : public std::exception {
	private:
		std::string msg;
	public:
		explicit CmdException(const std::string& str) : msg(str) {}
		const char* what() const noexcept override {
			return msg.c_str();
		}
	};
};

#endif
