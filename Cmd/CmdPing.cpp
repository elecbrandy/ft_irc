#include "Cmd.hpp"

/*
    PING
*/

void Cmd::cmdPing(std::string &cmdParams, int client_fd) {
    Client* client = server.getClient(client_fd);
    client->setLastPongTime(); //마지막 pong 시간 갱신을 하는 이유 : 1.고스트커넥션 방지 2.서버가 살아있는지 확인 3.서버의 리소스 관리를 위해
	std::string servername = client->getServername();
	if (cmdParams == servername)
		server.castMsg(client_fd, makeMsg("PONG" + ' ' + servername).c_str());
}