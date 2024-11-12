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

bool Cmd::handleClientCmd() {
	try {
		this->cmd = extractCmd();
		this->cmdParams = extractCmdParams();

		if (cmd == "NICK")
			cmdNick();
		else if (cmd == "PASS")
			cmdPass();
		else if (cmd == "USER")
			cmdUser();
		else if (cmd == "PING")
			cmdPong();
		else if (cmd == "JOIN")
			cmdJoin();
		else if (cmd == "PART")
			cmdPart();
		else if (cmd == "PRIVMSG")
			cmdPrivmsg();
		else if (cmd == "KICK")
			cmdKick();
		else if (cmd == "INVITE")
			cmdInvite();
		else if (cmd == "MODE")
			cmdMode();
		else if (cmd == "TOPIC")
			cmdTopic();
		else if (cmd == "QUIT")
			cmdQuit();
		else
			return ;
	} catch (const CmdException& e) {
		if (!this->client->getPassStatus() || \
			!this->client->getRegisteredStatus()) {	
			server.castMsg(client_fd, e.what());
			return false;
		}
		server.castMsg(client_fd, e.what());
	}
	return true;
}

const char* Cmd::CmdException::what() const throw() {return msg.c_str();}
std::string Cmd::getCmdParams() const {return this->cmdParams;}

void Cmd::printWelcome() {
	/* RPL */
	server.castMsg(client_fd, server.makeMsg(PREFIX_SERVER(server.getName()), RPL_WELCOME(client->getNickname(), client->getServername())));
	server.castMsg(client_fd, server.makeMsg(PREFIX_SERVER(server.getName()), RPL_YOURHOST(client->getNickname(), client->getServername())));
	server.castMsg(client_fd, server.makeMsg(PREFIX_SERVER(server.getName()), RPL_CREATED(client->getNickname(), server.formatDateToString(server.getStartTime()))));
	server.castMsg(client_fd, server.makeMsg(PREFIX_SERVER(server.getName()), RPL_MYINFO(client->getNickname())));
	
	/* MOTD */
	server.castMsg(client_fd, server.makeMsg(PREFIX_SERVER(server.getName()), RPL_MOTDSTART(client->getNickname())));
	std::ifstream goatFile(PATH_MOTD);
	if (goatFile.is_open()) {

		std::string line;
		while (getline(goatFile, line)) {
			server.castMsg(client_fd, server.makeMsg(PREFIX_SERVER(server.getName()), RPL_MOTD(client->getNickname(), line)));
		}
		goatFile.close();
	}
	server.castMsg(client_fd, server.makeMsg(PREFIX_SERVER(server.getName()), RPL_ENDOFMOTD(client->getNickname())));
}
