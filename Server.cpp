#include "Server.hpp"
#include "Client.hpp"

IrcServer::IrcServer() {}

IrcServer::IrcServer(int port, const std::string& password)
: _name(SERVER_NAME), port(port), password(password) {}

IrcServer::~IrcServer() {}

std::string IrcServer::getPassword() {return this->password;}

const std::string IrcServer::getName() const {return this->_name;}

time_t IrcServer::getStartTime() const {return this->_startTime;}

std::string IrcServer::formatDateToString(time_t time) {
    struct tm *timeInfo = std::localtime(&time);
    char buffer[11];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeInfo);
    return std::string(buffer);
}

void IrcServer::init() {
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

void IrcServer::removeClient(int client_fd) {
	// new로 클라이언트 할당한 것에 대해서 객체 메모리 해제 추가!
	if (_clients.find(client_fd) != _clients.end()) {
		delete _clients[client_fd];
		_clients.erase(client_fd);
	}

	for (size_t i = 0; i < fds.size(); ++i) {
		if (fds[i].fd == client_fd) {
			fds.erase(fds.begin() + i);
			break;
		}
	}
	close(client_fd);
	std::cout << LOG_END_CONNECTED << C_LOG << client_fd << C_RESET << std::endl;
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
				if (fds[i].revents & POLLIN) {
					handleSocketEvent(fds[i].fd);
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

void IrcServer::handleSocketEvent(int fd) {
	if (fd == _fd) {
		acceptClient();
	} else {
		handleClientMessage(fd);
	}
}

void IrcServer::handleClientMessage(int client_fd) {

	char buffer[BUFFER_SIZE];
	std::memset(buffer, 0, BUFFER_SIZE);

	int bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

	if (bytes_received < 0) {
		// Client에서 에러 발생 시
		if (errno != EWOULDBLOCK && errno != EAGAIN) {
			removeClient(client_fd);
			throw ServerException(ERR_RECV);
		}
	} else if (bytes_received == 0) {
		// 종료된 Client일 경우
		removeClient(client_fd);
	} else {
		buffer[bytes_received] = '\0';

		std::cout << "\n--------------------------------------------------------" << std::endl;
		std::cout << ">>" <<" msg from client " << C_LOG << client_fd << ":\n" << C_RESET << buffer << std::endl;
		Client * client = getClient(client_fd);
		if (client) {
			client->appendToBuffer(buffer);
			std::string message;
			while (client->extractMessage(message)) {
				Cmd cmdHandler(*this, message, client_fd);
				if (!cmdHandler.handleClientCmd()) {
					return ;
				}
			}
		}
	}
}

// castMsg 사용법 : castMsg(client_fd, server.makeMsg(매크로(매크로의 파라미터1, 파라미터2, 파라미터3, ...)).c_str());
void IrcServer::castMsg(int client_fd, const std::string msg) {
	std::cout << "<< msg to client " << C_LOG << client_fd << C_RESET << ":\n" << msg << std::endl;
	ssize_t bytesSent = send(client_fd, msg.c_str(), msg.length(), 0);


	if (bytesSent == -1) {								// 전송 중 오류 예외 처리
		throw ServerException(ERR_SEND);
	}

	// 전송된 바이트 수가 전체 메시지 길이보다 적을 때 처리
	// if (bytesSent < msgLen) {
	//     std::cerr << "Warning: Only partial message sent to client fd " << client_fd << std::endl;
	//     // 필요한 경우 여기서 추가 처리를 할 수 있음
	// }

	//std::cout << "Message sent to client fd " << client_fd << ": " << message << std::endl;
}

// 1. 서버->채널 내 모든 클라이언트 2. 서버->서버 내 모든 클라이언트로 메시지 전송하는 함수
void IrcServer::broadcastMsg(const std::string& message, Channel* channel) {
    if (channel != NULL) { 		// 채널이 지정된 경우, 해당 채널의 모든 참가자에게 메시지 전송
        std::map<std::string, Client*>::iterator it = channel->getParticipant().begin();
        for (; it != channel->getParticipant().end(); ++it) {
            Client* receiver = it->second;
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

const std::map<std::string, Channel*>& IrcServer::getChannels() const
{
	return (this->_channels);
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

	while (it != _clients.end()) {
		if (it->second->isConnectionTimedOut(TIME_OUT)) {
			std::cout << "client[" << C_LOG << it->first << C_RESET << "] connection timed out." << std::endl;
			int tmp = it->first;
			it = _clients.erase(it);
			removeClient(tmp);
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
		//초대된 사용자 목록도 삭제?
	}
	if (_clients.find(client->getFd()) == _clients.end())
		_clients.erase(client->getFd());
	if (nickNameClientMap.find(client->getNickname()) == nickNameClientMap.end())
		nickNameClientMap.erase(client->getNickname());
	delete(client);
}

const char* IrcServer::ServerException::what() const throw() {
	return msg.c_str();
}
