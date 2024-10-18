#include "Server.hpp"
#include "Client.hpp"

void IrcServer::cmdUser(std::stringstream &msg, int client_fd) {
	int cnt = 0;
	std::string tmp;
	std::string names[4];
	Client* client = getClient(client_fd);

	while (cnt < 4) {
		if (!(msg >> tmp))
			castMsg(client_fd, makeMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "USER")).c_str());
		names[cnt] = tmp;
		cnt++;
	}

	// 정상적으로 4개의 필드가 들어왔을 때 클라이언트 정보를 설정
    client->setUsername(names[0]);
    client->setHostname(names[1]);
    client->setServername(names[2]);
    client->setRealname(names[3]); 

	// std::cout << "client->getUsername() : " << client->getUsername() << std::endl;
	// std::cout << "client->getHostname() : " << client->getHostname() << std::endl;
	// std::cout << "client->getServername() : " << client->getServername() << std::endl;
	// std::cout << "client->getRealname() : " << client->getRealname() << std::endl;

	// 클라이언트에게 웰컴 메시지 전송
    castMsg(client_fd, makeMsg(RPL_WELCOME(client->getUsername())).c_str());
}

void IrcServer::cmdNick(std::stringstream &msg, int client_fd) {
	Client* client = getClient(client_fd);
	std::string nickname;
	msg >> nickname;
	client->setNickname(nickname);
	std::cout << "client->getNickname() : " << client->getNickname() << std::endl;
}

void IrcServer::cmdPass(std::string msg, int client_fd) {
	Client* client = getClient(client_fd);
    std::string password;
    password = msg;
    std::cout << "password : " << password << std::endl;
    client->setPassword("1111");
}


void IrcServer::cmdCap(std::stringstream& msg, int client_fd) {
	std::cout << "cmdCAP() : " << msg.str() << std::endl;
    std::string subcommand;
    msg >> subcommand;

    if (subcommand == "LS") {
        // 클라이언트가 요청한 CAP 목록을 보내는 로직
        std::string response = ":server CAP * LS :multi-prefix";
        send(client_fd, response.c_str(), response.size(), 0);
    }
    // 추가적인 CAP 서브 커맨드 처리
}