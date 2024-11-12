#include "Cmd.hpp"

/*
    Author : sejkim2
    Description : /MODE

    처음 /connect 하면 client -> /mode nickname +i <- server

    client -> /mode #test +i +t +o alice +k password +l 10
    server <- MODE #test +itokl alice password 10

    구현해야 할 옵션 : i, t, k, o, l
    -> i, t는 추가 파라미터가 없으므로 계속 추가 가능
    -> k, o, 
    l은 추가 파라미터가 있으므로 3개 제한 (3개 넘기면 다음 메시지 프로토콜로 넘어감)

    #channel option param
    1. 채널 검증
        1) #으로 시작하는가
        2) 존재하는 채널인가
        3) 채널의 운영자가 현재 호출 클라이언트인가
    2. 옵션 검증
        1) +로 시작하는가
        2) {i, t, k, o, l} 인가
    3. 파라미터 검증
        1) k, o, l의 개수와 동일한가
        2) 3개를 넘지 않는가
    4. 옵션 처리
        1) option - param 비교하며 처리

*/

void printParam(std::vector<std::string> param)
{
    std::cout << "this is start" << '\n';
    for(std::vector<std::string>::iterator it = param.begin(); it != param.end(); it++)
        std::cout << *it << "\n";
    std::cout << "this is end" << '\n';
}

// option +-o
void Cmd::validationNickName(std::string nickname, Channel* channel, int option_flag)
{
    //해당 클라이언트가 채널에 없음
    if (option_flag == 1)
    {
        if (channel->isParticipant(nickname) == false)
            throw CmdException(ERR_NOSUCHNICK(this->client->getUsername(), nickname));
    }
    else
    // option_flag == 0, 채널의 운영자에 nickname이 포함되어 있는지 검증
    {
        if (channel->isOperator(nickname) == false)
        {
            throw CmdException(ERR_NOSUCHNICK(this->client->getUsername(), nickname));
        }
    }
}

//option : +k (채널에 password 설정)
void Cmd::validationKey(std::string key, Channel* channel)
{
    if (channel->isSetKey() == false)
        throw CmdException(ERR_KEYSET(channel->getName()));

    if (key.empty()) {
		throw CmdException(ERR_NEEDMOREPARAMS(client->getNickname(), "PASS"));
	}

	/* SIZE check */
	if (key.size() >= PASSWORD_MAX_LEN) {
		throw CmdException(ERR_PASS_PASSWORD);
	}

	/* ALNUM check */
	for (std::string::const_iterator it = key.begin(); it != key.end(); ++it) {
		if (!std::isalnum(static_cast<unsigned char>(*it))) {
			throw CmdException(ERR_PASS_PASSWORD);
		}
	}
}

//option : +l (채널 제한 인원 설정)
void Cmd::validationInt(std::string _size)
{
    size_t _len = _size.length();

    if (_size[0] == '0' && _len == 1) 
        throw CmdException(ERR_INVALIDPARAM);

    if (_size[0] == '0' && _len > 1) 
        throw CmdException(ERR_INVALIDPARAM);
    
    for (size_t i = 0; i < _len; ++i) {
        if (!std::isdigit(_size[i])) {
            throw CmdException(ERR_INVALIDPARAM);
        }
    }
}

void Cmd::addChannelOperator(std::string nickname, Channel* channel)
{
    std::map<std::string, Client*> map = this->server.getNickNameClientMap();
    std::map<std::string, Client*>::iterator client = map.find(nickname);
    
    //client->option_o (flag = 1)
    channel->addOperator(nickname, client->second);
    channel->setMode('o');

    std::map<std::string, Client*> participant = channel->getParticipant();
    channel->removeParticipant(nickname);
    channel->setParticipant("@" + nickname, client->second);
}

// #ch -o user
void Cmd::removeChannelOperator(std::string nickname, Channel* channel)
{
    std::map<std::string, Client*> map = this->server.getNickNameClientMap();
    std::map<std::string, Client*>::iterator client = map.find(nickname);
    
    //client->option_o (flag = 0)
    channel->removeOperator(client->second);
    channel->removeMode('o');

    std::map<std::string, Client*> participant = channel->getParticipant();
    channel->removeParticipant(nickname);
    channel->setParticipant(nickname, client->second);
}

