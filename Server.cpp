#include "Server.hpp"
#include "Client.hpp"

IrcServer::IrcServer() {}

IrcServer::IrcServer(int port, const std::string& password)
:	_servername(SERVER_NAME),
	port(port),
	password(password),
	_startTime(time(NULL)) {}

IrcServer::~IrcServer() {
	/* Close server socket */
	if (_fd != -1) {
		close(_fd);
	}

	/* Close & Delete Client resource */
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {

		if (it->first != -1) {
			close(it->first);
		}

		delete it->second;
	}
	_clients.clear();

	/* Close poll struct */
	fds.clear();

	/* Another Clean up */
	nickNameClientMap.clear();

	// std::cout << LOG_SERVER_SHUTDOWN << std::endl;
}

/* getter */

std::string IrcServer::getPassword() {return this->password;}

const std::string IrcServer::getName() const {return this->_servername;}

time_t IrcServer::getStartTime() const {return this->_startTime;}

std::string IrcServer::formatDateToString(time_t time) {
    struct tm *timeInfo = std::localtime(&time);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);
    return std::string(buffer);
}

void IrcServer::init() {
	// ScopedTimer("init");
	if ((_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		throw ServerException(ERR_SOCKET_CREATION);
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

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
	std::cout << LOG_SERVER_INIT << C_LOG << port << C_RESET << std::endl;
}

void IrcServer::acceptClient() {
	// ScopedTimer("acceptClient");
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

	std::cout << LOG_NEW_CONNECTED << C_LOG << client_fd << C_RESET << std::endl;
}

void IrcServer::run() {
	time_t lastCheckTime = time(NULL);

	while (true) {
		bool exitFlag= false;
		try {
			time_t currentTime = time(NULL);
			if (currentTime - lastCheckTime >= TIME_CHECK_INTERVAL) {
				checkConnections();
				lastCheckTime = currentTime;
			}

			if (poll(&fds[0], fds.size(), -1) < 0) {
				if (errno != EINTR) {
					exitFlag = true;
				}
				throw ServerException(ERR_POLL);
			}

			for (int i = fds.size() - 1; i>= 0; --i) {
				// 해당 소켓에 읽기 플래그가 설정되어 있는 경우
				if (fds[i].revents & POLLIN) {
					handleSocketRead(fds[i].fd);
				}

				// 해당 소켓에 쓰기 플래그가 설정되어 있는 경우
				if (fds[i].revents & POLLOUT) {
					handleSocketWrite(fds[i].fd);
				}

				// 해당 소켓에 오류 플래그가 설정되어 있는 경우
				if (fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
					removeClinetFromServer(getClient(fds[i].fd));
				}
			}
		} catch (const ServerException& e) {
			std::cerr << C_ERR << "Error: server.run(): " << e.what() << C_RESET << std::endl;
			if (exitFlag) {
				exit(EXIT_FAILURE);
			}
		}
	}
}

void IrcServer::handleSocketRead(int fd) {
	if (fd == this->_fd) {
		acceptClient();
	} else {
		handleClientMessage(fd);
	}
}

void IrcServer::handleClientMessage(int client_fd) {
	Client * client = getClient(client_fd);
	if (client) {
		client->setlastActivityTime();
		char buffer[BUFFER_SIZE];
		int bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

		if (bytes_received < 0) {			// Client에서 에러 발생 시
			if (errno == EWOULDBLOCK && errno == EAGAIN) {
				return ;
			} else {
				removeClinetFromServer(client);
				throw ServerException(ERR_RECV);
			}
		} else if (bytes_received == 0) {
			removeClinetFromServer(client);
		} else {
			buffer[bytes_received] = '\0';
			client->appendToRecvBuffer(buffer);
			std::string tmp;
			while (client->extractMessage(tmp)) {
				Cmd cmdHandler(*this, tmp, client_fd);
				std::cout << " INPUT[" << C_LOG << client_fd << C_RESET << "]: " << tmp << std::endl;
				if (!cmdHandler.handleClientCmd()) {
					return ;
				}
			}
		}
	}
}

void IrcServer::castMsg(int client_fd, const std::string msg) {
	Client* client = getClient(client_fd);
	if (!client) {
		throw ServerException(ERR_SEND);
	}

	ssize_t bytesSent = send(client_fd, msg.c_str(), msg.length(), 0);

	// 송신 오류
	if (bytesSent == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			// 소켓의 송신 버퍼가 가득 차있는 경우, send 해야할 데이터를 Client의 _sendBuffer에 임시저장
			client->appendToSendBuffer(msg);

			// POLLOUT (소켓에 써야하는 플래그) 를 감지하도록 설정. 임시저장한 데이터를 언젠가 소켓 버퍼에 써야하므로
			modifyPollEvent(client_fd, POLLIN | POLLOUT);
		} else if (errno == EPIPE || errno == ECONNRESET || errno == ENOTCONN) {
			// 다른 에러 발생 시 클라이언트 리소스 정리 / 제거 
			removeClinetFromServer(getClient(client_fd));
		}

	// 송신해야하는 데이터 중 일부만 송신한 경우
	} else if (bytesSent < static_cast<ssize_t>(msg.length())) {
		// 이 경우 또한 임시 저장한 테이터를 언젠가 소켓 버퍼에 써야하므로!
		// 남은 데이터를 Client의 _sendbuffer에 임시저장
		std::string tmp = msg.substr(bytesSent);
		client->appendToSendBuffer(tmp);

		// 이어서 POLLOUT 이벤트를 모니터링 하도록 설정
		modifyPollEvent(client_fd, POLLIN | POLLOUT);
	}
	std::cout << "OUTPUT[" << C_LOG << client_fd << C_RESET << "]: " << msg.substr(0, msg.length());
}

void IrcServer::modifyPollEvent(int fd, short events) {
	// ScopedTimer("modifyPollEvent");
	for (size_t i = 0; i < fds.size(); ++i) {
		if (fds[i].fd == fd) {
			fds[i].events = events;
			break;
		}
	}
}

void IrcServer::handleSocketWrite(int client_fd) {
	// ScopedTimer("handleScketWritable");
	Client* client = getClient(client_fd);

	if (client && client->hasDataToSend()) {
		const std::string& buffer = client->getSendBuffer();
		ssize_t bytesSent = send(client_fd, buffer.c_str(), buffer.size(), 0);

		if (bytesSent == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				// 송신 버퍼가 아직 가득 찬 경우, 다음 POLLOUT 이벤트를 기다림
				return ;
			} else {
				// 심각한 에러 발생 시 클라이언트 제거
				removeClinetFromServer(getClient(client_fd));
			}
		} else {
			client->clearSendBuffer(bytesSent);
			if (!client->hasDataToSend()) {
				// 모든 데이터가 전송된 경우. 즉 이 시점에서 _sendBuffer가 비어있는경우 POLLOUT 모니터링 해제
				modifyPollEvent(client_fd, POLLIN);
			}
		}
	} else {
		// 보낼 데이터가 없으므로 POLLOUT 모니터링 해제
		modifyPollEvent(client_fd, POLLIN);
	}
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
	// ScopedTimer("getClient");
	for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second->getNickname() == nickname) {
			return it->second;
		}
	}
	return NULL;
}

