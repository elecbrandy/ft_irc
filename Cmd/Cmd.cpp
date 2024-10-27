#include "Cmd.hpp"

Cmd::Cmd(IrcServer &s, std::string &msg, int client_fd)
: server(s), msg(msg), client_fd(client_fd), client(server.getClient(client_fd)) {}

Cmd::~Cmd() {}

std::string Cmd::extractCmd() {
	size_t pos = 0;

	while (pos < msg.size() && !std::isspace(static_cast<unsigned char>(msg[pos]))) {
		++pos;
	}
	return msg.substr(0, pos);
}

std::string Cmd::extractCmdParams() {
	size_t startPos = this->cmd.size() + 1;			// 시작 위치
	size_t endPos = msg.find(CRLF, startPos);		// 끝 위치 (CRLF 위치)

	if (endPos != std::string::npos) {     			// CRLF 존재O -> 파라미터 추출
		return msg.substr(startPos, endPos - startPos);
	} else {
		return "";									// CRLF 존재X -> 빈 문자열 반환
	}
}

void Cmd::handleClientCmd() {
	try {
		this->cmd = extractCmd();
		this->cmdParams = extractCmdParams();

		if (this->cmd == "PASS") {
			cmdPass(cmdParams, client_fd);
		} else if (this->cmd == "NICK") {
			cmdNick(cmdParams, client_fd);
		} else if (this->cmd == "USER") {
			cmdUser(cmdParams, client_fd);
		} else if (this->cmd == "PING") {
			cmdPing(cmdParams, client_fd);
		} else if (this->cmd == "JOIN") {
			cmdJoin(cmdParams, client_fd);
		} else if (cmd == "PART") {
			// cmdMode(client_fd, clientMsg);
		} else if (cmd == "PRIVMSG") {
			// cmdPrivmsg(client_fd, clientMsg);
		} else if (cmd == "KICK") {
			// cmdKick(client_fd, clientMsg);
		} else if (cmd == "INVITE") {
			// cmdInvite(client_fd, clientMsg);
		} else if (cmd == "MODE") {
			// cmdPart(client_fd, clientMsg);
		} else if (cmd == "TOPIC") {
			// cmdTopic(client_fd, clientMsg);
		} else {
			// castMsg(client_fd, makeMsg(ERR_UNKNOWNCOMMAND(client->getNickname())).c_str());
		}
	}
	catch (const CmdException& e) {
		server.castMsg(client_fd, e.what());
	}
}

// void Cmd::cmdPing(std::string &cmdParams, int client_fd) {
//     Client* client = getClient(client_fd);
//     client->setLastPongTime(); //마지막 pong 시간 갱신을 하는 이유 : 1.고스트커넥션 방지 2.서버가 살아있는지 확인 3.서버의 리소스 관리를 위해
// 	std::string servername = client->getServername();
// 	if (cmdParams == servername)
// 		castMsg(client_fd, makeMsg("PONG" + ' ' + servername).c_str());
// }


/*
	PASS
	1. 닉네임을 설정하거나 기존의 닉네임을 변경하는 명령어
	2. 이미 사용 중인 닉네임인 경우 에러 응답 ERR_NICKCOLLISION
	3. 닉네임은 영문 문자나 특수문자로 시작하며 최대 9자로 구성됨.
*/

// void Cmd::cmdPass(std::string &cmdParams, int client_fd) {
// 	Client* client = getClient(client_fd);
//     std::string password = cmdParams;
//     client->setPassword(password);
//     // std::cout << "client->getPassword() : " << client->getPassword() << std::endl;
// }
