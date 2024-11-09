#include "Server.hpp"
#include "Client.hpp"

std::string IrcServer::makeMsg(cosnt std::string& prefix, const std::string& msg) {
	//prefix는 선택사항!
	// std::string completeMsg = ":" + this->_name + " " + msg + " " + CRLF;
	
	std::string completeMsg = msg + " " + CRLF;

	return completeMsg;
}