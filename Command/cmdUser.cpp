#include "Server.hpp"

std::string cmdUser(Client &client, std::stringstream &buf) {
	std::string names[4];
	std::string tmp;
	int cnt = 0;

	while (cnt < 4) {
		if (!(buf >> tmp))
			return ERR_NEEDMOREPARAMS(client.getNickname(), "USER");
		names[cnt] = tmp;
		cnt++;
	}

	client.setUsername(names[0]);
	client.setHostname(names[1]);
	client.setServername(names[2]);

	if (names[3][0] == ':') { //realname이 ':'로 시작하는지 확인
		names[3].erase(0, 1); //인덱스 0의 1개 문자(:) 삭제
		while (buf >> tmp)
			names[3] += " " + tmp;
	}

	client.setRealname(names[3]);

	return (RPL_WELCOME(client.getNickname()));
}