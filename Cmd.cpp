#include "Cmd.hpp"

Cmd::Cmd() {}

Cmd::Cmd(std::string &msg, int client_fd) : msg(msg), client_fd(client_fd) {}

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
	this->cmd = extractCmd();

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
