#include "Cmd.hpp"

void Cmd::cmdPong() {
	server.castMsg(client_fd, server.makeMsg(PREFIX_SERVER(server.getName()), "PONG " + getCmdParams()));
}
