#include "Server.hpp"
#include "Client.hpp"

void IrcServer::cmdUser(std::stringstream &msg, int client_fd) {
	int cnt = 0;
	std::string tmp;
	std::string names[4];
	Client* client = getClient(client_fd);

	while (cnt < 4) {
		if (!(msg >> tmp)) {
			std::string needmoreparams = ERR_NEEDMOREPARAMS(client->getNickname(), "USER");
			char tmpMsg[needmoreparams.length() + 1];
			broadcastMessage(client_fd, tmpMsg);
		}
		names[cnt] = tmp;
		cnt++;
	}

	// 정상적으로 4개의 필드가 들어왔을 때 클라이언트 정보를 설정
    client->setUsername(names[0]);
    client->setHostname(names[1]);
    client->setServername(names[2]);
    client->setRealname(names[3]); 

	std::cout << "client->getUsername() : " << client->getUsername() << std::endl;
	std::cout << "client->getHostname() : " << client->getHostname() << std::endl;
	std::cout << "client->getServername() : " << client->getServername() << std::endl;
	std::cout << "client->getRealname() : " << client->getRealname() << std::endl;

	// 클라이언트에게 웰컴 메시지 전송
    std::string welcomeMsg = RPL_WELCOME(client->getUsername());
    broadcastMessage(client_fd, makeMsgFromServer(welcomeMsg));
}

void IrcServer::cmdNick(std::stringstream &msg, int client_fd) {
	std::string nickname;
	msg >> nickname;
	Client* client = getClient(client_fd);
	client->setNickname(nickname);
	std::cout << "client->getNickname() : " << client->getNickname() << std::endl;
}