#include "Cmd.hpp"

void Cmd::checkPassword(const std::string& str) {
	/* EMPTY check */
	if (str.empty()) {
		throw CmdException(server.makeMsg(PREFIX_SERVER(server.getName()), ERR_NEEDMOREPARAMS(client->getNickname(), "PASS")));
	}

	/* SIZE check */
	if (str.size() >= PASSWORD_MAX_LEN) {
		throw CmdException(server.makeMsg(PREFIX_SERVER(server.getName()), ERR_PASS_PASSWORD));
	}

	/* ALNUM check */
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
		if (!std::isalnum(static_cast<unsigned char>(*it))) {
			throw CmdException(server.makeMsg(PREFIX_SERVER(server.getName()), ERR_PASS_PASSWORD));
		}
	}

	if (this->cmdParams == server.getPassword()) {
		client->setPassStatus(true);
	} else {
		throw CmdException(server.makeMsg(PREFIX_SERVER(server.getName()), ERR_PASS_PASSWORD));
	}
}

void Cmd::cmdPass() {
	checkPassword(this->cmdParams);
	// client->setPassword(this->cmdParams);
}
