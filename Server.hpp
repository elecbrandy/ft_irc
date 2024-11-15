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

#define PATH_GOAT "./conf/goat.txt"
#define PATH_MOTD "./conf/motd.txt"
#define SERVER_NAME "ircserv"

#define CRLF "\r\n"
#define PREFIX_SERVER ":ircserv"
#define PREFIX_USER(nickname, username, hostname) 	":" + nickname + "!" + username + "@" + hostname

#define TIME_OUT 20
#define WAIT_FOR_PING_MAX 60
#define WAIT_FOR_PONG_MAX 20

#define LOG_ERR -1
#define LOG_SERVER 0
#define LOG_INPUT 1
#define LOG_OUTPUT 2

class IrcServer {
private:
	int									_fd;
	int									_port;
	std::string							_password;
	const std::string					_servername;
	time_t								_startTime;

	std::vector<struct pollfd>			fds;
	std::map<int, Client*>				_clients;
	std::map<std::string, Channel *>	_channels;
	std::map<std::string, Client*>		nickNameClientMap;
	std::vector<int>					_fdsToRemove;

public:
	IrcServer();
	IrcServer(const char *port, const char *password);
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

	std::map<std::string, Channel*>& getChannels();
	void removeChannel(const std::string channelName);
	
	const std::map<std::string, Client*>& getNickNameClientMap() const;
	void addClientByNickname(const std::string& nickname, Client* client);
	std::string makeMsg(const std::string& prefix, const std::string& msg);

	void sendPingToClient(); // 일정 시간동안 PING을 보내지 않은 클라이언트에게 PING 전송
	void checkPongFromClient(); // PING을 보낸 클라이언트로부터 PONG을 받지 못한 경우 처리
	std::string	getPassword();
	void printGoat();
	std::string formatDateToString(time_t time);

	void removeClientFromServer(Client* client);

	void updateClients(Client* client);
	void updateNickNameClientMap(const std::string& oldNick, const std::string& newNick, Client* client);

	void serverLog(int fd, int log_type, std::string log_color, std::string msg);
	std::string intToString(int num);
	static void signalHandler(int signal);
	void sendPing();
	void checkPong();

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
