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
    std::map<std::string, Client*> map = this->server.getNickNameClientMap();
    std::map<std::string, Client*>::iterator client = map.find(nickname);
    
    //해당 클라이언트 등록 안되어 있음
    if (option_flag == 1)
    {
        if (client == map.end())
            throw CmdException(ERR_NOSUCHNICK(this->client->getUsername(), nickname));
    }
    else
    // option_flag == 0, 채널의 운영자에 nickname이 포함되어 있는지 검증
    {
        if (channel->isOperator(client->second) == false)
            throw CmdException(ERR_NOSUCHNICK(this->client->getUsername(), nickname));
    }
}

//option : +k (채널에 password 설정)
void Cmd::validationKey(std::string key)
{
    checkPassword(key);
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
}

// #ch -o user
void Cmd::removeChannelOperator(std::string nickname, Channel* channel)
{
    std::map<std::string, Client*> map = this->server.getNickNameClientMap();
    std::map<std::string, Client*>::iterator client = map.find(nickname);
    
    //client->option_o (flag = 1)
    channel->removeOperator(client->second);
}

void Cmd::setChannelKey(std::string key, Channel* channel)
{
    channel->setKey(key);
}

void Cmd::removeChannelKey(Channel* channel)
{
    channel->removeKey();
}

void Cmd::setChannelUserLimit(std::string _size, Channel* channel)
{
    channel->setLimit(atoi(_size.c_str()));
}

void Cmd::removeChannelUserLimit(Channel* channel)
{
    // 미구현
    channel->setLimit(DEFAULT_LIMIT);
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
                throw CmdException(ERR_NOPRIVILEGES(this->client->getUsername()));
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
                validationKey(param[param_index]);
            //(_option[option_index] == 'l')
            else
                validationInt(param[param_index]);
            option_index++;
            param_index++;
        }

        // executeOption
        if (flag_i == 1)
            channel->second->set_b_invite_mode(false);
        if (flag_t == 1)
            channel->second->set_b_topic_mode(false);

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
        // std::cout << "flag 1 success!!!!!!!!!!!!!!!" << '\n';
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
                throw CmdException(ERR_NOPRIVILEGES(this->client->getUsername()));
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
            channel->second->set_b_invite_mode(true);
        if (flag_t == 1)
            channel->second->set_b_topic_mode(true);
        if (flag_l == 1)
            removeChannelUserLimit(channel->second);
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
        // std::cout << "flag 0 success!!!!!!!!!!!!!!!" << '\n';
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

    // printParam(modeParse);

    //mode nickname +i (/connect 하고 한번 요청, 과제 요구사항 x)
    //채널 이름이 #으로 시작하는가
    if (modeParse.size() < 1 || modeParse[0].at(0) != '#')
        throw CmdException(ERR_NEEDMOREPARAMS(this->client->getUsername(), this->cmdParams));

    std::string channelName = modeParse[0].substr(1);

    // test channel
    // Channel* testChannel = new Channel(channelName);
    // testChannel->addOperator(this->client);
    // _channels[channelName] = testChannel;

    //servername validation
    std::map<std::string, Channel*>::iterator channel = _channels.find(channelName);

    //존재하는 채널인가
    if (channel == _channels.end())
        throw CmdException(ERR_NOSUCHCHANNEL(this->client->getUsername(), modeParse[0]));
    //채널의 운영자가 현재 호출 클라이언트인가
    if ((channel->second)->isOperator(this->client) == false)
        throw CmdException(ERR_CHANOPRIVSNEEDED(this->client->getUsername(), modeParse[0]));

    // option validation
    // 옵션이 +나 -로 시작하는가
    if (modeParse[1].at(0) == '+')
        option_flag = 1;
    else if (modeParse[1].at(0) == '-')
        option_flag = 0;
    else
        throw CmdException(ERR_UNKNOWNMODE(this->client->getUsername(), modeParse[1], channel->second->getName()));

    //excute
    if (option_flag == 1)
        handleMinusFlagOption(modeParse, channel, option_flag);
    else
        handlePlusFlagOption(modeParse, channel, option_flag);
}
