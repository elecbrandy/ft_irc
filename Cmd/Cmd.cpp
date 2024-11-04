#include "Cmd.hpp"

Cmd::Cmd(IrcServer &s, std::string &msg, int client_fd)
: server(s), client_fd(client_fd), client(server.getClient(client_fd)), msg(msg) {}

Cmd::~Cmd() {}

std::string Cmd::extractCmd() {
	size_t pos = 0;

	while (pos < msg.size() && !std::isspace(static_cast<unsigned char>(msg[pos]))) {
		++pos;
	}
	return msg.substr(0, pos);
}

std::string Cmd::extractCmdParams() {
	size_t startPos = this->cmd.size();

	// 명령어와 파라미터 사이에 공백이 있을 경우를 처리
	if (msg.size() > startPos && std::isspace(static_cast<unsigned char>(msg[startPos]))) {
		++startPos;
	}

	if (startPos < msg.size()) {
		return msg.substr(startPos);
	} else {
		return ""; // 파라미터가 없는 경우 빈 문자열 반환
	}
}

void	Cmd::authorizeClient() {
	if (this->cmd == "CAP") {
		cmdCap();
	} else if (this->cmd == "PASS") {
		cmdPass();
		client->setPassStatus(true);
	} else if (this->cmd == "NICK") {
		cmdNick();
		client->setNickStatus(true);
	} else if (this->cmd == "USER") {
		cmdUser(cmdParams, client_fd);
		client->setUserStatus(true);
	}
	if (client->getPassStatus() &&	\
		client->getNickStatus() &&	\
		client->getUserStatus()) {
		client->setRegisteredStatus(true);
		server.castMsg(client_fd, server.makeMsg(RPL_WELCOME(client->getUsername())).c_str());
	}
}

bool Cmd::handleClientCmd() {
	try {
		this->cmd = extractCmd();
		this->cmdParams = extractCmdParams();
		// std::cout << YELLOW << "cmd: " << this->cmd << C_RESET << std::endl;
		// std::cout << YELLOW << "params: " << this->cmdParams << C_RESET << "\n" << std::endl;
		if (!client->getRegisteredStatus()) {
			authorizeClient();
		} else {
			if (this->cmd == "NICK") {
				cmdNick();
			} else if (this->cmd == "USER") {
				cmdUser(cmdParams, client_fd);
			} else if (this->cmd == "PING") {
				cmdPing();
			} else if (this->cmd == "JOIN") {
				// cmdJoin(cmdParams, client_fd);
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
				server.castMsg(client_fd, server.makeMsg(ERR_UNKNOWNCOMMAND(client->getNickname())).c_str());
			}
		}
	}
	catch (const CmdException& e) {
		if (!this->client->getPassStatus() || \
			!this->client->getRegisteredStatus()) {
			server.castMsg(client_fd, e.what());
			server.removeClient(client_fd);
			return false;
		}
		server.castMsg(client_fd, e.what());
	}
	return true;
}

const char* Cmd::CmdException::what() const throw() {
	return msg.c_str();
}

std::string Cmd::getCmdParams() const {return this->cmdParams;}