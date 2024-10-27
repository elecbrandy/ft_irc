#include "Cmd.hpp"

/*
	NICK

	1. 닉네임을 설정하거나 기존의 닉네임을 변경하는 명령어
	2. 중복 불가
	3. 닉네임은 영문 문자만 가능하며 최대 9자로 구성
	(영문 문자나 특수문자로 시작하며 최대 9자로 구성)

	Numeric Replies
	ERR_NONICKNAMEGIVEN: 닉네임이 제공되지 않았을 때 발생
	ERR_NICKNAMEINUSE: 사용하려는 닉네임이 이미 다른 사용자가 사용 중일 때 발생
	ERR_ERRONEUSNICKNAME: 잘못된 형식의 닉네임이 제공되었을 때 발생
	? - ERR_UNAVAILRESOURCE: 사용할 수 없는 자원이거나 닉네임이 제한되어 있을 때 발생
	? - ERR_RESTRICTED: 사용자가 닉네임 변경 권한이 제한되어 있을 때 발생
*/

void Cmd::checkNick(const std::string& str) {
	/* EMPTY check */
	if (str.empty() == 1) {
		if (server.getClient(client_fd)->getNickname().empty() == 1) {
			throw CmdException(ERR_NONICKNAMEGIVEN("guest"));
		} else {
			throw CmdException(ERR_NONICKNAMEGIVEN(""));
		}
		
	}

	/* SIZE check */
	if (str.size() > NICK_MAX_LEN) {
		throw CmdException(ERR_ERRONEUSNICKNAME);
	}
	
	/* ALNUM check */
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
		if (!std::isalpha(static_cast<unsigned char>(*it))) {
			throw CmdException(ERR_ERRONEUSNICKNAME);
		}
	}
}

void Cmd::cmdNick(std::string &cmdParams, int client_fd) {
	checkNick(cmdParams);
	Client* client = server.getClient(client_fd);
	client->setNickname(cmdParams);
	// std::cout << "client->getNickname() : " << client->getNickname() << std::endl;
}
