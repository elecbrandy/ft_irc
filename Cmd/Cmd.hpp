#ifndef CMD_HPP
# define CMD_HPP

#include "../Server.hpp"
#include "../Color.hpp"

#define NICK_MAX_LEN 20
#define USERNAME_MAX_LEN 100
#define REALNAME_MAX_LEN 100

class IrcServer;
class Client;

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
	void		authorizeClient();
	bool		handleClientCmd();

	/* Commands */
	void		cmdCap();
	void		cmdUser();
	void		cmdNick();
	void		cmdPass();
	void		cmdPong();
	void		cmdJoin();
	void		cmdMode();
	void		cmdTopic();
	void		cmdPrivmsg();
	void		cmdQuit();
	void		cmdKick();
	void		cmdPart();
	void		cmdInvite();

	/* Utils */
	void checkNick(const std::string& str);
	void checkPassword(const std::string& str);
	void checkUsername(const std::string& str);
	void checkRealname(const std::string& str);
	// std::vector<std::string> joinSplit(std::string &msg);
	bool isValidChannelName(std::vector<std::string> &channel);
	bool isDupReceiver(std::vector<std::string> &receivers);
	std::vector<std::string> topicSplit();
	std::vector<std::string> joinSplit(std::string &cmdParams);
	std::vector<std::string> privmsgSplit();
	std::vector<std::string> split(char delim);

	/* Setter & Getter */
	std::string getCmdParams() const;

	void validationKey(std::string key);
	void validationInt(std::string _size);
	void addChannelOperator(std::string nickname, Channel* channel);
	void setChannelKey(std::string key, Channel* channel);
	void setChannelUserLimit(std::string _size, Channel* channel);

	void validationNickName(std::string nickname, Channel* channel, int option_flag);
	void removeChannelOperator(std::string nickname, Channel* channel);
	void removeChannelKey(Channel* channel);
	void removeChannelUserLimit(Channel* channel);
	void removeParticipant(std::string target);

	void handleMinusFlagOption(std::vector<std::string> modeParse, std::map<std::string, Channel*>::iterator channel, int option_flag);
	void handlePlusFlagOption(std::vector<std::string> modeParse, std::map<std::string, Channel*>::iterator channel, int option_flag);


	/* Exception */
	class CmdException : public std::exception {
	private:
		std::string msg;
	public:
		CmdException(const std::string& str) : msg(str) {}
		virtual ~CmdException() throw() {}
		const char* what() const throw();
	};
};

#endif