void Cmd::setChannelKey(std::string key, Channel* channel)
{
    channel->setKey(key);
    channel->setMode('k');
}

void Cmd::removeChannelKey(Channel* channel)
{
    channel->removeKey();
    channel->removeMode('k');
}

void Cmd::setChannelUserLimit(std::string _size, Channel* channel)
{
    channel->setLimit(atoi(_size.c_str()));
    channel->setMode('l');
}

void Cmd::removeChannelUserLimit(Channel* channel)
{
    channel->setLimit(DEFAULT_LIMIT);
    channel->removeMode('l');
}

void Cmd::handleMinusFlagOption(std::vector<std::string> modeParse, std::map<std::string, Channel*>::iterator channel, int option_flag)
{
    //{i, t, k, o, l} 인가
        std::string _option = modeParse[1].substr(1);
        std::vector<char> mode_kol;
        int flag_i = 0;
        int flag_t = 0;
        for (size_t i = 0; i < _option.length(); i++)
        {
            if (_option[i] == 'o' || _option[i] == 'k' || _option[i] == 'l')
                mode_kol.push_back(_option[i]);
            else if (_option[i] == 'i')
                flag_i = 1;
            else if (_option[i] == 't')
                flag_t = 1;
            else
                throw CmdException(ERR_UNKNOWNMODE(std::string(1, _option[i])));
        }
        //o, k, l은 혼합하여 3개까지 가능
        if (mode_kol.size() > 3)
            throw CmdException(ERR_NEEDMOREPARAMS(this->client->getUsername(), this->cmdParams));

        // k, o, l의 개수와 파라미터 개수가 동일한가
        // 3개를 넘지 않는가
        std::vector<std::string> param(modeParse.begin() + 2, modeParse.end());
        if (param.size() != mode_kol.size() || param.size() > 3)
            throw CmdException(ERR_NEEDMOREPARAMS(this->client->getUsername(), this->cmdParams));

        // param validation
        size_t option_index = 0;
        size_t param_index = 0;
        size_t _size = mode_kol.size();
        while (option_index < _size)
        {
            if (_option[option_index] == 'o')
                validationNickName(param[param_index], channel->second, option_flag);
            else if (_option[option_index] == 'k')
                validationKey(param[param_index], channel->second);
            //(_option[option_index] == 'l')
            else
                validationInt(param[param_index]);
            option_index++;
            param_index++;
        }

        // executeOption
        if (flag_i == 1)
            channel->second->setMode('i');
        if (flag_t == 1)
            channel->second->setMode('t');

        option_index = 0;
        param_index = 0;
        while (option_index < _size)
        {
            if (_option[option_index] == 'o')
                addChannelOperator(param[param_index], channel->second);
            else if (_option[option_index] == 'k')
                setChannelKey(param[param_index], channel->second);
            //(_option[option_index] == 'l')
            else
                setChannelUserLimit(param[param_index], channel->second);
            option_index++;
            param_index++;
        }
}

