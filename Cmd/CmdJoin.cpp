#include "Cmd.hpp"

std::vector<std::string> Cmd::joinSplit(std::string &cmdParams) {
	std::vector<std::string> res;
    std::string tmp;

    for (size_t i = 0; i < cmdParams.size(); i++) {
        char c = cmdParams[i];
        if (c == ',' || c == ' ') {
            if (!tmp.empty()) {
                res.push_back(tmp);
                tmp.clear();
            }
		} else
            tmp += c;
    }	

    if (!tmp.empty())
        res.push_back(tmp);

    return res;
}

void Cmd::cmdJoin(std::string &cmdParams, int client_fd) {
	// Client* client = getClient(client_fd);
	(void)client_fd;

	std::string chName;
	std::string chKey;

	std::vector<std::string> joinTokens = joinSplit(cmdParams);
	std::vector<std::string> channel;
	std::vector<std::string> key;
	
	for (size_t i = 0; i < joinTokens.size(); i++) {
		if (joinTokens[i][0] == '#')
			channel.push_back(joinTokens[i]);
		else
			key.push_back(joinTokens[i]);
	}

	for (size_t i = 0; i < channel.size(); i++) {
		chName = channel[i];	
		bool isOperator = false;

		if (i < key.size())
			chKey = key[i];

		if (_channels.find(channel[i]) == _channels.end()) {
			isOperator = true;
			_channels[chName] = new Channel(chName);
			if (!chKey.empty()) {
				_channels[chName]->setMode(KEY_MODE);
				_channels[chName]->setKey(chKey);
			}
		}

		// Channel *ch = _channels[chName];
		// std::string participantName = "";

		// std::map<std::string, Client*>::iterator begin = ch->getParticipant().begin();
		// std::map<std::string, Client*>::iterator end = ch->getParticipant().end();

		// if (begin == end)
		// 	participantName = '@' + client->getNickname();
		// else {
		// 	participantName = client->getNickname();
		
		// ch->setParticipant(participantName, client);
		// ch->addParticipantname(participantName);
		
		// castMsg(client_fd, makeMsg(RPL_NAMREPLY(client->getNickname(), ch->getName(), participantName)).c_str());
	}






}