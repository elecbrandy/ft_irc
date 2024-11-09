#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <fstream>
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
#include "ServerCode.hpp"
#include "ReplyCode.hpp"
#include "Cmd/Cmd.hpp"

#define MAX_CLIENTS 42
#define BUFFER_SIZE 512

#define PORT_MAX 65535
#define PORT_MIN 0
#define PORT_MAX_LEN 5
#define PASSWORD_MAX_LEN 10

#define PATH_GOAT "./goat.txt"

#define PING_INTERVAL 120

#define CRLF "\r\n"
#define SERVER_PREFIX(servername)					":" + servername
#define USER_PREFIX(nickname, username, hostname)	":" + nickname + "!" + username + "@" + hostname


class IrcServer {
private:
	int									server_fd;
	int									port;
	std::string							password;
	std::vector<struct pollfd>			fds;
	std::map<int, Client*>				_clients;
	std::map<std::string, Channel *>	_channels;
	std::map<std::string, Client*>		nickNameClientMap;

public:
	IrcServer();
	IrcServer(int port, const std::string& password);
	~IrcServer();

	void	init();
	void	acceptClient();
	void	removeClient(int fd);
	void	handleSocketEvent(int fd);
	void	handleClientMessage(int fd);
	void	broadcastMsg(const std::string& message, Channel* channel);
	void	castMsg(int client_fd, const std::string message); //프라이빗 메세지 등 다양한 모드로 메세지를 전송할 수 있기 때문에 castMsg라는 이름으로 변경
	void	run();

	// void 	handleClientRequest(int client_fd);
	// std::string extractCmd();
	// std::string extractCmdParams(size_t cmdSize);
	// void 	handleClientCmd(int client_fd);
	
	/* setter & getter */
	void setChannels(const std::string& channelName, const std::string& key, const char& mode);

	Client* getClient(const std::string& nickname);
	Client* getClient(int client_fd);

	const std::map<std::string, Channel*>& getChannels() const;
	
	const std::map<std::string, Client*>& getNickNameClientMap() const;
	void addClientByNickname(const std::string& nickname, Client* client);
	std::string makeMsg(const std::string& prefix, const std::string& msg);

	void checkConnections();
	std::string	getPassword();
	void printGoat();

	/* exception */
	class ServerException : public std::exception {
	private:
		std::string	msg;
	public:
		ServerException(std::string str) : msg(str) {};
		virtual ~ServerException() throw() {};
		virtual const char*	 what() const throw();
	};
};

#endif
