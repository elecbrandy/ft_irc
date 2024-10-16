#include "server.hpp"
#include "client.hpp"

const char* IrcServer::makeMsgFromServer(std::string msg) {
	std::string prefix = ":localhost ";
	std::string completeMsg = prefix + " " + msg + " " + CRLF;

	return completeMsg.c_str();
}