#include "Cmd.hpp"

/*
명령어 : PART <channel>{,<channel>}

ERR_NEEDMOREPARAMS
ERR_NOSUCHCHANNEL
ERR_NOTONCHANNEL

- 채널에서 나가는 명령어
- 여러 채널을 나가려면 채널 이름을 콤마로 구분하여 나열

경우의 수
1. 파라미터 부족 : 파라미터가 없는 경우 (최소 1개 있어야함)
2. 해당 채널이 존재하지 않는 경우(#으로 시작하지 않는 경우와 그냥 존재하지 않는 경우)
3. 채널은 존재하지만 사용자가 해당 채널에 참여하지 않은 경우
*/

void Cmd::cmdPart() {
    std::vector<std::string> params = split(',');

    for (size_t i = 0; i < params.size(); i++) {
        std::cout << "params[" << i << "]: |" << params[i] << "|" << std::endl;
    }
    
    if (params.empty()){
        server.castMsg(this->client_fd, server.makeMsg(ERR_NEEDMOREPARAMS(client->getNickname(), cmd)));
        throw Cmd::CmdException(ERR_NEEDMOREPARAMS(client->getNickname(), cmd));
    }

    for (size_t i = 0; i < params.size(); i++) {
        std::string chName = params[i];

        if (chName[0] != '#') {
            server.castMsg(this->client_fd, server.makeMsg(ERR_NOSUCHCHANNEL(client->getNickname(), chName)));
            throw Cmd::CmdException(ERR_NOSUCHCHANNEL(client->getNickname(), chName));
            continue;
        }

        std::map<std::string, Channel*> chs = server.getChannels();
        if (chs.find(chName) == chs.end()) {
            server.castMsg(this->client_fd, server.makeMsg(ERR_NOSUCHCHANNEL(client->getNickname(), chName)));
            throw Cmd::CmdException(ERR_NOSUCHCHANNEL(client->getNickname(), chName));
            continue;
        }

        Channel* ch = chs[chName];
        if (ch->getParticipant().find(client->getNickname()) == ch->getParticipant().end()) {
            server.castMsg(this->client_fd, server.makeMsg(ERR_NOTONCHANNEL(client->getNickname(), chName)));
            throw Cmd::CmdException(ERR_NOTONCHANNEL(client->getNickname(), chName));
            continue;
        }

        // server.broadcastMsg(server.makeMsg(RPL_PART(client->getNickname(), chName)).c_str());
        ch->removeParticipant(client->getNickname());
        
        if (ch->getParticipant().empty())
            server.removeChannel(chName);
    }
}