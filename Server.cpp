#include "Server.hpp"
#include "Client.hpp"

IrcServer::IrcServer() {}

IrcServer::IrcServer(const char *port, const char *password)
: _servername(SERVER_NAME), _startTime(time(NULL)) {
	/* Check Port number */
	if (port == NULL || *port == '\0') {
		throw ServerException(ERR_PORT_NULL);
	}

	for (int i = 0; port[i] != '\0'; ++i) {
		if (!std::isdigit(static_cast<unsigned char>(port[i]))) {
			throw ServerException(ERR_PORT_DIGIT);
		}
	}

	int tmpPort = std::atoi(port);
	if (tmpPort < PORT_MIN || tmpPort > PORT_MAX || std::strlen(port) > PORT_MAX_LEN) {
		throw ServerException(ERR_PORT_RANGE);
	}
	this->_port = tmpPort;

	/* Check Password */
	if (password == NULL || *password == '\0') {
		throw ServerException(ERR_PASSWORD_NULL);
	}

	for (int i = 0; password[i] != '\0'; ++i) {
		if (!std::isalnum(static_cast<unsigned char>(password[i]))) {
			throw ServerException(ERR_PASSWORD_ALNUM);
		}
	}

	if (std::strlen(password) > PASSWORD_MAX_LEN) {
		throw ServerException(ERR_PASSWORD_SIZE);
	}
	this->_password = std::string(password);
}

IrcServer::~IrcServer() {
	/* Close server socket */
	close(this->_fd);

	/* Close & Delete Client resource */
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->first != -1) {
			close(it->first);
		}
		delete it->second;
	}
	_clients.clear();
	fds.clear();
	nickNameClientMap.clear();
}

/* getter */

std::string IrcServer::getPassword() {return this->_password;}

const std::string IrcServer::getName() const {return this->_servername;}

time_t IrcServer::getStartTime() const {return this->_startTime;}

std::string IrcServer::formatDateToString(time_t time) {
	struct tm *timeInfo = std::localtime(&time);
	char buffer[20];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);
	return std::string(buffer);
}

void IrcServer::init() {
	if ((_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		throw ServerException(ERR_SOCKET_CREATION);
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(_port);

	if (bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		throw ServerException(ERR_SOCKET_BIND);
	}

	if (listen(_fd, MAX_CLIENTS) == -1) {
		throw ServerException(ERR_SOCKET_LISTEN);
	}

	if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1) {
		throw ServerException(ERR_SET_NONBLOCKING);
	}

	struct pollfd server_poll_fd;
	server_poll_fd.fd = _fd;
	server_poll_fd.events = POLLIN;
	fds.push_back(server_poll_fd);

	std::system("clear");
	printGoat(); // goat
	serverLog(this->_fd, LOG_SERVER, C_MSG, MSG_SERVER_INIT(intToString(this->_port)));
}

void IrcServer::acceptClient() {
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_fd = accept(_fd, (struct sockaddr*)&client_addr, &client_len);
	if (client_fd == -1) {
		throw ServerException(ERR_SOCKET_CREATION);
	}

	if (fds.size() - 1 >= MAX_CLIENTS) {
		close(client_fd);
		throw ServerException(ERR_ACCEPT_CLIENT);
	}

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
		close(client_fd);
		throw ServerException(ERR_CLIENT_NONBLOCKING);
	}

	struct pollfd client_poll_fd;
	client_poll_fd.fd = client_fd;
	client_poll_fd.events = POLLIN;
	fds.push_back(client_poll_fd);

	Client* newClient = new Client(client_addr.sin_addr);
	newClient->setFd(client_fd);
	_clients[client_fd] = newClient;
	serverLog(this->_fd, LOG_SERVER, C_MSG, MSG_NEW_CONNECTED(intToString(client_fd)));
}

void IrcServer::checkPingTimeOut() {
	std::vector<int> clientsToRemove;
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		Client* client = it->second;
		time_t now = time(NULL);
		if (now - client->getLastPingSent() > WAIT_FOR_PING_MAX)
			clientsToRemove.push_back(client->getFd());
	}

	for (std::vector<int>::iterator it = clientsToRemove.begin(); it != clientsToRemove.end(); it++) {
    	serverLog(this->_fd, LOG_SERVER, C_MSG, MSG_CONNECTION_TIMEOUT(intToString(*it)));
		removeClientFromServer(_clients[*it]);
	}
}

