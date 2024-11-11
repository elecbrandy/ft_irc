#include "Cmd.hpp"

void Cmd::cmdBotMe() {
	server.castMsg(this->client_fd, "[user info]");
	server.castMsg(this->client_fd, "nickname: " + client->getNickname());
	server.castMsg(this->client_fd, "username: " + client->getUsername());
	server.castMsg(this->client_fd, "hostname: " + client->getHostname());
}

void Cmd::cmdBotTime() {
	server.castMsg(this->client_fd, "[time]");
	server.castMsg(this->client_fd, server.formatDateToString(time(NULL)));
}
