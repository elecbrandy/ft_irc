#include "Cmd.hpp"

/*
명령어: PRIVMSG <receiver>{,<receiver>} <msg to send>

- 사용자가 다른 사용자에게 메시지를 보낼 때 사용하는 명령어
- <receiver>는 메시지를 받을 사용자의 닉네임이나 채널 이름
- 여러 사용자에게 메시지를 보낼 때는 <receiver>를 콤마로 구분
- PRIVMSG 명령어는 특정서버나, 특정 호스트(특정 IP 대역에 속한 사용자)에게도 보낼 수 있음
	- 우리는 단일 서버로 구성되어 있으므로 <receiver>에는 닉네임이나 채널 이름만 사용할 수 있도록 구현한다.
- :는 텍스트 시작을 명확히 구분하기 위해 사용

Numeric Replies:
ERR_NORECIPIENT -> PRIVMSG :Hello, everyone! 처럼 수신자정보가 아예 없는 경우
ERR_NOTEXTTOSEND -> PRIVMSG #channel : 처럼 메시지 부분이 비어있는 경우나 :로 시작하지 않는 경우
ERR_CANNOTSENDTOCHAN -> 참여하고있지 않은 채널에 메시지를 보내는 경우
ERR_TOOMANYTARGETS -> 보통 한번에 보낼 수 있는 수신자의 수를 5로 제한하는데, 이를 초과하는 경우 (테스트 필요함)
ERR_NOSUCHNICK -> 닉네임을 사용하는 클라이언트가 존재하지 않는 경우

(x)ERR_NOTOPLEVEL -> xxx다중서버환경에서 특정한 형식의 수신자 처리와 관련있는 에러라 단일서버환경에서는 일반적으로 발생하지 않음
(x)ERR_WILDTOPLEVEL -> xxx다중서버환경에서 특정한 형식의 수신자 처리와 관련있는 에러라 단일서버환경에서는 일반적으로 발생하지 않음
(x)RPL_AWAY -> xxx수신자가 away명령어를 사용하여 현재 AWAY 상태인 경우, user mode와 연관이 있으므로 구현필요 x

경우의 수
1. 메시지를 받는 사용자/채널이 존재하는지 확인
	- 아예 수신자가 인자로 들어오지 않은 경우 ERR_NORECIPIENT (411)
	- 닉네임이나 채널이 존재하지 않는 경우 ERR_NOSUCHNICK (401)
	- 채널에 참여하지 않은 경우 ERR_CANNOTSENDTOCHAN (404) -> 채널에 참여하지 않은 경우는 채널에 메시지를 보낼 수 있는지? 비밀번호 설정 안되면 보낼 수 있나?
2. 메세지가 비어있지는 않은지, 양식이 잘못되지는 않은지 확인
	- 메시지가 :으로 시작하지 않는 경우, 비어있는경우 ERR_NOTEXTTOSEND (432)
3. 메세지 길이 제한
	- 클라가 보낸 메세지의 모든 구성 요소를 합쳐 셌을때 512자를 넘어가는 경우 ERR_UNKNOWNCOMMAND (421) -> 공식문서 확인해봐야함!!!!
4. privmsg를 통해 채널로 메세지를 보내는 경우에, 사용자가 채널에서 ban 되어있는 경우는 메세지를 보낼 수 없다.
	- 구현을 해야하나? 구현을 한다면 ban모드를 구현을 해야되나? 아니면 모드는 구현 안해도 구색만 갖추게 리스트를 채널이 가지고 있어야 하나?

InspIRCd에서는 혼합 수신자 PRIVMSG를 다음과 같이 처리:
첫 번째 대상의 타입을 기준으로 처리하며, 이후의 수신자들 중 다른 타입의 수신자는 무시
*/

std::vector<std::string> Cmd::privmsgSplit() {
    std::vector<std::string> res;
    std::string paramCopy = cmdParams;
    std::string currentPart;

    for (size_t i = 0; i < paramCopy.size(); ++i) {
        if (paramCopy[i] == ' ' || paramCopy[i] == ',') {
            if (!currentPart.empty()) {
                res.push_back(currentPart);
                currentPart.clear();
            }
            continue; // 빈 문자열을 추가하지 않음
        } else if (paramCopy[i] == ':') {
            if (!currentPart.empty()) {
                res.push_back(currentPart);
                currentPart.clear();
            }
            // ':'로 시작하는 메시지를 시작
            currentPart += ':'; // ':'를 포함
            // ':' 다음의 모든 내용을 추가
            for (++i; i < paramCopy.size(); ++i) {
                currentPart += paramCopy[i];
            }
            break; // 메시지 처리 후 루프 종료
        } else {
            currentPart += paramCopy[i]; // 일반 문자열로 추가
        }
    }

    // 마지막 부분 추가
    if (!currentPart.empty()) {
        res.push_back(currentPart);
    }

    return res;
}

