#include "Cmd.hpp"

void Cmd::cmdCap() {
    const std::string response = "CAP * LS :\r\n";
    send(client_fd, response.c_str(), response.size(), 0);
}
