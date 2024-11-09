#include "Server.hpp"
#include "Client.hpp"

std::string IrcServer::makeMsg(const std::string& prefix, const std::string& msg) {
	//prefix는 선택사항!
	// std::string completeMsg = ":" + this->_name + " " + msg + " " + CRLF;
	
	std::string completeMsg = prefix + " "  + msg + CRLF;

	return completeMsg;
}