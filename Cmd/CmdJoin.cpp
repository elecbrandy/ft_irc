// #include "Cmd.hpp"
// #include "../Server.hpp"
// #include "../Channel.hpp"
// #include "../Client.hpp"

// /*
// JOIN 명령어 : /join #채널명1(,#채널명2,...) (비밀번호1,비밀번호2,...)

// 0. 여러 채널 동시 요청 (join 명령어 파싱 후 for문으로 처리)
// 	각각의 채널에 대해 위 조건을 개별적으로 처리.
// 1. 채널 존재 O
// 	클라이언트를 채널에 참여시키고, 채널 참가자들에게 참여 알림을 보냄.
// 2. 채널 존재 X
// 	새로운 채널을 생성하고 클라이언트를 운영자로 설정.
// 3. 비밀번호가 필요한 채널
// 	비밀번호가 없거나 틀리면 ERR_BADCHANNELKEY (475) 전송.
// 4. 인원이 초과된 채널
// 	인원이 가득 찼으면 ERR_CHANNELISFULL (471) 전송.
// 5. 초대 전용 채널
// 	초대를 받지 않은 경우 ERR_INVITEONLYCHAN (473) 전송.
// 6. 차단된 채널
// 	클라이언트가 차단된 상태면 ERR_BANNEDFROMCHAN (474) 전송.
// */

// // 채널 이름과 비밀번호를 ','로 구분하여 vector에 저장
// std::vector<std::string> Cmd::joinSplit(std::string &cmdParams) {
// 	std::vector<std::string> res;
//     std::string tmp;

//     for (size_t i = 0; i < cmdParams.size(); i++) {
//         char c = cmdParams[i];
//         if (c == ',' || c == ' ') {
//             if (!tmp.empty()) {
//                 res.push_back(tmp);
//                 tmp.clear();
//             }
// 		} else
//             tmp += c;
//     }	

//     if (!tmp.empty())
//         res.push_back(tmp);

//     return res;
// }

// /*
// 채널 이름이 유효한지 확인하는 함수
// 	1. 채널 이름 길이가 200자 이하인지 확인
// 	2. 채널 이름이 '#'으로 시작하는지 확인
// 	3. 채널 이름에 공백이나 제어문자가 포함되어 있는지 확인
// */
// bool Cmd::isValidChannelName(std::vector<std::string> &channel) {
// 	for (size_t i = 0; i < channel.size(); i++) {
// 		if (channel[i].size() > 200 || channel[i][0] != '#') {
// 			server.castMsg(client_fd, server.makeMsg(ERR_NOSUCHCHANNEL(client->getNickname(), channel[i])).c_str());
// 			channel.erase(channel.begin() + i);
// 			return false;
// 		}
// 		for(size_t j = 1; j < channel[i].size(); j++) {
// 			/* iscntrl: 문자가 ASCII 제어 문자인지 확인하는 함수. 0x00부터 0x1F, 0x7F를 제어 문자로 인식 */
// 			if (channel[i][j] == ' ' || iscntrl(channel[i][j])) {
// 				server.castMsg(client_fd, server.makeMsg(ERR_NOSUCHCHANNEL(client->getNickname(), channel[i])).c_str());
// 				channel.erase(channel.begin() + i);
// 				return false;
// 			}
// 		}
// 	}
// 	return true;
// }

// void Cmd::cmdJoin() {
// 	std::vector<std::string> joinTokens = joinSplit(cmdParams);
// 	std::vector<std::string> channel;
// 	std::vector<std::string> key;

// 	// 채널과 키 분리
// 	for (std::vector<std::string>::size_type i = 0; i < joinTokens.size(); ++i) {
// 		if (joinTokens[i][0] == '#')
// 			channel.push_back(joinTokens[i]);
// 		else
// 			key.push_back(joinTokens[i]);
// 	}

