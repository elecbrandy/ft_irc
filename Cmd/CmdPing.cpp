#include "Cmd.hpp"

/*
	PING
	/ping a --> PRIVMSG a :PING 1730057358 334518
*/

void Cmd::cmdPing() {
	client->setLastPongTime(); //마지막 pong 시간 갱신을 하는 이유 : 1.고스트커넥션 방지 2.서버가 살아있는지 확인 3.서버의 리소스 관리를 위해
	std::string servername = client->getServername();
	if (cmdParams == servername){
		server.castMsg(client_fd, server.makeMsg(std::string("PONG") + ' ' + servername).c_str());
	std::cout << "pong sent!" << std::endl;
	}
}

// void Cmd::cmdPing() {
//     client->setLastPongTime(); // 마지막 pong 시간 갱신을 하는 이유 : 1.고스트커넥션 방지 2.서버가 살아있는지 확인 3.서버의 리소스 관리를 위해
//     std::string pongMsg = "PONG " + this->cmdParams + "\r\n";
//     std::string serverName = this->client->getServername();
//     std::cout << C_ERR << pongMsg << C_RESET << std::endl;
//     send(client_fd, pongMsg.c_str(), pongMsg.size(), 0);
// }