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

void Cmd::authorizeClient() {
	if (this->cmd == "CAP") {
		cmdCap();
	} else if (this->cmd == "PASS") {
		cmdPass();
		client->setPassStatus(true);
	} else if (this->cmd == "NICK") {
		cmdNick();
		client->setNickStatus(true);
	} else if (this->cmd == "USER") {
		cmdUser();
		client->setUserStatus(true);
	}
	if (client->getPassStatus() && client->getNickStatus() && client->getUserStatus()) {
		client->setRegisteredStatus(true);

		/* RPL */
		server.castMsg(client_fd, server.makeMsg(RPL_WELCOME(client->getNickname(), client->getServername())));
		server.castMsg(client_fd, server.makeMsg(RPL_YOURHOST(client->getNickname(), client->getServername())).c_str());
		server.castMsg(client_fd, server.makeMsg(RPL_CREATED(client->getNickname(), server.formatDateToString(server.getStartTime()))).c_str());
		server.castMsg(client_fd, server.makeMsg(RPL_MYINFO(client->getNickname())).c_str());

		/* MOTD */
		server.castMsg(client_fd, server.makeMsg(RPL_MOTDSTART(client->getNickname())).c_str());
		server.castMsg(client_fd, server.makeMsg(RPL_MOTD(client->getNickname())).c_str());
		server.castMsg(client_fd, server.makeMsg(RPL_ENDOFMOTD(client->getNickname())).c_str());

	}
}

bool Cmd::handleClientCmd() {
	try {
		// ScopedTimer("Cmd");
		this->cmd = extractCmd();
		this->cmdParams = extractCmdParams();

		if (!client->getRegisteredStatus()) {
			authorizeClient();
		} else {
			if (this->cmd == "NICK") {
				cmdNick();
			} else if (this->cmd == "USER") {
				cmdUser();
			} else if (this->cmd == "PING") {
				cmdPong();
			} else if (this->cmd == "JOIN") {
				cmdJoin();
			} else if (cmd == "PART") {
				cmdPart();
			} else if (cmd == "PRIVMSG") {
				cmdPrivmsg();
			} else if (cmd == "KICK") {
				cmdKick();
			} else if (cmd == "INVITE") {
				cmdInvite();
			} else if (cmd == "MODE") {
				cmdMode();
			} else if (cmd == "TOPIC") {
				cmdTopic();
			} else if (cmd == "QUIT") {
				cmdQuit();
			} else {
				return ;
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
