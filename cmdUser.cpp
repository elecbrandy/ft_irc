#include "Server.hpp"
#include "Client.hpp"

void IrcServer::cmdUser(std::stringstream &msg, int client_fd) {
	std::cout << "here" << std::endl;
	std::string names[4];
	std::string tmp;
	int cnt = 0;
	std::cout << "here1" << std::endl;
	Client* client = getClient(client_fd); //클라이언트 fd가 저장이 안된 상태에서 가져와서 실행이 안된다!!!!!

	while (cnt < 4) {
		if (!(msg >> tmp)) {
			std::string needmoreparams = ERR_NEEDMOREPARAMS(client->getNickname(), "USER");
			char tmpMsg[needmoreparams.length() + 1];
			broadcastMessage(client_fd, tmpMsg);
		}
	std::cout << "here2" << std::endl;
		names[cnt] = tmp;
		cnt++;
	}
	std::cout << "here3" << std::endl;
	std::string welcomeMsg = RPL_WELCOME(client->getNickname());
	char tmpMsg[welcomeMsg.length() + 1];
	broadcastMessage(client_fd, tmpMsg);
}