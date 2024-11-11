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
#include <signal.h>
#include <fcntl.h>
#include <sstream>
#include <errno.h>
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

#define PATH_GOAT "./etc/goat.txt"
#define PATH_MOTD "./etc/motd.txt"
#define SERVER_NAME "ircserv"

#define CRLF "\r\n"
#define PREFIX_SERVER(servername) 					":" + servername
#define PREFIX_USER(nickname, username, hostname) 	":" + nickname + "!" + username + "@" + hostname

#define TIME_OUT 60
#define TIME_CHECK_INTERVAL 180

class IrcServer {
private:
	const std::string					_servername;
	int									_fd;
	int									port;
	std::string							password;
	time_t								_startTime;
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
	void	handleSocketRead(int fd);
	void	handleClientMessage(int fd);
	void	broadcastMsg(const std::string& message, Channel* channel, int senderFd);
	void	castMsg(int client_fd, const std::string message);
	void	run();
	
	/* setter & getter */
	void setChannels(const std::string& channelName, const std::string& key, const char& mode);

	
	Client* getClient(const std::string& nickname);
	Client* getClient(int client_fd);
	const std::string getName() const;
	time_t getStartTime() const;

	const std::map<std::string, Channel*>& getChannels() const;
	void removeChannel(const std::string channelName);
	
	const std::map<std::string, Client*>& getNickNameClientMap() const;
	void addClientByNickname(const std::string& nickname, Client* client);
	std::string makeMsg(const std::string& prefix, const std::string& msg);

	void checkConnections();
	std::string	getPassword();
	void printGoat();
	std::string formatDateToString(time_t time);

	void modifyPollEvent(int client_fd, short events);
	void handleSocketWrite(int client_fd);
	void removeClinetFromServer(Client* client);

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
