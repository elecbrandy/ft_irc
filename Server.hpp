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
#include "ErrorCode.hpp"

#define MAX_CLIENTS 42
#define BUFFER_SIZE 42

#define EXIT 1
#define UNEXIT 0

class IrcServer {
private:
	int							server_fd;
	int							port;
	std::string					password;
	std::vector<struct pollfd>	fds;
	std::map<int, Client*>		clients;

public:
	IrcServer();
	IrcServer(int port, const std::string& password);
	~IrcServer();

	void	init();
	void	acceptClient();
	void	removeClient(int fd);
	void	handleSocketEvent(int fd);
	void	handleClientMessage(int fd);
	void	broadcastMessage(int sender_fd, const char* message);
	void	handleError(ErrorCode code, int flag);
	void	run();
};

#endif
