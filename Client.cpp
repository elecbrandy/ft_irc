#include "Client.hpp"

Client::Client() {}

Client::Client(const std::string& str) : name(str) {}

Client:: ~Client() {}

std::string	Client::getName() const {
	return this->name;
}

void Client::setName(const std::string& str) {
	this->name = str;
}