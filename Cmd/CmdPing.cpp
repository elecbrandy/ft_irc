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
	client->setlastActivityTime();
	server.castMsg(client_fd, server.makeMsg(PREFIX_SERVER(server.getName()), "PONG " + getCmdParams()));
}
