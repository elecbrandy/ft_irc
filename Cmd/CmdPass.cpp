#include "Cmd.hpp"

/*
	PASS
	1. 닉네임을 설정하거나 기존의 닉네임을 변경하는 명령어
	2. 이미 사용 중인 닉네임인 경우 에러 응답 ERR_NICKCOLLISION
	3. 닉네임은 영문 문자나 특수문자로 시작하며 최대 9자로 구성

	Numeric 응답 코드
	ERR_NEEDMOREPARAMS: 비밀번호가 제공되지 않았을 때 발생
	ERR_ALREADYREGISTRED: 이미 서버에 등록된 상태에서 PASS 명령어를 다시 보낼 때 발생
	ERR_PASS_PASSWORD: 첫 PASS 시도에서 비번이 틀렸을 경우 응답코드는 rfc 상 딱히 없음.
	그래서 그냥 이걸로 해보면 어떨까?라는 생각
*/

void Cmd::checkPassword(const std::string& str) {
	/* EMPTY check */
	if (str.empty()) {
		throw CmdException(ERR_NEEDMOREPARAMS(client->getNickname(), "PASS"));
	}

	/* SIZE check */
	if (str.size() >= PASSWORD_MAX_LEN) {
		throw CmdException(ERR_PASS_PASSWORD);
	}

	/* ALNUM check */
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
		if (!std::isalnum(static_cast<unsigned char>(*it))) {
			throw CmdException(ERR_PASS_PASSWORD);
		}
	}

	if (this->cmdParams == server.getPassword()) {
		client->setPassStatus(true);
	} else {
		throw CmdException(ERR_PASS_PASSWORD);
	}
}

void Cmd::cmdPass() {
	checkPassword(this->cmdParams);
	client->setPassword(this->cmdParams);
}
