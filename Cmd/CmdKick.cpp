#include "Cmd.hpp"

/*
명령어: KICK <channel> <nickname> [<comment>]

Numeric Replies:
ERR_NEEDMOREPARAMS - 파라미터 부족
ERR_NOSUCHCHANNEL - 채널이 채널 목록에 존재하지 않음
ERR_BADCHANMASK - 주어진 채널이름의 형식이 잘못됨. (채널이름이 규칙에 맞지 않는 경우, #으로 시작하지 않는 등..)
ERR_CHANOPRIVSNEEDED - 채널 오퍼레이터 권한이 없음
ERR_NOTONCHANNEL - 강퇴시킬 사용자가 채널에 참여자가 아님

- 채널에서 사용자를 강제로 퇴장시킬 때 사용하는 명령어
- <channel>은 사용자를 강제로 퇴장시킬 채널 이름
- <nickname>은 강제로 퇴장시킬 사용자의 닉네임
- <comment>는 선택사항으로, 사용자를 강제로 퇴장시킬 때 이유를 명시
*/