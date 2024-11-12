#include "Cmd.hpp"

void Cmd::cmdQuit() {
    server.removeClinetFromServer(server.getClient(client_fd));
}