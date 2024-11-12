#include "Cmd.hpp"

void Cmd::cmdBotMe() {
	server.castMsg(this->client_fd, server.makeMsg(PREFIX_SERVER, "[user info]"));
	server.castMsg(this->client_fd, server.makeMsg(PREFIX_SERVER, "nickname: " + client->getNickname()));
	server.castMsg(this->client_fd, server.makeMsg(PREFIX_SERVER, "username: " + client->getUsername()));
	server.castMsg(this->client_fd, server.makeMsg(PREFIX_SERVER, "hostname: " + client->getHostname()));
	server.castMsg(this->client_fd, server.makeMsg(PREFIX_SERVER, "lastActivityTime: " + server.formatDateToString(client->getLastActivityTime())));
}

void Cmd::cmdBotTime() {
	server.castMsg(this->client_fd, server.makeMsg(PREFIX_SERVER, "[time]"));
	server.castMsg(this->client_fd, server.makeMsg(PREFIX_SERVER, server.formatDateToString(time(NULL))));
}
