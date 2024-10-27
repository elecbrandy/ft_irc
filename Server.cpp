#include "Server.hpp"
#include "Client.hpp"

IrcServer::IrcServer() {}

IrcServer::IrcServer(int port, const std::string& password)
: port(port), password(password) {}

IrcServer::~IrcServer() {}

void IrcServer::init() {
	if ((server_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		handleError(ERR_SOCKET_CREATION, EXIT);
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		handleError(ERR_SOCKET_BIND, EXIT);
	}

	if (listen(server_fd, MAX_CLIENTS) == -1) {
		handleError(ERR_SOCKET_LISTEN, EXIT);
	}

	if (fcntl(server_fd, F_SETFL, O_NONBLOCK) == -1) {
		handleError(ERR_SET_NONBLOCKING, EXIT);
	}

	struct pollfd server_poll_fd;
	server_poll_fd.fd = server_fd;
	server_poll_fd.events = POLLIN;
	fds.push_back(server_poll_fd);

	std::cout << "Server initialized on port : " << port << std::endl;
}

void IrcServer::acceptClient() {
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
	if (client_fd == -1) {
		handleError(ERR_ACCEPT_CLIENT, UNEXIT);
		return;
	}

	if (fds.size() - 1 >= MAX_CLIENTS) {
		handleError(ERR_ACCEPT_CLIENT, UNEXIT);
		close(client_fd);
	}

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
		handleError(ERR_CLIENT_NONBLOCKING, UNEXIT);
		close(client_fd);
		return;
	}

	struct pollfd client_poll_fd;
	client_poll_fd.fd = client_fd;
	client_poll_fd.events = POLLIN;
	fds.push_back(client_poll_fd);

	Client* newClient = new Client();
	_clients[client_fd] = newClient;

	std::cout << "New Client connected, client_fd is " << client_fd << std::endl;
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
	std::cout << "Client connection end : " << client_fd << std::endl;
}

void IrcServer::run() {
	time_t lastCheckTime = time(NULL);

	while (true) {
		time_t now = time(NULL);

		if (now - lastCheckTime >= PING_INTERVAL) {
			checkConnections();
			lastCheckTime = now;
		}

		if (poll(&fds[0], fds.size(), -1) < 0) {
			handleError(ERR_POLL, EXIT);
		}

		for (int i = fds.size() - 1; i>= 0; --i) {
			if (fds[i].revents & POLLIN) {	
				handleSocketEvent(fds[i].fd);
			}
		}
	}
}

void IrcServer::handleSocketEvent(int fd) {
	if (fd == server_fd) {
		acceptClient();
	} else {
		handleClientMessage(fd);
	}
}

void IrcServer::handleClientMessage(int client_fd) {
	char buffer[BUFFER_SIZE];
	std::memset(buffer, 0, BUFFER_SIZE);

	int bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

	if (bytes_received < 0) {							// Client에서 에러 발생 시
		if (errno != EWOULDBLOCK && errno != EAGAIN) {
			handleError(ERR_ETC, UNEXIT);
			removeClient(client_fd);
		}
	} else if (bytes_received == 0) {					// 종료된 Client일 경우
		removeClient(client_fd);
	} else {											// 메세지 정상 받음
		buffer[bytes_received] = '\0';
		std::cout << "\n\n--------------------------------" << std::endl;
		std::cout << "Message from client " << client_fd << ": " << buffer << std::endl;

		this->_msgBuf += buffer;

		size_t pos;
		while ((pos = _msgBuf.find("\r\n")) != std::string::npos) { 
			handleClientCmd(client_fd);					// 클라이언트 요청 처리
			_msgBuf.erase(0, pos + 2);					// 처리한 명령어 삭제
		}
	}
}

void IrcServer::castMsg(int client_fd, const char* message) {
	size_t msgLen = std::strlen(message);				// 메시지 길이를 계산
	ssize_t bytesSent = send(client_fd, message, msgLen, 0);

	if (bytesSent == -1) {								// 전송 중 오류 예외 처리
		std::cerr << "Error: Failed to send message to client fd " << client_fd << std::endl;
		throw std::runtime_error("Failed to send message");
	}

	// 전송된 바이트 수가 전체 메시지 길이보다 적을 때 처리
	// if (bytesSent < msgLen) {
	//     std::cerr << "Warning: Only partial message sent to client fd " << client_fd << std::endl;
	//     // 필요한 경우 여기서 추가 처리를 할 수 있음
	// }

	//std::cout << "Message sent to client fd " << client_fd << ": " << message << std::endl;
}

std::string IrcServer::extractCmd() {
	std::string cmd;
	std::stringstream ss(_msgBuf);

	ss >> cmd;  // 첫 번째 단어 추출

	return cmd;
}

