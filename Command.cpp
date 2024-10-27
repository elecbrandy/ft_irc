// #include "Server.hpp"
// #include "Client.hpp"
// #include "Channel.hpp"

// void IrcServer::cmdUser(std::string &cmdParams, int client_fd) {
// 	std::stringstream ss(cmdParams);
// 	std::string names[4];
// 	Client* client = getClient(client_fd);

// 	for (int i = 0; i < 4; i++) {
// 		if (cmdParams.empty())
// 			castMsg(client_fd, makeMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "USER")).c_str());
// 		ss >> names[i];
// 		if (i == 3 && !ss.eof()) {
// 			while (!ss.eof()) {
// 				std::string tmp;
// 				ss >> tmp;
// 				names[i] += " " + tmp;
// 			}
// 		}
// 	}

// 	// 정상적으로 4개의 필드가 들어왔을 때 클라이언트 정보를 설정
//     client->setUsername(names[0]);
//     client->setHostname(names[1]);
//     client->setServername(names[2]);
//     client->setRealname(names[3]); 

// 	// std::cout << "client->getUsername() : " << client->getUsername() << std::endl;
// 	// std::cout << "client->getHostname() : " << client->getHostname() << std::endl;
// 	// std::cout << "client->getServername() : " << client->getServername() << std::endl;
// 	// std::cout << "client->getRealname() : " << client->getRealname() << std::endl;

// 	// 클라이언트에게 웰컴 메시지 전송
//     castMsg(client_fd, makeMsg(RPL_WELCOME(client->getUsername())).c_str());
// }

// void IrcServer::cmdNick(std::string &cmdParams, int client_fd) {
// 	Client* client = getClient(client_fd);
// 	std::string nickname = cmdParams;
// 	client->setNickname(nickname);
// 	// std::cout << "client->getNickname() : " << client->getNickname() << std::endl;
// }

// void IrcServer::cmdPass(std::string &cmdParams, int client_fd) {
// 	Client* client = getClient(client_fd);
//     std::string password = cmdParams;
//     client->setPassword(password);
//     // std::cout << "client->getPassword() : " << client->getPassword() << std::endl;
// }

// void IrcServer::cmdPing(std::string &cmdParams, int client_fd) {
//     Client* client = getClient(client_fd);
//     client->setLastPongTime(); //마지막 pong 시간 갱신을 하는 이유 : 1.고스트커넥션 방지 2.서버가 살아있는지 확인 3.서버의 리소스 관리를 위해
// 	std::string servername = client->getServername();
// 	if (cmdParams == servername)
// 		castMsg(client_fd, makeMsg("PONG" + ' ' + servername).c_str());
// }

// std::vector<std::string> IrcServer::joinSplit(std::string &cmdParams) {
// 	std::vector<std::string> res;
//     std::string tmp;

//     for (size_t i = 0; i < cmdParams.size(); i++) {
//         char c = cmdParams[i];
//         if (c == ',' || c == ' ') {
//             if (!tmp.empty()) {
//                 res.push_back(tmp);
//                 tmp.clear();
//             }
// 		} else
//             tmp += c;
//     }

//     if (!tmp.empty())
//         res.push_back(tmp);

//     return res;
// }

// void IrcServer::cmdJoin(std::string &cmdParams, int client_fd) {
// 	//Client* client = getClient(client_fd);
// 	(void)client_fd;
// 	std::string chName;
// 	std::string chKey;

// 	std::vector<std::string> joinTokens = joinSplit(cmdParams);
// 	std::vector<std::string> channel;
// 	std::vector<std::string> key;

// 	for (size_t i = 0; i < joinTokens.size(); i++) {
// 		if (joinTokens[i][0] == '#')
// 			channel.push_back(joinTokens[i]);
// 		else
// 			key.push_back(joinTokens[i]);
// 	}

// 	for (size_t i = 0; i < channel.size(); i++) {
// 		chName = channel[i];	
// 		bool isOperator = false;

// 		if (i < key.size())
// 			chKey = key[i];

// 		if (_channels.find(channel[i]) == _channels.end()) {
// 			isOperator = true;
// 			_channels[chName] = new Channel(chName);
// 			if (!chKey.empty()) {
// 				_channels[chName]->setMode(KEY_MODE);
// 				_channels[chName]->setKey(chKey);
// 			}
// 		}
// 	}

// 	std::string participantName = "";
// 	std::set<Client*>::iterator it = _participant.begin();

// 	for (it = _participant.begin(); it != _participant.end(); it++) {
// 		Client* participant = *it;
// 		if (isOperator == true)
// 			participantName = "@" + participant->getNickname();
// 	}

// }