const std::map<std::string, Channel*>& IrcServer::getChannels() const
{
	return (this->_channels);
}

void IrcServer::removeChannel(const std::string channelName) {
	if (_channels.find(channelName) != _channels.end()) {
		delete _channels[channelName];
		_channels.erase(channelName);
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
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																									
void IrcServer::checkConnections() {
	std::map<int, Client*>::iterator it = _clients.begin();
	std::cout << GREEN << LOG_CHECK_CONNECTION_START << C_RESET << std::endl;
	while (it != _clients.end()) {
		if (it->second->isConnectionTimedOut(TIME_OUT)) {
			std::cout << LOG_CONNECTION_TIMEOUT << C_LOG << it->first << C_RESET << std::endl;
			removeClinetFromServer(it->second); // Client 제거
			it = _clients.erase(it);            // 지운 후 다음 iterator 반환
		} else {
			++it;
		}
	}
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
		std::cerr << C_ERR << "Unable to open GOAT art file: " << C_RESET << PATH_GOAT << std::endl;
	}
}

void IrcServer::removeClinetFromServer(Client* client)
{
	for(std::map<std::string, Channel*>::iterator chs = _channels.begin(); chs != _channels.end(); chs++)
	{
		Channel* ch = chs->second;
		if (ch->isOperator(client->getNickname()) == true)
			ch->removeOperator(client);
		if (ch->isParticipant(ch->isOperatorNickname(client->getNickname())))
			ch->removeParticipant(client->getNickname());
		// if (ch->isInvited(client->getNickname()) == true)
			
		//초대된 사용자 목록도 삭제?
	}
	if (_clients.find(client->getFd()) != _clients.end())
		_clients.erase(client->getFd());
	if (nickNameClientMap.find(client->getNickname()) != nickNameClientMap.end())
		nickNameClientMap.erase(client->getNickname());
	// poll fd close
	for (size_t i = 0; i < fds.size(); ++i) {
		if (fds[i].fd == client->getFd()) {
			fds.erase(fds.begin() + i);
			break;
		}
	}
	delete(client);
}

const char* IrcServer::ServerException::what() const throw() {
	return msg.c_str();
}

std::string IrcServer::makeMsg(const std::string& prefix, const std::string& msg) {
	if (prefix.empty()) {
		return (msg + CRLF);
	}
	return (prefix + " " + msg + CRLF);
}
