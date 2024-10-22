#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Server.hpp"
#include "Client.hpp"

class Channel {
	private:
		std::string		_name;
		std::string		_topic;
		std::string		_key; // 채널에 들어갈 때 필요한 비밀번호
		std::vector<Client*> _clients;
};
	
#endif