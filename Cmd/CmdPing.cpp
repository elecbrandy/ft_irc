#include "Cmd.hpp"

/*
	PING
	/ping a --> PRIVMSG a :PING 1730057358 334518

	1분 간격
	Tue Nov 05 2024 10:09:46 USERINPUT: C[811AAAAAA] I PING irc.local
	Tue Nov 05 2024 10:09:46 USEROUTPUT: C[811AAAAAA] O :irc.local PONG irc.local :irc.local

	PONG irc.local
*/

void Cmd::cmdPong() {
	// 명령어를 보낸 클라이언트가 register 되지 않은 경우
    if (client->getRegisteredStatus() == false)
        throw Cmd::CmdException(server.makeMsg(PREFIX_SERVER(server.getName()), ERR_NOTREGISTERED(client->getNickname())));

	client->setlastActivityTime();
	server.castMsg(client_fd, server.makeMsg(std::string("PONG") + ' ' + getCmdParams()));
}