bool Cmd::isDupReceiver(std::vector<std::string> &receivers) {
	std::set<std::string> receiverSet;
    // Set에 삽입을 시도했을 때 이미 존재하는 경우 중복이라고 판단!
    for (std::vector<std::string>::const_iterator it = receivers.begin(); it != receivers.end(); ++it) {
		//insert 메서드는 삽입 결과로 std::pair를 반환합니다. 이 pair의 첫 번째 요소는 삽입된 값이고,
		//두 번째 요소는 삽입 성공 여부를 나타낸다. 삽입이 성공하면 true, 이미 존재하는 값이라면 false를 반환
        if (!receiverSet.insert(*it).second) // 
            return true; // 중복 발견
    }
    return false; // 중복 없음
}

void Cmd::cmdPrivmsg() {
	std::vector<std::string> params = privmsgSplit();
	std::vector<std::string> receivers;
	std::string msg;
	bool chFlag = false;

	for (size_t i = 0; i < params.size(); i++) {
		if (params[i][0] != ':') {
			if (i == 1 && params[i][0] == '#') // 첫 수신자의 타입을 기준으로 PRIVMSG를 처리
				chFlag = true;
			if (chFlag == true && params[i][0] == '#') { // 채널이 첫 수신자인 경우
				receivers.push_back(params[i]);
			}
			if (chFlag == false && params[i][0] != '#') { // 사용자가 첫 수신자인 경우
				receivers.push_back(params[i]);
			}
		} else if (params[i][0] == ':')
			msg = params[i];
	}
	
	//케이스 1 : 수신자가 없는 경우
	if (receivers.empty()) {
		throw Cmd::CmdException(ERR_NORECIPIENT(client->getNickname(), "PRIVMSG")); // getcmd() 해서 객체에 저장해놓고 사용하기
		server.castMsg(client_fd, server.makeMsg(ERR_NORECIPIENT(client->getNickname(), "PRIVMSG")).c_str());
	}
	//케이스 2 : 수신자가 5명을 초과하거나, 수신자가 중복되는 경우
	if (receivers.size() > 5 || isDupReceiver(receivers) == true) {
		std::string receiverStr;
		for (size_t i = 0; i < receivers.size(); i++) {
			receiverStr = receiverStr + receivers[i] + ',';
			if (i == receivers.size() - 1)
				receiverStr += receivers[i];
		}
		throw Cmd::CmdException(ERR_TOOMANYTARGETS(client->getNickname(), receiverStr));
		server.castMsg(client_fd, server.makeMsg(ERR_TOOMANYTARGETS(client->getNickname(), receiverStr)).c_str());
	}
	//케이스 3 : 메시지가 없거나 메세지의 형태가 잘못된 경우 -> 메세지 길이 재는 함수를 만들어야 하나...?
	if (msg.empty() || msg[0] != ':') {
		throw Cmd::CmdException(ERR_NOTEXTTOSEND(client->getNickname()));
		server.castMsg(client_fd, server.makeMsg(ERR_NOTEXTTOSEND(client->getNickname())).c_str());
	}
	//케이스 4 :
	//	수신자 집단의 형태가 채널일 경우 -> 채널 존재 확인 -> 채널이 있으면, 채널에 참여하고 있는지 확인
	//	수신자 집단의 형태가 사용자일 경우 -> 사용자가 존재하는지 확인
	for (size_t i = 0; i < receivers.size(); i++) {
		if (chFlag == true) { // 수신자집단이 채널인 경우
			std::map<std::string, Channel*> chs = server.getChannels();
			std::map<std::string, Channel*>::iterator it = chs.find(receivers[i]);

			if (it == server.getChannels().end()) {
				throw Cmd::CmdException(ERR_NOSUCHNICK(client->getNickname(), receivers[i]));
				server.castMsg(client_fd, server.makeMsg(ERR_NOSUCHNICK(client->getNickname(), receivers[i])).c_str());
			} else {
				if (it->second->getParticipant().find(client->getNickname()) == it->second->getParticipant().end()) {
				throw Cmd::CmdException(ERR_CANNOTSENDTOCHAN(client->getNickname(), receivers[i]));
				server.castMsg(client_fd, server.makeMsg(ERR_CANNOTSENDTOCHAN(client->getNickname(), receivers[i])).c_str());
				}
			}
		} else { // 수신자집단이 사용자인 경우
			Client* receiver = server.getClient(receivers[i]);
			if (receiver == NULL) {
				throw Cmd::CmdException(ERR_NOSUCHNICK(client->getNickname(), receivers[i]));
				server.castMsg(client_fd, server.makeMsg(ERR_NOSUCHNICK(client->getNickname(), receivers[i])).c_str());
			}
		}
	}
	//모든 조건을 만족하면, 수신자 집단에 메세지를 전송
	if (chFlag == true) {
		for (size_t i = 0; i < receivers.size(); i++) {
			std::string privmsg = ':' + client->getNickname() + ' ' + cmd + ' ' + receivers[i] + ' ' +  msg;
			std::map<std::string, Channel*> chs = server.getChannels();
			Channel* ch = chs.find(receivers[i])->second;
			server.broadcastMsg(server.makeMsg(privmsg), ch);
		}
	} else {
		for (size_t i = 0; i < receivers.size(); i++) {
			std::string privmsg = ':' + client->getNickname() + ' ' + cmd + ' ' + receivers[i] + ' ' +  msg;
			Client* receiver = server.getClient(receivers[i]);
			server.castMsg(receiver->getFd(), server.makeMsg(privmsg));
		}
	}
}
