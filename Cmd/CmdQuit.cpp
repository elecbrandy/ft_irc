#include "Cmd.hpp"

void Cmd::cmdQuit() {
    //removeClientFrom~~ 그거 넣기
    server.removeClinetFromServer(server.getClient(client_fd));
}