void IrcServer::run() {
	while (true) {
		bool exitFlag= false;
		try {
			// poll 호출에 타임아웃 설정 (예: 1초)
			int timeout_ms = 1000; // 1000ms = 1초
			if (poll(&fds[0], fds.size(), timeout_ms) < 0) {
				if (errno != EINTR) {
					exitFlag = true;
				}
				throw ServerException(ERR_POLL);
			}

			// poll 이벤트 처리
			for (int i = fds.size() - 1; i >= 0; --i) {
				if (fds[i].revents & POLLIN) {
					if (fds[i].fd == this->_fd) {
						acceptClient();
					} else {
						handleSocketRead(fds[i].fd);
					}
				}
			}

			// 타임아웃 또는 입력 이벤트 후 퐁 검사
			checkPingTimeOut();

		} catch (const ServerException& e) {
			serverLog(this->_fd, LOG_ERR, C_ERR, e.what());
			if (exitFlag) {
				exit(EXIT_FAILURE);
			}
		}
	}
}

void IrcServer::handleSocketRead(int fd) {
	char	buffer[2000];

	size_t recvLen = recv(fd, buffer, BUFFER_SIZE - 1, 0);
	if (recvLen <= 0) {
		return ;
	} else {
		Client *tmp = getClient(fd);
		buffer[recvLen] = '\0';
		tmp->appendToRecvBuffer(buffer);
		std::string str;
		while (getClient(fd) && tmp->extractMessage(str)) {
			Cmd cmdHandler(*this, str, fd);
			serverLog(fd, LOG_INPUT, C_MSG, str);
			if (!cmdHandler.handleClientCmd())
				return ;
		}
	}
}

void IrcServer::castMsg(int client_fd, const std::string msg) {
	Client* client = getClient(client_fd);
	if (!client) {
		throw ServerException(ERR_CLIENT_FD);
	}

	ssize_t bytesSent = send(client_fd, msg.c_str(), msg.length(), 0);

	// 송신 오류
	if (bytesSent == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			// 소켓의 송신 버퍼가 가득 차있는 경우, send 해야할 데이터를 Client의 _sendBuffer에 임시저장
			client->appendToSendBuffer(msg);

		} else if (errno == EPIPE || errno == ECONNRESET || errno == ENOTCONN) {
			// 다른 에러 발생 시 클라이언트 리소스 정리 / 제거 
			removeClientFromServer(client);
		}
	// 송신해야하는 데이터 중 일부만 송신한 경우
	} else if (bytesSent < static_cast<ssize_t>(msg.length())) {
		// 이 경우 또한 임시 저장한 테이터를 언젠가 소켓 버퍼에 써야하므로 남은 데이터를 Client의 _sendbuffer에 임시저장
		std::string tmp = msg.substr(bytesSent);
		client->appendToSendBuffer(tmp);

	}
	serverLog(client_fd, LOG_OUTPUT, C_MSG, msg.substr(0, msg.length()));
}

void IrcServer::broadcastMsg(const std::string& message, Channel* channel, int senderFd) {
	if (channel != NULL) { 		// 채널이 지정된 경우, 해당 채널의 모든 참가자에게 메시지 전송
		std::map<std::string, Client*>::iterator it = channel->getParticipant().begin();
		for (; it != channel->getParticipant().end(); ++it) {
			Client* receiver = it->second;
			if (senderFd != -1 && receiver->getFd() == senderFd)
				continue;
			castMsg(receiver->getFd(), message);
		}
	} else {		// 채널이 지정되지 않은 경우, 서버에 접속한 모든 클라이언트에게 메시지 전송
		std::map<int, Client*>::iterator it = this->_clients.begin();
		for (; it != _clients.end(); ++it) {
			int receiverFd = it->first;
			castMsg(receiverFd, message);
		}
	}
}

Client* IrcServer::getClient(int client_fd) {
	if (_clients.find(client_fd) == _clients.end())
		return NULL;

	return (_clients[client_fd]);
}

Client* IrcServer::getClient(const std::string& nickname) {
	for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second->getNickname() == nickname) {
			return it->second;
		}
	}
	return NULL;
}

