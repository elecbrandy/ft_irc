#include "Client.hpp"
#include "Server.hpp"

Client::Client() : _nickname(""), 
					_username(""), 
					_hostname(""), 
					_realname(""), 
					_password(""),
					_servername(""),
					_lastPongTime(time(NULL)) {}

Client:: ~Client() {}

/* setter */
void Client::setNickname(const std::string& str) {this->_nickname = str;}

void Client::setUsername(const std::string& str) {this->_username = str;}

void Client::setHostname(const std::string& str) {this->_hostname = str;}

void Client::setRealname(const std::string& str) {this->_realname = str;}

void Client::setPassword(const std::string str) {this->_password = str;}

void Client::setServername(const std::string& str) {this->_servername = str;}

void Client::setLastPongTime() {this->_lastPongTime = time(NULL);}

/* getter */
std::string	Client::getNickname() const {return this->_nickname;}

std::string	Client::getUsername() const {return this->_username;}

std::string	Client::getHostname() const {return this->_hostname;}

std::string	Client::getRealname() const {return this->_realname;}

std::string	Client::getPassword() const {return this->_password;}

std::string	Client::getServername() const {return this->_servername;}

time_t	Client::getLastPongTime() const {return this->_lastPongTime;}

/* other */
bool Client::isConnectionTimedOut(time_t timeout) {
	time_t now = time(NULL);
	if (now - this->_lastPongTime > timeout)
		return true;
	return false;
}