void Cmd::handlePlusFlagOption(std::vector<std::string> modeParse, std::map<std::string, Channel*>::iterator channel, int option_flag)
{
    //{i, t, k, o, l} 인가
        std::string _option = modeParse[1].substr(1);
        std::vector<char> mode_kol;
        int flag_i = 0;
        int flag_t = 0;
        int flag_l = 0;
        int flag_k = 0;
        for (size_t i = 0; i < _option.length(); i++)
        {
            if (_option[i] == 'o')
                mode_kol.push_back(_option[i]);
            else if (_option[i] == 'i')
                flag_i = 1;
            else if (_option[i] == 't')
                flag_t = 1;
            else if (_option[i] == 'l')
                flag_l = 1;
            else if (_option[i] == 'k')
                flag_k = 0;
            else
                throw CmdException(ERR_UNKNOWNMODE(std::string(1, _option[i])));
        }
        if (mode_kol.size() > 3)
            throw CmdException(ERR_NEEDMOREPARAMS(this->client->getUsername(), this->cmdParams));

        // o의 개수와 파라미터 개수가 동일한가
        // 3개를 넘지 않는가
        std::vector<std::string> param(modeParse.begin() + 2, modeParse.end());
        if (param.size() != mode_kol.size() || param.size() > 3)
            throw CmdException(ERR_NEEDMOREPARAMS(this->client->getUsername(), this->cmdParams));

        // param validation
        size_t option_index = 0;
        size_t param_index = 0;
        size_t _size = mode_kol.size();
        while (option_index < _size)
        {
            if (_option[option_index] == 'o')
                validationNickName(param[param_index], channel->second, option_flag);
            option_index++;
            param_index++;
        }

        // executeOption
        if (flag_i == 1)
            channel->second->removeMode('i');
        if (flag_t == 1)
            channel->second->removeMode('t');
        if (flag_l == 1)
            channel->second->removeMode('l');
        if (flag_k == 1)
            removeChannelKey(channel->second);

        option_index = 0;
        param_index = 0;
        while (option_index < _size)
        {
            if (_option[option_index] == 'o')
                removeChannelOperator(param[param_index], channel->second);
            option_index++;
            param_index++;
        }
}

void Cmd::cmdMode()
{
    std::vector<std::string> modeParse;
    std::istringstream stream(this->cmdParams);
    std::string token;
    std::map<std::string, Channel*> _channels = this->server.getChannels();
    int option_flag = 0;
    
    while (stream >> token)
        modeParse.push_back(token);

    std::string channelName = modeParse[0];

    //servername validation
    std::map<std::string, Channel*>::iterator channel = _channels.find(channelName);

    //존재하는 채널인가
    if (channel == _channels.end())
        throw CmdException(ERR_NOSUCHCHANNEL(this->client->getUsername(), modeParse[0]));
    //호출자가 현재 채널에 참여하고 있는가
    std::map<std::string, Client*> participaciant = channel->second->getParticipant();
    if (participaciant.find(channel->second->isOperatorNickname(this->client->getNickname())) == participaciant.end())
        throw CmdException(ERR_NOTONCHANNEL(this->client->getNickname(), channelName));

    // /mode #ch -> #ch의 옵션 정보 출력
    if (modeParse.size() == 1)
    {
        // std::set<char> s = channel->second->getMode();
        // std::string mode;
        // for(std::set<char>::iterator it = s.begin(); it != s.end(); it++)
        //     mode += *it;
        // if (s.size() == 0)
        //     mode = ":";
        // else
        //     mode = "+" + mode;
        // server.broadcastMsg(server.makeMsg(RPL_CHANNELMODEIS(modeParse[0], params)), channel->second);
        return;
    }
    //채널의 운영자가 현재 호출 클라이언트인가
    if ((channel->second)->isOperator(this->client->getNickname()) == false)
        throw CmdException(ERR_CHANOPRIVSNEEDED(this->client->getUsername(), modeParse[0]));

    // option validation // 옵션이 +나 -로 시작하는가
    if (modeParse[1].at(0) == '+')
        option_flag = 1;
    else if (modeParse[1].at(0) == '-')
        option_flag = 0;
    else
        throw CmdException(ERR_UNKNOWNMODE(std::string(1, modeParse[1].at(0))));

    //excute
    if (option_flag == 1)
        handleMinusFlagOption(modeParse, channel, option_flag);
    else
        handlePlusFlagOption(modeParse, channel, option_flag);

    std::string str = "MODE";
    for(size_t i = 0; i < modeParse.size() - 1; i++)
        str += " " + modeParse[i];
    str += " :" + modeParse[modeParse.size() - 1];
    server.broadcastMsg(server.makeMsg(USER_PREFIX(this->client->getNickname(), this->client->getUsername(), this->client->getHostname()), str), channel->second, this->client->getFd());
}
