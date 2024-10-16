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

// 001: 서버에 연결됨
#define RPL_WELCOME(user) 						"001 " + user + " :Welcome to the motherfucking Internet Relay Network, " + user + "!"

// 401: 대상 닉네임을 가진 사용자가 존재하지 않음
#define ERR_NOSUCHNICK							"401 " + user + " "

// 402: 해당 서버를 찾을 수 없음
#define ERR_NOSUCHSERVER						"402 " + user + " "

// 403: 해당 채널이 존재하지 않음
#define ERR_NOSUCHCHANNEL 						"403 " + user + " "

// 404: 사용자가 채널에 메시지를 보낼 수 없음
#define ERR_CANNOTSENDTOCHAN 					"404 " + user + " "

// 405: 사용자가 가입할 수 있는 채널 수를 초과함
#define ERR_TOOMANYCHANNELS 					"405 " + user + " "

// 406: 해당 닉네임을 가진 사용자가 존재하지 않았음
#define ERR_WASNOSUCHNICK 						"406 " + user + " "

// 407: 대상이 너무 많아서 명령을 처리할 수 없음
#define ERR_TOOMANYTARGETS 						"407 " + user + " "

// 409: 출처가 없거나 누락됨 (PING/PONG)
#define ERR_NOORIGIN 							"409 " + user + " "

// 411: 명령에 메시지 대상이 없음
#define ERR_NORECIPIENT 						"411 " + user + " "

// 412: 보내야 할 메시지가 없음
#define ERR_NOTEXTTOSEND 						"412 " + user + " "

// 413: 주소가 최상위 도메인이 아님
// #define ERR_NOTOPLEVEL 							"413 " + user + " "

// 414: 와일드카드가 최상위 도메인에 사용됨
// #define ERR_WILDTOPLEVEL 						"414 " + user + " "

// 421: 알 수 없는 명령을 보냄
#define ERR_UNKNOWNCOMMAND 						"421 " + user + " "

// 422: 서버에 MOTD(메시지)가 설정되지 않음
#define ERR_NOMOTD 								"422 " + user + " "

// 423: 서버의 관리자 정보를 찾을 수 없음
#define ERR_NOADMININFO 						"423 " + user + " "

// 424: 파일 관련 에러 (파일 읽기/쓰기 실패 등)
#define ERR_FILEERROR 							"424 " + user + " "

// 431: 닉네임이 주어지지 않음
#define ERR_NONICKNAMEGIVEN 					"431 " + user + " "

// 432: 잘못된 형식의 닉네임 사용
#define ERR_ERRONEUSNICKNAME 					"432 " + user + " "

// 433: 닉네임이 이미 사용 중임
#define ERR_NICKNAMEINUSE 						"433 " + user + " "

// 436: 동일한 닉네임으로 다른 서버에서 충돌 발생
#define ERR_NICKCOLLISION 						"436 " + user + " "

// 441: 사용자가 해당 채널에 있지 않음
#define ERR_USERNOTINCHANNEL 					"441 " + user + " "

// 442: 사용자가 해당 채널에 속해 있지 않음
#define ERR_NOTONCHANNEL 						"442 " + user + " "

// 443: 사용자가 이미 해당 채널에 속해 있음
#define ERR_USERONCHANNEL 						"443 " + user + " "

// 444: 사용자가 로그인하지 않음
#define ERR_NOLOGIN 							"444 " + user + " "

// 445: 서버에서 소환 기능이 비활성화됨
#define ERR_SUMMONDISABLED 						"445 " + user + " "

// 446: 서버에서 USERS 명령이 비활성화됨
#define ERR_USERSDISABLED 						"446 " + user + " "

// 451: 클라이언트가 등록되지 않음 (USER/NICK 명령을 사용하지 않음)
#define ERR_NOTREGISTERED 						"451 " + user + " "

// 461: 명령에 필요한 파라미터가 부족함
#define ERR_NEEDMOREPARAMS(user, command) 		"461 " + user + " " + command + " :Not enough parameters"

// 462: 클라이언트가 이미 등록된 상태에서 다시 등록 시도
#define ERR_ALREADYREGISTERED 					"462 " + user + " "

// 463: 호스트에 대한 권한이 부족함
#define ERR_NOPERMFORHOST 						"463 " + user + " "

// 464: 비밀번호가 틀림
#define ERR_PASSWDMISMATCH 						"464 " + user + " "

// 465: 클라이언트가 서버에서 금지됨
#define ERR_YOUREBANNEDCREEP 					"465 " + user + " "

// 467: 채널의 키가 이미 설정되어 있음
#define ERR_KEYSET 								"467 " + user + " "

// 471: 채널이 가득 차서 더 이상 사용자가 들어올 수 없음
#define ERR_CHANNELISFULL 						"471 " + user + " "

// 472: 알 수 없는 모드를 사용함
#define ERR_UNKNOWNMODE 						"472 " + user + " "

// 473: 초대받은 사용자만 들어갈 수 있는 채널임
#define ERR_INVITEONLYCHAN 						"473 " + user + " "

// 474: 사용자가 해당 채널에서 금지됨
#define ERR_BANNEDFROMCHAN 						"474 " + user + " "

// 475: 잘못된 채널 키를 입력함
#define ERR_BADCHANNELKEY 						"475 " + user + " "

// 476: 잘못된 채널 이름 형식을 사용함
#define ERR_BADCHANMASK 						"476 " + user + " "

// 477: 채널이 모드를 지원하지 않음
#define ERR_NOCHANMODES 						"477 " + user + " "

// 478: 채널의 금지 목록이 가득 참
#define ERR_BANLISTFULL 						"478 " + user + " "

// 481: 사용자가 이 명령을 수행할 권한이 없음
#define ERR_NOPRIVILEGES 						"481 " + user + " "

// 482: 채널 운영자 권한이 필요함
#define ERR_CHANOPRIVSNEEDED 					"482 " + user + " "

// 483: 서버를 종료할 수 없음
#define ERR_CANTKILLSERVER 						"483 " + user + " "

// 491: 해당 호스트에서 운영자 권한을 사용할 수 없음
#define ERR_NOOPERHOST 							"491 " + user + " "

// 501: 알 수 없는 사용자 모드 플래그 사용
#define ERR_UMODEUNKNOWNFLAG 					"501 " + user + " "

// 502: 사용자가 자신이 아닌 다른 사람의 모드를 변경하려 함
#define ERR_USERSDONTMATCH 						"502 " + user + " "


class IrcServer {
private:
	int							server_fd;
	int							port;
	std::string					password;
	std::vector<struct pollfd>	fds;
	std::map<int, Client*>		clients;
	std::stringstream			_msgBuf;

public:
	IrcServer();
	IrcServer(int port, const std::string& password);
	~IrcServer();

	void	init();
	void	acceptClient();
	void	removeClient(int fd);
	void	handleSocketEvent(int fd);
	void	handleClientMessage(int fd);
	void	broadcastMessage(int client_fd, char* message);
	void	handleError(ErrorCode code, int flag);
	void	run();

	void 	handleClientRequest(int client_fd);
	std::string extractCmd();
	void 	handleClientCommand(int client_fd);
	Client* getClient(int client_fd);
	void cmdUser(std::stringstream &msg, int client_fd);
};

#endif
