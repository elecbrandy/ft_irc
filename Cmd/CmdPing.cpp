#include "Cmd.hpp"

void Cmd::cmdPong() {
	// 명령어를 보낸 클라이언트가 register 되지 않은 경우
    if (client->getRegisteredStatus() == false)
        throw Cmd::CmdException(server.makeMsg(PREFIX_SERVER(server.getName()), ERR_NOTREGISTERED(client->getNickname())));

	client->setlastActivityTime();
	server.castMsg(client_fd, server.makeMsg(std::string("PONG") + ' ' + getCmdParams()));
}