std::map<std::string, Channel*>& IrcServer::getChannels()
{
	return (this->_channels);
}

void IrcServer::removeChannel(const std::string channelName) {
	std::map<std::string, Channel*>::iterator it = _channels.find(channelName);
	if (it != _channels.end()) {
		delete it->second;  // 채널 객체 먼저 할당 해제
		_channels.erase(it);  // 채널 삭제 (참조 못하게 아예 채널 목록에서 삭제)
	}
}

const std::map<std::string, Client*>& IrcServer::getNickNameClientMap() const
{
	return (this->nickNameClientMap);
}

void IrcServer::addClientByNickname(const std::string& nickname, Client* client)
{
	this->nickNameClientMap[nickname] = client;
}

void IrcServer::setChannels(const std::string& channelName, const std::string& key, const char& mode) {
	_channels[channelName] = new Channel(channelName);
	if (key != "")
		_channels[channelName]->setKey(key);
	if (mode != '\0')
		_channels[channelName]->setMode(mode);
}

void IrcServer::printGoat() {
	std::ifstream goatFile(PATH_GOAT);
	if (goatFile.is_open()) {
		std::string line;
		while (getline(goatFile, line)) {
			std::cout << line << std::endl;
		}
		goatFile.close();
	} else {
		serverLog(this->_fd, LOG_ERR, C_ERR, ERR_OPEN_FILE);
	}
}

void IrcServer::removeClientFromServer(Client* client) {
    if (client == NULL) {
        return;
    }

    // 클라이언트를 채널에서 제거
    for (std::map<std::string, Channel*>::iterator chs = _channels.begin(); chs != _channels.end(); ++chs) {
        Channel* ch = chs->second;
        if (ch->isParticipant(client->getNickname())) {
            ch->removeParticipant(client->getNickname());
        }
        if (ch->isOperator(client->getNickname())) {
            ch->removeOperator(client->getNickname());
        }
    }

    // 클라이언트를 _clients와 nickNameClientMap에서 제거
    if (_clients.find(client->getFd()) != _clients.end()) {
        _clients.erase(client->getFd());
    }
    nickNameClientMap.erase(client->getNickname());

    // fds 벡터에서 클라이언트를 안전하게 제거
    for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end();) {
        if (it->fd == client->getFd()) {
            it = fds.erase(it); // 반복자를 갱신
        } else {
            ++it;
        }
    }

    // // 클라이언트 메모리 해제
	// serverLog(this->_fd, LOG_SERVER, C_MSG, MSG_CONNECTION_TIMEOUT(intToString(*it)));
	serverLog (client->getFd(), LOG_SERVER, C_MSG, MSG_END_CONNECTED(intToString(client->getFd())));
    delete client;
}


const char* IrcServer::ServerException::what() const throw() {
	return msg.c_str();
}

std::string IrcServer::makeMsg(const std::string& prefix, const std::string& msg) {
	return (prefix + " " + msg + CRLF);
}

void IrcServer::updateClients(Client* client) {
	_clients.erase(client->getFd());
	_clients[client->getFd()] = client;
}

void IrcServer::updateNickNameClientMap(const std::string& oldNick, const std::string& newNick, Client* client) {
	nickNameClientMap.erase(oldNick);
	nickNameClientMap[newNick] = client;
}

void IrcServer::serverLog(int fd, int log_type, std::string log_color, std::string msg) {
	if (msg.empty() || msg.back() != '\n') {
		msg += '\n';
	}

	if (log_type == LOG_OUTPUT) {
		std::cout << "OUTPUT[" << C_KEY << fd << C_RESET << "]: " << log_color << msg << C_RESET;
	} else if (log_type == LOG_INPUT) {
		std::cout << "INPUT[" << C_KEY << fd << C_RESET << "]: " << log_color << msg << C_RESET;
	} else if (log_type == LOG_SERVER) {
		std::cout << "SERV_LOG: " << log_color << msg << C_RESET;
	} else if (log_type == LOG_ERR) {
		std::cerr << "SERV_LOG: " << log_color << msg << C_RESET;
	}
}

std::string IrcServer::intToString(int num) {
	std::stringstream ss;
	ss << num;
	return ss.str();
}