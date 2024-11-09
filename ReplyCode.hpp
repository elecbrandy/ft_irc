// 001: 서버에 연결됨
#define RPL_WELCOME(nick, servername) 			":" + servername + " 001 " + nick + " :Welcome to the Internet Relay Network, " + nick + "!"

// 002: 서버의 호스트 정보와 버전을 알림
#define RPL_YOURHOST(nick, servername)          "002 " + nick + " :Your host is " + servername

// 003: 서버 생성 시간을 알림
#define RPL_CREATED(nick, time_msg)             "003 " + nick + " :This server was created " + time_msg

// 004: 서버의 설정 및 버전 관련 정보를 알림
#define RPL_MYINFO(nick)                        "004 " + nick + " :ft_irc, Simple IRC server for 42 cursus project"

#define RPL_CHANNELMODEIS(ch, mode, params)     "324 " + ch + " " + mode + " " + params

// 331: 해당 채널에 설정된 토픽이 없음
#define RPL_NOTOPIC(nick, ch)					"331 " + nick + " " + ch + " :No topic is set" 

// 332: 해당 채널의 토픽을 반환
#define RPL_TOPIC(nick, ch, topic)				"332" + nick + " " + ch + " :" + topic

// 341: 해당 채널에 초대가 전송됨
#define RPL_INVITING(nick, target, ch)			"341 " + nick + " " + ch + " " + target

// 353: 해당 채널에 참여 중인 사용자 목록을 반환
#define RPL_NAMREPLY(nick, symbol, ch, participants) "353 " + nick + " " + symbol + " " + ch + " :" + participants

// 366: 해당 채널의 사용자 목록을 모두 반환했음
#define RPL_ENDOFNAMES(nick, ch)				"366 " + nick + " " + ch + " :End of /NAMES list"

// 375: MOTD 메세지의 시작을 알림
#define RPL_MOTDSTART(nick)                     "375 " + nick + " :==========start of /MOTD command"

// 372: MOTD 메세지의 본문
#define RPL_MOTD(nick)                          "372 " + nick + " :========== Welcome to FT_IRC"

// 376: MOTD 메세지의 끝을 알림
#define RPL_ENDOFMOTD(nick)                     "376 " + nick + " :==========end of /MOTD command"

// 401: 대상 닉네임을 가진 사용자가 존재하지 않음
#define ERR_NOSUCHNICK(nick, target)			"401 " + nick + " " + target + " :No such nick/channel"

// 402: 해당 서버를 찾을 수 없음
#define ERR_NOSUCHSERVER						"402 " + nick + " "

// 403: 해당 채널이 존재하지 않음
#define ERR_NOSUCHCHANNEL(nick, ch) 			"403 " + nick + " " + ch + " :No such channel"

// 404: 사용자가 채널에 메시지를 보낼 수 없음
#define ERR_CANNOTSENDTOCHAN(nick, target) 		"404 " + nick + " " + target + " :Cannot send to channel"

// 405: 사용자가 가입할 수 있는 채널 수를 초과함
#define ERR_TOOMANYCHANNELS 					"405 " + nick + " "

// 406: 해당 닉네임을 가진 사용자가 존재하지 않았음
#define ERR_WASNOSUCHNICK 						"406 " + nick + " "

// 407: 대상이 너무 많아서 명령을 처리할 수 없음
#define ERR_TOOMANYTARGETS(nick, target) 		"407 " + nick + " " + target + " :Duplicate recipients. No message delivered"

// 409: 출처가 없거나 누락됨 (PING/PONG)
#define ERR_NOORIGIN 							"409 " + nick + " "

// 411: 명령에 메시지 대상이 없음
#define ERR_NORECIPIENT(nick,cmd) 					"411 " + nick + " " + ":No recipient given (" + cmd + ")"

// 412: 보내야 할 메시지가 없음
#define ERR_NOTEXTTOSEND(nick) 						"412 " + nick + " " ":No text to send"

// 413: 주소가 최상위 도메인이 아님
// #define ERR_NOTOPLEVEL 							"413 " + nick + " "

// 414: 와일드카드가 최상위 도메인에 사용됨
// #define ERR_WILDTOPLEVEL 						"414 " + nick + " "

// 421: 알 수 없는 명령을 보냄
#define ERR_UNKNOWNCOMMAND(nick)  				"421 " + nick + " "

// 422: 서버에 MOTD(메시지)가 설정되지 않음
#define ERR_NOMOTD 								"422 " + nick + " "

// 423: 서버의 관리자 정보를 찾을 수 없음
#define ERR_NOADMININFO 						"423 " + nick + " "

// 424: 파일 관련 에러 (파일 읽기/쓰기 실패 등)
#define ERR_FILEERROR 							"424 " + nick + " "

// 431: 닉네임이 주어지지 않음
#define ERR_NONICKNAMEGIVEN(nick)				(std::string("431 ") + nick + " :No nickname given")

// 432: 잘못된 형식의 닉네임 사용
#define ERR_ERRONEUSNICKNAME(nick)				"432 " + nick + " :Erroneous nickname"

// 433: 닉네임이 이미 사용 중임
#define ERR_NICKNAMEINUSE(nick)					"433 " + nick + " :Nickname is already in use"

// 436: 동일한 닉네임으로 다른 서버에서 충돌 발생
#define ERR_NICKCOLLISION 						"436 " + nick + " "

