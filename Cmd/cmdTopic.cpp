#include "Cmd.hpp"

/*
명령어 : /Topic <channel> [<topic>]

토픽 파라미터가 안들어오면 토픽이 리턴된다.
토픽 파라미터가 들어오면 채널 모드에 따라 토픽이 변경된다.

Numeric Replies 종류 :
ERR_NEEDMOREPARAMS 		ERR_NOTONCHANNEL
RPL_NOTOPIC        		RPL_TOPIC
ERR_CHANOPRIVSNEEDED 	(근데 여기에 NOSUCHCHANNEL은 왜 없지? 필요한뎅)

*** 경우의 수 ***
1: 토픽 파라미터가 없을 때
	1-1: 채널에 설정된 토픽이 없을 때
	1-2: 채널에 참여한 클라이언트가 아닐 때
2: 토픽 파라미터가 있을 때
	2-1: topic모드일 때 (오퍼레이터만 토픽 변경 가능)
		2-1-1: topic모드인데 채널에 참여한 클라이언트가 operator가 아닐 때
	2-2: topic모드가 아닐 때 (일반 참여자도 토픽 변경 가능) 
*/

std::vector<std::string> Cmd::topicSplit() {
    std::vector<std::string> res;
	std::string paramCopy = getCmdParams();
    for (size_t i = 0; i < paramCopy.size(); i++) {
        if (paramCopy[i] == ' ') {
            if (paramCopy[i + 1] == ':') {
                // ':'로 시작하는 부분을 만나면, 해당 부분을 그대로 추가하고 종료
                res.push_back(paramCopy.substr(0, i)); // 공백 전까지 추가 후에
                res.push_back(paramCopy.substr(i + 1)); // ':'부터 끝까지 추가함!
                return res;
            } else {
                // 일반 공백일 경우, 분할하여 추가
                res.push_back(paramCopy.substr(0, i));
                paramCopy = paramCopy.substr(i + 1);
                i = -1; // 다음 루프에서 i++ 때문에 0으로 시작
            }
        }
    }
    res.push_back(paramCopy); // 남은 문자열 추가

    return res;
}

void Cmd::cmdTopic() {
	std::vector<std::string> params = topicSplit();
	for (size_t i = 0; i < params.size(); i++) {
		std::cout << "topic params[" << i << "] : " << params[i] << std::endl;
	}
	std::string chName = params[0];
	std::string topic = params[1];

	if (chName.empty())
		server.castMsg(this->client_fd, server.makeMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "TOPIC")));
	
	std::map<std::string, Channel *> channels = server.getChannels();
	if (channels.find(chName) == channels.end()) {
		throw Cmd::CmdException(ERR_NOSUCHCHANNEL(client->getNickname(), chName));
		server.castMsg(this->client_fd, server.makeMsg(ERR_NOSUCHCHANNEL(client->getNickname(), chName)));
	}

	Channel *ch = channels[chName];

	if (topic.empty()) { // 1: 토픽 파라미터가 없을 때

		if (ch->getTopic().empty()) { // 1-1: 채널에 설정된 토픽이 없을 때
			server.castMsg(this->client_fd, server.makeMsg(RPL_NOTOPIC(client->getNickname(), chName)));
			throw Cmd::CmdException(RPL_NOTOPIC(client->getNickname(), chName));
		}

		if (ch->getParticipant().find(client->getNickname()) == ch->getParticipant().end()) { // 1-2: 채널에 참여한 클라이언트가 아닐 때
			server.castMsg(this->client_fd, server.makeMsg(ERR_NOTONCHANNEL(client->getNickname(), chName)));
			throw Cmd::CmdException(ERR_NOTONCHANNEL(client->getNickname(), chName));
		}

		server.broadcastMsg(server.makeMsg(RPL_TOPIC(client->getNickname(), chName, ch->getTopic())), ch, -1);
	} else { // 2: 토픽 파라미터가 있을 때
		if (ch->getParticipant().find(client->getNickname()) == ch->getParticipant().end()) { // 2-1: 채널에 참여한 클라이언트가 아닐 때
			server.castMsg(this->client_fd, server.makeMsg(ERR_NOTONCHANNEL(client->getNickname(), chName)));
			throw Cmd::CmdException(ERR_NOTONCHANNEL(client->getNickname(), chName));
		}

		bool tFlag = false; // 토픽 모드인지 확인하기 위한 플래그
		if (ch->getMode().find(TOPIC_MODE) != ch->getMode().end())
			tFlag = true;

		if (tFlag == true) { // 2-1: topic모드일 때
			{
				if (ch->getOperator().find(client->getNickname()) == ch->getOperator().end()) {
					// 2-1-1: topic모드인데 채널에 참여한 클라이언트가 operator가 아닐 때
					server.castMsg(this->client_fd, server.makeMsg(ERR_CHANOPRIVSNEEDED(client->getNickname(), chName)));
					throw Cmd::CmdException(ERR_CHANOPRIVSNEEDED(client->getNickname(), chName));
				}
				ch->setTopic(topic);
				server.broadcastMsg(server.makeMsg(RPL_TOPIC(client->getNickname(), chName, ch->getTopic())), ch, -1);
			}
		} else { // 2-2: topic모드가 아닐 때 (일반 참여자도 토픽 변경 가능)
			ch->setTopic(topic);
			server.broadcastMsg(server.makeMsg(RPL_TOPIC(client->getNickname(), chName, ch->getTopic())), ch, -1);
		}

	}
}