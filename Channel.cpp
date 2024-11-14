//자자 채널을 만들어 보자

// 할일 순서
// 1. 채널을 만들기 위한 클래스 구조를 만든다.
// 2. join 명령어 공부 (파라미터 구조, 이 파라미터를 어떻게 받아서 처리할 것인가)
// 	2.1 파라미터는 채널명과 비밀번호가 있다.
// 3. 해당 채널명 존재 여부 확인, 없으면 채널 만들기
// 4. 채널에 입장하기
// 5. 채널 참여 완료 관련 처리 e.g)메세지 출력 등

// 1. #secret 채널이 invite-only (+i) 모드
// 2. 운영자가 alice를 초대 (/INVITE alice #secret)
// 3. alice가 초대 목록에 추가됨
// 4. alice가 채널 참여 (/JOIN #secret)
// 5. removeInvite로 alice의 초대 정보 삭제
// 6. alice가 나갔다가 다시 들어오려면 새로운 초대 필요

/* dongwook

	< 채널 >
	1. 최소한 하나 이상의 클라이언트로 구성되있고, 마지막 클라이언트가 떠나면 채널 종료
	2. 채널 이름은 문자열 (& or # 으로 시작)이고, 최대 200자 까지 가능. 공백과 제어G 포함X
	3. MODE 변경 가능. MODE에 따라 참가 승인 여부 다름
	4. 사용자가 가입 가능한 채널은 최대 10개 까지
	5. 

	< op >
	1. 채널 운영자 = chop or chanop
	2. 채널 옆에 있는 @으로 식별
	3. KICK, MODE, INVITE, TOPIC 명령어 사용 가능


*/

#include "Channel.hpp"

Channel::Channel(std::string name) :
_name(name), _key(""), _topic(""), _mode() , _participant() {}

Channel::~Channel() {}

/* setter */
void Channel::setKey(std::string key) {_key = key;}

void Channel::setTopic(std::string topic) {_topic = topic;}

void Channel::setMode(char mode) {_mode.insert(mode);}

void Channel::addParticipant(std::string participantName, Client* client) {
    _participant.insert(std::make_pair(participantName, client));
}

// void Channel::setOperator(std::string nick, Client* client) {_operator.insert(std::make_pair(nick, client));}

void Channel::setLimit(unsigned int limit) {_limit = limit;}

void Channel::addInvited(std::string nickname) {_invited.push_back(nickname);}

void Channel::addOperator(std::string nick, Client* client)
{
	this->_operator[nick] = client;
}

void Channel::removeOperator(std::string nickname)
{
	this->_operator.erase(nickname);
}

void Channel::removeKey()
{
	this->_key = "";
}

void Channel::removeMode(char mode)
{
	this->_mode.erase(mode);
}

const std::string Channel::isOperatorNickname(std::string nickname) const
{
	if (isOperator(nickname) == true)
		return "@" + nickname;
	else
		return nickname;
}

void Channel::removeParticipant(std::string target){
	_participant.erase(target);
}

/* getter */
std::string Channel::getName() {return this->_name;}

std::string Channel::getKey() {return this->_key;}

std::string Channel::getTopic() {return this->_topic;}

const std::set<char>& Channel::getMode() const {return this->_mode;}

std::map<std::string, Client*>& Channel::getParticipant() {return this->_participant;}

std::string Channel::getParticipantNameStr() {
    std::string names = "";
    std::map<std::string, Client*>::iterator it = _participant.begin();
    while (it != _participant.end()) {
        // Client 객체에서 순수한 닉네임만 가져오기
        std::string nickname = it->second->getNickname();
        // 운영자(@) 표시가 필요한 경우 보존
        if (it->first[0] == '@') {
            names += '@';
        }
        names += nickname;
        ++it;
        if (it != _participant.end()) {
            names += " ";
        }
    }
    return names;
}

bool Channel::isOperator(std::string nickname) const
{
	if (this->_operator.find(nickname) == this->_operator.end())
		return false;
	else
		return true;
}

bool Channel::isParticipant(std::string nickname) const
{
	if (this->_participant.find(nickname) == this->_participant.end())
		return false;
	else
		return true;
}

bool Channel::isSetKey() const
{
	if (this->_key == "")
		return (false);
	else
		return true;
}

void Channel::removeInvited(std::string nickname) {
	// if (std::find(_invited.begin(), _invited.end(), nickname) != _invited.end())
	_invited.erase(std::remove(_invited.begin(), _invited.end(), nickname), _invited.end());
}

std::map<std::string, Client*> Channel::getOperator() {return this->_operator;}

unsigned int Channel::getLimit() const {return this->_limit;} 

std::vector<std::string> Channel::getInvited() {return this->_invited;}