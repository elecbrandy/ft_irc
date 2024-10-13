#include "Server.hpp"

IrcServer::IrcServer() {}

IrcServer::IrcServer(int port, const std::string& password)
: port(port), password(password) { }

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

	std::cout << "New Client connected : " << client_fd << std::endl;
}

void IrcServer::removeClient(int client_fd) {
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
	while (true) {
		// poll 하는 중 ~
		if (poll(&fds[0], fds.size(), -1) < 0) {
			handleError(ERR_POLL, EXIT);
		}
		// 
		for (int i = fds.size() - 1; i>= 0; --i) {
			if (fds[i].revents & POLLIN) {	// 파일 디스크립터가 poll 상태인지 비트연산해서->revent(short)의 비트랑 비교해서 그 안에 Pollin 있는지 아마 최저 1비트s
				handleSocketEvent(fds[i].fd);
			}
		}
	}
}

void IrcServer::handleSocketEvent(int fd) {
	if (fd == server_fd) {	// 이벤트가 서버 소켓에서 감지되었을 경우에는->?
		acceptClient();		// 클라이언트 추가
	} else {
		handleClientMessage(fd); // 아마 그게 아니면 클라이언트 메세지일거임!s
	}
}

void IrcServer::handleClientMessage(int client_fd) {
	char buffer[BUFFER_SIZE];
	std::memset(buffer, 0, BUFFER_SIZE); // 이거 써도 되나?

	int bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
	// 나중에 이 클라에서 보낸 데이터를 읽어올때 이걸 반복문으로 돌려야하나? 버퍼 이상 보낼수도있으니까?
	
	if (bytes_received < 0) {	 // 클라에서 에러 발생
		if (errno != EWOULDBLOCK && errno != EAGAIN) {
			handleError(ERR_ETC, UNEXIT);
			removeClient(client_fd);
		}
	} else if (bytes_received == 0) { // 클라 정상 종료?
		removeClient(client_fd);
	} else { // 메ㅔ세지 정상 받음
		buffer[bytes_received] = '\0';  // 널 문자추가
		std::cout << "Message from client " << client_fd << ": " << buffer << std::endl;
		broadcastMessage(client_fd, buffer);  // 다른 클라이언트들에게 메시지 쏴주기
	}
}

// 깔끔하게 좀 수정하기 밑에
void IrcServer::broadcastMessage(int sender_fd, const char* message) {
	std::string full_message = "Client ";
	std::stringstream ss;
	ss << sender_fd;
	full_message += ss.str();
	full_message += ": ";
	full_message += message;

	for (size_t i = 0; i < fds.size(); ++i) {
		int client_fd = fds[i].fd;

		// 서버 소켓과 메시지 보낸 클라이언트는 제외
		if (client_fd != server_fd && client_fd != sender_fd) {
			int bytes_sent = send(client_fd, full_message.c_str(), full_message.length(), 0);
			if (bytes_sent < 0) {
				std::cerr << "Error sending message to client " << client_fd << ": " << strerror(errno) << std::endl;
				// 필요에 따라 클라이언트를 제거할 수 있음
				// removeClient(client_fd);
			}
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
