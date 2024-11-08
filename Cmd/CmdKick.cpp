#include "Cmd.hpp"

/*
명령어: KICK <channel> <nickname> [<comment>]

Numeric Replies:
ERR_NEEDMOREPARAMS - 파라미터 부족
ERR_BADCHANMASK - 주어진 채널이름의 형식이 잘못됨. (채널이름이 규칙에 맞지 않는 경우, #으로 시작하지 않는 등..)
ERR_NOSUCHCHANNEL - 채널이 채널 목록에 존재하지 않음
ERR_NOTONCHANNEL - 강퇴시킬 사용자가 채널에 참여자가 아님
ERR_CHANOPRIVSNEEDED - 채널 오퍼레이터 권한이 없음

- 채널에서 사용자를 강제로 퇴장시킬 때 사용하는 명령어 (강제 part)
- <channel>은 사용자를 강제로 퇴장시킬 채널 이름
- <nickname>은 강제로 퇴장시킬 사용자의 닉네임
- <comment>는 선택사항으로, 사용자를 강제로 퇴장시킬 때 이유를 명시

경우의 수
1. 파라미터 부족/과다 : 최소 채널이름과 닉네임이 필요 (총 2개, 최대 3개)
2. 채널 파라미터 잘못된 경우
	2.1 채널이름의 형식이 잘못된 경우
	2.2 채널이 존재하지 않는 경우
3. 강퇴시킬 사용자가 채널의 참여자가 아닌 경우
4. 채널 오퍼레이터 권한이 없는 경우
*/

std::vector<std::string> Cmd::split(char delimeter) {
	std::vector<std::string> res;
	std::string paramCopy = getCmdParams();

	for (size_t i = 0; i < paramCopy.size(); i++) {
		if (paramCopy[i] == delimeter) {
			res.push_back(paramCopy.substr(0, i - 1));
			paramCopy = paramCopy.substr(i + 1);
		}
	}
	res.push_back(paramCopy);

	return res;
}

void Cmd::cmdKick() {
	std::vector<std::string> params = split(' ');

	// 파라미터 부족/과다 (최소 2개, 최대 3개)
	if (params.size() < 2 || params.size() > 3) {
		server.castMsg(this->client_fd, server.makeMsg(ERR_NEEDMOREPARAMS(client->getNickname(), cmd)));
		throw Cmd::CmdException(ERR_NEEDMOREPARAMS(client->getNickname(), cmd));
	}

	std::string chName = params[0];
	std::string target = params[1];
	std::string comment = "";

	if (params.size() == 3)
		std::string comment = params[2];
	
	// 채널 이름의 형식이 잘못된 경우
	if (chName[0] != '#') {
		server.castMsg(this->client_fd, server.makeMsg(ERR_BADCHANMASK(client->getNickname(), chName)));
		throw Cmd::CmdException(ERR_BADCHANMASK(client->getNickname(), chName));
	}
	// 채널이 존재하지 않는 경우
	if (server.getChannels().find(chName) == server.getChannels().end()) {
		server.castMsg(this->client_fd, server.makeMsg(ERR_NOSUCHCHANNEL(client->getNickname(), chName)));
		throw Cmd::CmdException(ERR_NOSUCHCHANNEL(client->getNickname(), chName));
	}

	std::map<std::string, Channel*> chs = server.getChannels();
	std::map<std::string, Channel*>::iterator it = chs.find(chName);
	Channel* ch = it->second;

	// 강퇴시킬 사용자가 채널의 참여자가 아닌 경우
	if (ch->getParticipant().find(ch->isOperatorNickname(target)) == ch->getParticipant().end()) {
		server.castMsg(this->client_fd, server.makeMsg(ERR_NOTONCHANNEL(client->getNickname(), chName)));
		throw Cmd::CmdException(ERR_NOTONCHANNEL(client->getNickname(), chName));
	}

	// 채널 오퍼레이터 권한이 없는 경우
	if (ch->isOperator(client->getNickname()) == false) {
		server.castMsg(this->client_fd, server.makeMsg(ERR_CHANOPRIVSNEEDED(client->getNickname(), chName)).c_str());		
		throw Cmd::CmdException(ERR_CHANOPRIVSNEEDED(client->getNickname(), chName));
	}
	
	// 강퇴 되었다고 채널의 모든 참여자(강퇴 대상자 포함)에게 알림 후 강퇴
	// server.broadcastMsg(server.makeMsg(RPL_KICK(client->getNickname(), client->getUsername(), client->getHostname(), chName, target, comment).c_str()), ch);
	ch->removeParticipant(target);
}
