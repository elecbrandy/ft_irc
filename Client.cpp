#include "Client.hpp"
#include "Server.hpp"

Client::Client():	_nickname(""), 
					_username(""), 
					_hostname(""), 
					_realname(""), 
					_password(""),
					_servername(""),
					_lastPongTime(time(NULL)) {
					_registerStatus.pass = false;
					_registerStatus.nick = false;
					_registerStatus.user = false;
					_registerStatus.registered = false;
}

Client:: ~Client() {}

/* setter */
void Client::setNickname(const std::string& str) {this->_nickname = str;}

void Client::setUsername(const std::string& str) {this->_username = str;}

void Client::setHostname(const std::string& str) {this->_hostname = str;}

void Client::setRealname(const std::string& str) {
	size_t pos = str.find(":");
	if (pos != std::string::npos)
		this->_realname = str.substr(pos + 1);
}

void Client::setPassword(const std::string str) {this->_password = str;}

void Client::setServername(const std::string& str) {this->_servername = str;}

void Client::setLastPongTime() {this->_lastPongTime = time(NULL);}

void Client::setPassStatus(bool status) {this->_registerStatus.pass = status;}

void Client::setNickStatus(bool status) {this->_registerStatus.nick = status;}

void Client::setUserStatus(bool status) {this->_registerStatus.user = status;}

void Client::setRegisteredStatus(bool status) {this->_registerStatus.registered = status;}


/* getter */
int	Client::getFd() const {return this->_fd;}

std::string	Client::getNickname() const {return this->_nickname;}

std::string	Client::getUsername() const {return this->_username;}

std::string	Client::getHostname() const {return this->_hostname;}

std::string	Client::getRealname() const {return this->_realname;}

std::string	Client::getPassword() const {return this->_password;}

std::string	Client::getServername() const {return this->_servername;}

time_t	Client::getLastPongTime() const {return this->_lastPongTime;}

bool Client::getPassStatus() const {return this->_registerStatus.pass;}

bool Client::getNickStatus() const {return this->_registerStatus.nick;}

bool Client::getUserStatus() const {return this->_registerStatus.user;}

bool Client::getRegisteredStatus() const {return this->_registerStatus.registered;}

/* other */
void Client::appendToBuffer(const std::string& str) {
	_msgBuf += str;
}

bool Client::extractMessage(std::string& message) {
	size_t pos = _msgBuf.find("\r\n");
	if (pos != std::string::npos) {
		message = _msgBuf.substr(0, pos);
		_msgBuf.erase(0, pos + 2);
		return true;
	}
	return false;
}

bool Client::isConnectionTimedOut(time_t timeout) {
	time_t now = time(NULL);
	if (now - this->_lastPongTime > timeout)
		return true;
	return false;
}
