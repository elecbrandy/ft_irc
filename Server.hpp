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

#define MAX_CLIENTS 42
#define BUFFER_SIZE 42

#define EXIT 1
#define UNEXIT 0

enum ServerError {
    ERR_SOCKET_CREATION = 1,		// 소켓 생성 에러
    ERR_SOCKET_OPTIONS,				// 소켓 옵션 설정 에러
    ERR_SOCKET_BIND,				// 소켓 바인딩 에러
    ERR_SOCKET_LISTEN,				// 소켓 리슨 에러
    ERR_SET_NONBLOCKING,			// 논블로킹 모드 설정 에러
    ERR_ACCEPT_CLIENT,				// 클라이언트 수락 에러
    ERR_CLIENT_NONBLOCKING,			// 클라이언트 소켓 논블로킹 설정 에러
    ERR_DATA_RECEIVE,				// 데이터 수신 에러
    ERR_MESSAGE_SEND,				// 메시지 전송 에러
    ERR_POLL,						// poll() 에러
	ERR_ETC							// 이상한거 일단 여기에 다 넣기
};

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
	void	handleError(ServerError code, int flag);
	void	run();
};

#endif
