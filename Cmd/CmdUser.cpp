#include "Cmd.hpp"

/*
	USER
	- 명령어 : USER
	- 매개변수 : <user> <mode> <unused> <realname>
	
	~?

	Numeric 응답 코드
	- ERR_NEEDMOREPARAMS: 필요한 매개변수가 부족한 경우 반환
	- ERR_ALREADYREGISTRED: 클라이언트가 이미 등록된 상태에서 USER 명령을 다시 보낼 때 반환
*/

void Cmd::cmdUser(std::string &cmdParams, int client_fd) {
	std::stringstream ss(cmdParams);
	std::string names[4];
	Client* client = server.getClient(client_fd);

	for (int i = 0; i < 4; i++) {
		if (cmdParams.empty())
			server.castMsg(client_fd, makeMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "USER")).c_str());
		ss >> names[i];
		if (i == 3 && !ss.eof()) {
			while (!ss.eof()) {
				std::string tmp;
				ss >> tmp;
				names[i] += " " + tmp;
			}
		}
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
	server.castMsg(client_fd, makeMsg(RPL_WELCOME(client->getUsername())).c_str());
}