// 441: 사용자가 해당 채널에 있지 않음
#define ERR_nickNOTINCHANNEL 					"441 " + nick + " "

// 442: 사용자가 해당 채널에 속해 있지 않음
#define ERR_NOTONCHANNEL(nick, ch) 				"442 " + nick + " " + ch + " :You're not on that channel"

// 443: 사용자가 이미 해당 채널에 속해 있음
#define ERR_USERONCHANNEL(nick, ch) 						"443 " + nick + " " + ch + " :is already on channel"

// 444: 사용자가 로그인하지 않음
#define ERR_NOLOGIN 							"444 " + nick + " "

// 445: 서버에서 소환 기능이 비활성화됨
#define ERR_SUMMONDISABLED 						"445 " + nick + " "

// 446: 서버에서 nickS 명령이 비활성화됨
#define ERR_nickSDISABLED 						"446 " + nick + " "

// 451: 클라이언트가 등록되지 않음 (nick/NICK 명령을 사용하지 않음)
#define ERR_NOTREGISTERED 						"451 " + nick + " "

// 461: 명령에 필요한 파라미터가 부족함
#define ERR_NEEDMOREPARAMS(nick, cmd) 		"461 " + nick + " " + cmd + " :Not enough parameters"

// 462: 클라이언트가 이미 등록된 상태에서 다시 등록 시도
#define ERR_ALREADYREGISTERED 					"462 :You may not reregister"
                        
// 463: 호스트에 대한 권한이 부족함
#define ERR_NOPERMFORHOST 						"463 " + nick + " "

// 464: 비밀번호가 틀림
#define ERR_PASSWDMISMATCH 						"464 " + nick + " "

// 465: 클라이언트가 서버에서 금지됨
#define ERR_YOUREBANNEDCREEP 					"465 " + nick + " "

// 467: 채널의 키가 이미 설정되어 있음
#define ERR_KEYSET 								"467 " + nick + " "

// 471: 채널이 가득 차서 더 이상 사용자가 들어올 수 없음
#define ERR_CHANNELISFULL(nick, ch) 						"471 " + nick + " " + ch + " :Cannot join channel (+l)"

// 472: 알 수 없는 모드를 사용함
#define ERR_UNKNOWNMODE(ch)                     "472 " + ch + " :is unknown mode char to me"

// 473: 초대받은 사용자만 들어갈 수 있는 채널임
#define ERR_INVITEONLYCHAN(nick, ch) 						"473 " + nick + " " + ch + " :Cannot join channel (+i)"

// 474: 사용자가 해당 채널에서 차단됨
#define ERR_BANNEDFROMCHAN(nick, ch) 						"474 " + nick + " " + ch + " :Cannot join channel (+b)"

// 475: 잘못된 채널 키를 입력함
#define ERR_BADCHANNELKEY(nick, ch) 						"475 " + nick + " " + ch + " :Cannot join channel (+k)"

// 476: 잘못된 채널 이름 형식을 사용함
#define ERR_BADCHANMASK(nick,ch) 						"476 " + nick + " " + ch + " :Invalid channel mask"

// 477: 채널이 모드를 지원하지 않음
#define ERR_NOCHANMODES 						"477 " + nick + " "

// 478: 채널의 금지 목록이 가득 참
#define ERR_BANLISTFULL 						"478 " + nick + " "

// 481: 사용자가 이 명령을 수행할 권한이 없음
#define ERR_NOPRIVILEGES(user) 					"481 " + user + " :Permission Denied - You're not an operator"

// 482: 채널 운영자 권한이 필요함
#define ERR_CHANOPRIVSNEEDED(nick, ch) 			"482 " + nick + " " + ch + " :You're not channel operator"

// 483: 서버를 종료할 수 없음
#define ERR_CANTKILLSERVER 						"483 " + nick + " "

// 491: 해당 호스트에서 운영자 권한을 사용할 수 없음
#define ERR_NOOPERHOST 							"491 " + nick + " "

// 501: 알 수 없는 사용자 모드 플래그 사용
#define ERR_UMODEUNKNOWNFLAG 					"501 " + nick + " "

// 502: 사용자가 자신이 아닌 다른 사람의 모드를 변경하려 함
#define ERR_nickSDONTMATCH 						"502 " + nick + " "

/* Custom */

// PASS 시 Password 오류
#define ERR_PASS_PASSWORD                       "Error: incorrected PassWord"

#define ERR_INVALIDPARAM                        "Error: invalid parameter"

/* 사용자 정의 */

// 채널에 참여 완료시
#define RPL_JOIN(servername, ch) ":" + servername + " JOIN :" + ch

// 채널에서 강퇴 완료시
#define RPL_KICK(nick, username, hostname, ch, target, comment) nick + "!" + username + "@" + hostname + " KICK " + ch + " " + target + " :" + comment

// 채널에서 나가기 완료시
#define RPL_PART(nick, ch) nick + " PART " + ch

//채널 초대 완료시 초대 당한 사용자에게 보내는 메세지
#define RPL_INVITE(nick, target, ch) ":" + nick + " INVITE " + target + " " + ch

// 417: 메세지가 512자를 넘어가는 경우 (InspIRCd 기준)
#define ERR_LINETOOLONG(nick)					"417 " + nick + " :Input Line too long"


#define ERR_ERRUSERCMD "Error: "

