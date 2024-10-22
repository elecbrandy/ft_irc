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

	std::cout << "client->getUsername() : " << client->getUsername() << std::endl;
	std::cout << "client->getHostname() : " << client->getHostname() << std::endl;
	std::cout << "client->getServername() : " << client->getServername() << std::endl;
	std::cout << "client->getRealname() : " << client->getRealname() << std::endl;

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

void IrcServer::cmdPass(std::stringstream &msg, int client_fd) {
	Client* client = getClient(client_fd);
    std::string password;
    msg >> password;
    client->setPassword(password);
    std::cout << "client->getPassword() : " << client->getPassword() << std::endl;
}

void IrcServer::cmdPong(std::stringstream &msg, int client_fd) {
    Client* client = getClient(client_fd);
    client->setLastPongTime(); //마지막 pong 시간 갱신을 하는 이유 : 1.고스트커넥션 방지 2.서버가 살아있는지 확인 3.서버의 리소스 관리를 위해
    std::string server;
	msg >> server;
	castMsg(client_fd, makeMsg("PING" + server).c_str());
}

std::vector<std::string> IrcServer::joinSplit(std::string &msg) {
	std::vector<std::string> res;
	std::string tmp;

	for (size_t i = 0; i < msg.size(); i++) {
		if (msg[i] == ',') {
			if (!tmp.empty()) {
				res.push_back(tmp);
				tmp.clear();
			}
		} else if (msg[i] == ' ') {
			if (!tmp.empty()) {
				res.push_back(tmp);
				tmp.clear();
			}
		} else
			tmp += msg[i];
	}
	if (!tmp.empty())
		res.push_back(tmp);
		
	return res;
}

void IrcServer::cmdJoin(std::stringstream &msg, int client_fd) {
	//Client* client = getClient(client_fd);
	(void)client_fd;
	std::string channelName;
	
	std::string msgStr = msg.str();
	std::vector<std::string> joinTokens = joinSplit(msgStr);
	std::vector<std::string> channels;
	std::vector<std::string> keys;
	
	for (size_t i = 0; i < joinTokens.size(); i++) {
		if (joinTokens[i] == " ") {
			for (size_t j = i + 1; j < joinTokens.size(); j++)
				keys.push_back(joinTokens[j]);
		}
		else
			channels.push_back(joinTokens[i]);
	}

	for (size_t i = 0; i < channels.size(); i++) {
		std::cout << "channelName [" << i << "] : " << channels[i] << std::endl;
	}
	for (size_t i = 0; i < keys.size(); i++) {
		std::cout << "keys [" << i << "] : " << keys[i] << std::endl;
	}
}