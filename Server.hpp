#ifndef SERVER_HPP
# define SERVER_HPP

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

#define MAX_CLIENTS 42
#define BUFFER_SIZE 42

#define EXIT 1
#define UNEXIT 0

#define CRLF "\r\n"
#define PING_INTERVAL 120

class IrcServer {
private:
	int									server_fd;
	int									port;
	std::string							password;
	std::vector<struct pollfd>			fds;
	std::map<int, Client*>				_clients;
	std::string							_msgBuf;
	std::map<std::string, Channel *>	_channels;

public:
	IrcServer();
	IrcServer(int port, const std::string& password);
	~IrcServer();

	void	init();
	void	acceptClient();
	void	removeClient(int fd);
	void	handleSocketEvent(int fd);
	void	handleClientMessage(int fd);
	void	castMsg(int client_fd, const char* message); //프라이빗 메세지 등 다양한 모드로 메세지를 전송할 수 있기 때문에 castMsg라는 이름으로 변경
	void	handleError(ErrorCode code, int flag);
	void	run();

	void 	handleClientRequest(int client_fd);
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
