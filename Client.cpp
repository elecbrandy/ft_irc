#include "Client.hpp"
#include "Server.hpp"

Client::Client(in_addr addr)
:	_hostname(inet_ntoa(addr)),
	_lastActivityTime(time(NULL)) {
	_registerStatus.pass = false;
	_registerStatus.nick = false;
	_registerStatus.user = false;
	_registerStatus.registered = false;
}

Client:: ~Client() {
	close(this->_fd);
}

/* setter */

void Client::setFd(int fd) {this->_fd = fd;}

void Client::setNickname(const std::string& str) {
	size_t spacePos = str.find(' ');
    if (spacePos != std::string::npos) {
        this->_nickname = str.substr(0, spacePos);
    } else {
        this->_nickname = str;
    }
}

void Client::setUsername(const std::string& str) {this->_username = str;}

void Client::setHostname(const std::string& str) {this->_hostname = str;}

void Client::setRealname(const std::string& str) {this->_realname = str;}

void Client::setServername(const std::string& str) {this->_servername = str;}

void Client::setlastActivityTime() {this->_lastActivityTime = time(NULL);}

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

std::string	Client::getServername() const {return this->_servername;}

time_t	Client::getLastActivityTime() const {return this->_lastActivityTime;}

bool Client::getPassStatus() const {return this->_registerStatus.pass;}

bool Client::getNickStatus() const {return this->_registerStatus.nick;}

bool Client::getUserStatus() const {return this->_registerStatus.user;}

bool Client::getRegisteredStatus() const {return this->_registerStatus.registered;}

std::string Client::getPrefix() const {return this->_prefix;}

/* other */
void Client::appendToRecvBuffer(const std::string& str) {
	this->_recvBuffer += str;
}

bool Client::extractMessage(std::string& message) {
	size_t pos = _recvBuffer.find("\r\n");
	if (pos != std::string::npos) {
		message = _recvBuffer.substr(0, pos);
		_recvBuffer.erase(0, pos + 2);
		return true;
	}
	return false;
}

bool Client::isConnectionTimedOut(time_t timeout) {
	time_t now = time(NULL);
	if (now - this->_lastActivityTime > timeout)
		return true;
	return false;
}

void Client::appendToSendBuffer(const std::string& data) {
	this->_sendBuffer += data;
}

const std::string& Client::getSendBuffer() const {
	return this->_sendBuffer;
}

void Client::clearSendBuffer(size_t count) {
	this->_sendBuffer.erase(0, count);
}

bool Client::hasDataToSend() const {
	if (this->_sendBuffer.empty()) {
		return false;
	}
	return true;
}

void Client::setPrefix() {_prefix = PREFIX_USER(_nickname, _username, _hostname);}

void Client::printLog() {
	std::cout << C_WARN << std::boolalpha
	<< ">>>>> client log <<<<<\n"
	<< "nickname: " << this->getNickname() << "\n"
	<< "username: " << this->getUsername() << "\n"
	<< "hostname: " << this->getHostname() << "\n"
	<< "realname: " << this->getRealname() << "\n"
	// << "servername: " << this->getServername() << "\n"
	<< "registerStatus: " << this->getRegisteredStatus() << "\n"
	<< C_RESET << std::endl;
}
