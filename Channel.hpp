#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <set>
#include <vector>
// #include "Server.hpp"
#include "Client.hpp"

#define OPERATOR_MODE 'o'
#define PRIVATE_MODE 'p'
#define SECRET_MODE 's'
#define INVITE_MODE 'i'
#define TOPIC_MODE 't'
#define NOEXTERNALMSG_MODE 'n'
#define MODERATED_MODE 'm'
#define LIMIT_MODE 'l'
#define BAN_MASK_MODE 'b'
#define KEY_MODE 'k'
#define VOICE_MODE 'v'

class Channel {
	private:
		std::string				_name; //채널명
		std::string				_key; //채널 비밀번호
		std::string				_topic; //채널 주제
		std::set<char>			_mode; //채널 모드
		std::set<Client*> 		_participant; //채널 참여자
		std::set<Client*>		_operator; //채널 운영자  //set으로 하면 중복 안되고 자동 정렬이 유용함
	public:
		Channel(std::string name);
		~Channel();

		void setKey(std::string key);
		void setTopic(std::string topic);
		void setMode(char mode);
		void setParticipant(Client* client);

		std::string getName();
		std::string getKey();
		std::string getTopic();
		std::set<char> getMode();
		std::set<Client*> getParticipant();
};
	
#endif