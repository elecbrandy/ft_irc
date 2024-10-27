#include "Server.hpp"
#include "Client.hpp"

std::string IrcServer::makeMsg(std::string msg) {
	std::string prefix = ":localhost "; //prefix는 선택사항!
	std::string completeMsg = prefix + " " + msg + " " + CRLF;

	return completeMsg;
}