std::string IrcServer::extractCmdParams(size_t cmdSize) {
	std::string cmdParams;
	size_t pos = this->_msgBuf.find(CRLF);

	if (pos != std::string::npos)
		cmdParams = this->_msgBuf.substr(cmdSize + 1, pos - (cmdSize + 1));
	
	return cmdParams;
}

void IrcServer::handleClientCmd(int client_fd) {
	// client에서 보낸 메세지를 파싱해서 명령어를 추출
	// 추출한 명령어 실행 후 실행 결과를 클라이언트에게 전송
	Client* client = getClient(client_fd);
	std::string cmd = extractCmd(); // 클라이언트가 보낸 메세지에서 명령어 추출
	std::string cmdParams = extractCmdParams(cmd.size()); // 클라이언트가 보낸 메세지에서 명령어 파라미터 추출
	std::cout << "---------------------------------" << std::endl;
	std::cout << "Command : " << cmd << std::endl;
	std::cout << "Command Params : " << cmdParams << std::endl;
	if (cmd == "PASS")
		cmdPass(cmdParams, client_fd);
	if (cmd == "NICK")
		cmdNick(cmdParams, client_fd);
	else if (cmd == "USER")
		cmdUser(cmdParams, client_fd);
	else if (cmd == "PING")
		cmdPing(cmdParams, client_fd);
	else if (cmd == "JOIN")
		cmdJoin(cmdParams, client_fd);
	// else if (cmd == "PART")
	// 	cmdMode(client_fd, clientMsg);
	// else if (cmd == "PRIVMSG")
	// 	cmdPrivmsg(client_fd, clientMsg);
	// else if (cmd == "KICK")
	// 	cmdKick(client_fd, clientMsg);
	// else if (cmd == "INVITE")
	// 	cmdInvite(client_fd, clientMsg);
	// else if (cmd == "MODE")
	// 	cmdPart(client_fd, clientMsg);
	// else if (cmd == "TOPIC")
	// 	cmdTopic(client_fd, clientMsg);
	else
		castMsg(client_fd, makeMsg(ERR_UNKNOWNCOMMAND(client->getNickname())).c_str());
}

Client* IrcServer::getClient(int client_fd) {
	if (_clients.find(client_fd) == _clients.end())
		return nullptr;

	return (_clients[client_fd]);
}

void IrcServer::checkConnections() {
	std::cout << "Checking connections..." << std::endl;
	std::map<int, Client*>::iterator it = _clients.begin();
	
	while (it != _clients.end()) {
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																													
		if (it->second->isConnectionTimedOut(180)) {
			std::cout << "Client " << it->first << " connection timed out." << std::endl;
			int clientId = it->first;  // 현재 클라이언트 ID 저장
			it = _clients.erase(it);   // 안전하게 현재 요소 제거하고 다음 반복자 받기
			removeClient(clientId);     // 추가적인 정리 작업 수행
		} else {
			++it;  // 타임아웃이 아닌 경우에만 반복자 증가
		}
	}
}



// 나중에 영어로~
void IrcServer::handleError(ErrorCode code, int flag) {
	switch (code) {
		case ERR_SOCKET_CREATION:
			std::cerr << "소켓 생성 에러: " << strerror(errno) << std::endl;
			break;
		case ERR_SOCKET_OPTIONS:
			std::cerr << "소켓 옵션 설정 에러: " << strerror(errno) << std::endl;
			break;
		case ERR_SOCKET_BIND:
			std::cerr << "소켓 바인딩 에러: " << strerror(errno) << std::endl;
			break;
		case ERR_SOCKET_LISTEN:  
			std::cerr << "소켓 리슨 에러: " << strerror(errno) << std::endl;
			break;
		case ERR_SET_NONBLOCKING:
			std::cerr << "논블로킹 모드 설정 에러: " << strerror(errno) << std::endl;
			break;
		case ERR_ACCEPT_CLIENT:
			std::cerr << "클라이언트 수락 에러: " << strerror(errno) << std::endl;
			break;
		case ERR_CLIENT_NONBLOCKING:
			std::cerr << "클라이언트 소켓 논블로킹 설정 에러: " << strerror(errno) << std::endl;
			break;
		case ERR_DATA_RECEIVE:
			std::cerr << "데이터 수신 에러: " << strerror(errno) << std::endl;
			break;
		case ERR_MESSAGE_SEND:
			std::cerr << "메시지 전송 에러: " << strerror(errno) << std::endl;
			break;
		case ERR_POLL:
			std::cerr << "poll() 호출 에러: " << strerror(errno) << std::endl;
			break;
		default:
			std::cerr << "알 수 없는 에러가 발생했습니다." << std::endl;
			break;
	}
	if (flag == EXIT) {
		exit(EXIT_FAILURE);
	}
}