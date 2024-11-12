#include "Cmd.hpp"

void Cmd::cmdBotMe() {
	server.castMsg(this->client_fd, server.makeMsg(PREFIX_SERVER(server.getName()), "[user info]"));
	server.castMsg(this->client_fd, server.makeMsg(PREFIX_SERVER(server.getName()), "nickname: " + client->getNickname()));
	server.castMsg(this->client_fd, server.makeMsg(PREFIX_SERVER(server.getName()), "username: " + client->getUsername()));
	server.castMsg(this->client_fd, server.makeMsg(PREFIX_SERVER(server.getName()), "hostname: " + client->getHostname()));
	server.castMsg(this->client_fd, server.makeMsg(PREFIX_SERVER(server.getName()), "lastActivityTime: " + server.formatDateToString(client->getLastActivityTime())));
}

void Cmd::cmdBotTime() {
	server.castMsg(this->client_fd, server.makeMsg(PREFIX_SERVER(server.getName()), "[time]"));
	server.castMsg(this->client_fd, server.makeMsg(PREFIX_SERVER(server.getName()), server.formatDateToString(time(NULL))));
}
