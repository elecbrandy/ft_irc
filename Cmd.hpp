#ifndef CMD_HPP
# define CMD_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <sstream>
#include "Client.hpp"
#include "Channel.hpp"
#include "ErrorCode.hpp"
#include "ReplyCode.hpp"

class Cmd {
private:
	int			client_fd;
	std::string	&cmdParams;
public:
	Cmd();
	~Cmd();

	std::string extractCmd();
	std::string extractCmdParams(size_t cmdSize);
	void 	handleClientCmd(int client_fd);
	Client* getClient(int client_fd);

	void cmdUser(std::string &cmdParams, int client_fd);
	void cmdNick(std::string &cmdParams, int client_fd);
	void cmdPass(std::string &cmdParams, int client_fd);
	void cmdPing(std::string &cmdParams, int client_fd);
	void cmdJoin(std::string &cmdParams, int client_fd);
	std::vector<std::string> joinSplit(std::string &msg);
	std::string makeMsg(std::string msg);
	void checkConnections();
};

#endif
