#ifndef CMD_HPP
# define CMD_HPP

#include "../Server.hpp"
#include "../Color.hpp"

#define NICK_MAX_LEN 15

class IrcServer;

class Cmd {
private:
	IrcServer&	server;
	int			client_fd;
	Client*		client;
	std::string	msg;
	std::string cmd;
	std::string cmdParams;

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
	void		cmdCap();
	void		cmdUser(std::string &cmdParams, int client_fd);
	void		cmdNick();
	void		cmdPass();
	void		cmdPing();
	// void		cmdJoin(std::string &cmdParams, int client_fd);

	/* Utils */
	void checkNick(const std::string& str);
	void checkPassword(const std::string& str);
	// std::vector<std::string> joinSplit(std::string &msg);

	/* Exception */
	class CmdException : public std::exception {
	private:
		std::string msg;
	public:
		explicit CmdException(const std::string& str) : msg(str) {}
		virtual ~CmdException() throw() {}
		const char* what() const throw() {
			return msg.c_str();
		}
	};
};

#endif