// 	// 비밀번호가 더 많을 때 에러
// 	if (channel.size() < key.size()) {
// 		server.castMsg(client_fd, server.makeMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "JOIN")).c_str());
// 		return;
// 	}

// 	// 유효하지 않은 채널 이름일 때 에러
// 	if (!isValidChannelName(channel)) {
// 		server.castMsg(client_fd, server.makeMsg(ERR_NOSUCHCHANNEL(client->getNickname(), channel[0])).c_str());
// 		return;
// 	}

// 	for (std::vector<std::string>::size_type i = 0; i < channel.size(); ++i) {
// 		std::string chName = channel[i];
// 		std::string chKey = (i < key.size()) ? key[i] : "";

// 		std::map<std::string, Channel*> channels = server.getChannels();

// 		// 채널이 없을 때 생성 및 설정
// 		if (channels.find(channel[i]) == channels.end()) {
// 			server.setChannels(chName, chKey.empty() ? "" : chKey, chKey.empty() ? '\0' : KEY_MODE);
// 			std::map<std::string, Channel*> channelss = server.getChannels();
// 			channelss.find(chName)->second->addOperator(this->client->getNickname(), this->client);
// 		}

// 		std::map<std::string, Channel*> chs = server.getChannels();
// 		std::map<std::string, Channel*>::iterator it = chs.find(chName);
// 		Channel* ch = it->second;



// 		// 인원 초과 시 에러
// 		if (ch->getMode().find(LIMIT_MODE) != ch->getMode().end() &&
// 		    static_cast<unsigned int>(ch->getParticipant().size()) >= ch->getLimit()) {
// 			server.castMsg(client_fd, server.makeMsg(ERR_CHANNELISFULL(client->getNickname(), chName)).c_str());
// 			return;
// 		}

// 		// 초대 전용 채널일 때 초대 확인
// 		if (ch->getMode().find(INVITE_MODE) != ch->getMode().end()) {
// 			std::vector<std::string> invitationList = ch->getInvited();
// 			if (std::find(invitationList.begin(), invitationList.end(), client->getNickname()) == invitationList.end()) {
// 				server.castMsg(client_fd, server.makeMsg(ERR_INVITEONLYCHAN(client->getNickname(), chName)).c_str());
// 				return;
// 			}
// 		}

// 		// 비밀번호 필요 시 확인
// 		if (ch->getMode().find(KEY_MODE) != ch->getMode().end()) {
// 			if (chKey.empty() || chKey != ch->getKey()) {
// 				server.castMsg(client_fd, server.makeMsg(ERR_BADCHANNELKEY(client->getNickname(), chName)).c_str());
// 				return;
// 			}
// 		}

// 		// 참여자 추가
// 		 std::map<std::string, Client*> participant = ch->getParticipant();
// 		 std::string participantName = (participant.empty()) ? '@' + client->getNickname() : client->getNickname();
// 		 ch->setParticipant(participantName, client);

// 		// 참여자 목록 전송
// 		server.castMsg(client_fd, server.makeMsg(RPL_NAMREPLY(client->getNickname(), "=", ch->getName(), ch->getParticipantNameStr())).c_str());
// 		server.castMsg(client_fd, server.makeMsg(RPL_ENDOFNAMES(client->getNickname(), ch->getName())).c_str());

// 		// JOIN 알림
// 		server.broadcastMsg(server.makeMsg(RPL_JOIN(client->getServername(), chName)), ch, -1);
		
// 		// 토픽 전송
// 		if (ch->getTopic().empty())
// 			server.castMsg(client_fd, server.makeMsg(RPL_NOTOPIC(client->getNickname(), ch->getName())).c_str());
// 		else
// 			server.castMsg(client_fd, server.makeMsg(RPL_TOPIC(client->getNickname(), ch->getName(), ch->getTopic())).c_str());
		
// 		if (ch->getMode().find(KEY_MODE) != ch->getMode().end())
// 			ch->updateInviteList(client->getNickname());
// 	}
// }
