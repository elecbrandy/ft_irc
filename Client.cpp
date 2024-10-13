#include "Client.hpp"

Client::Client() {}

Client::Client(const std::string& str) : name(str) {}

Client:: ~Client() {}

/* setter */
void Client::setNickname(const std::string& str) {this->_nickname = str;}

void Client::setUsername(const std::string& str) {this->_username = str;}

void Client::setHostname(const std::string& str) {this->_hostname = str;}

void Client::setRealname(const std::string& str) {this->_realname = str;}

void Client::setPassword(const std::string& str) {this->_password = str;}

void Client::setIpAddress(const std::string& str) {this->_ipAddress = str;}

void Client::setServername(const std::string& str) {this->_servername = str;}

/* getter */
std::string	Client::getNickname() const {return this->_nickname;}

std::string	Client::getUsername() const {return this->_username;}

std::string	Client::getHostname() const {return this->_hostname;}

std::string	Client::getRealname() const {return this->_realname;}

std::string	Client::getPassword() const {return this->_password;}

std::string	Client::getIpAddress() const {return this->_ipAddress;}

std::string	Client::getServername() const {return this->_servername;}
