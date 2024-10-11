#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>      // atoi 사용
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>      // fcntl 사용
#include <sys/select.h> // select 사용
#include <errno.h>

#define BUFFER_SIZE 1024

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "사용법: " << argv[0] << " <서버 IP> <포트 번호>" << std::endl;
        return 1;
    }

    const char* server_ip = argv[1];
    int port = std::atoi(argv[2]);

    // 1. 소켓 생성
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        std::cerr << "소켓 생성 에러: " << strerror(errno) << std::endl;
        return 1;
    }

    // 2. 서버 주소 설정
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        std::cerr << "잘못된 서버 IP 주소입니다." << std::endl;
        close(client_fd);
        return 1;
    }

    // 3. 서버에 연결
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "서버에 연결하는 중 에러 발생: " << strerror(errno) << std::endl;
        close(client_fd);
        return 1;
    }

    std::cout << "서버에 연결되었습니다: " << server_ip << ":" << port << std::endl;

    // 4. 소켓을 논블로킹 모드로 설정
    int flags = fcntl(client_fd, F_GETFL, 0);
    if (flags == -1) flags = 0;
    if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "소켓 논블로킹 모드 설정 에러: " << strerror(errno) << std::endl;
        close(client_fd);
        return 1;
    }

    // 표준 입력을 논블로킹 모드로 설정
    int stdin_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (stdin_flags == -1) stdin_flags = 0;
    if (fcntl(STDIN_FILENO, F_SETFL, stdin_flags | O_NONBLOCK) == -1) {
        std::cerr << "표준 입력 논블로킹 모드 설정 에러: " << strerror(errno) << std::endl;
        close(client_fd);
        return 1;
    }

    fd_set read_fds;
    int max_fd = client_fd > STDIN_FILENO ? client_fd : STDIN_FILENO;

    char buffer[BUFFER_SIZE];
    bool running = true;

    while (running) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);   // 표준 입력 추가
        FD_SET(client_fd, &read_fds);      // 소켓 추가

        // 타임아웃 없이 select 호출
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (activity < 0 && errno != EINTR) {
            std::cerr << "select 에러 발생: " << strerror(errno) << std::endl;
            break;
        }

        // 표준 입력에서 데이터가 있는 경우
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            std::string message;
            std::getline(std::cin, message);

            // 종료 조건
            if (message == "exit") {
                std::cout << "연결을 종료합니다..." << std::endl;
                running = false;
            } else {
                // 서버에 메시지 전송
                if (send(client_fd, message.c_str(), message.length(), 0) == -1) {
                    std::cerr << "메시지 전송 에러: " << strerror(errno) << std::endl;
                    running = false;
                }
            }
        }

        // 소켓에서 데이터가 있는 경우
        if (FD_ISSET(client_fd, &read_fds)) {
            std::memset(buffer, 0, BUFFER_SIZE);
            int bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_received > 0) {
                std::cout << "서버로부터 수신된 메시지: " << buffer << std::endl;
            } else if (bytes_received == 0) {
                std::cout << "서버가 연결을 종료했습니다." << std::endl;
                running = false;
            } else {
                if (errno != EWOULDBLOCK && errno != EAGAIN) {
                    std::cerr << "메시지 수신 에러: " << strerror(errno) << std::endl;
                    running = false;
                }
            }
        }
    }

    // 5. 소켓 닫기
    close(client_fd);
    return 0;
}
