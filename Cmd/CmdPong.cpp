#include "Cmd.hpp"

// void Cmd::cmdPong() {
// 	// 명령어를 보낸 클라이언트가 register 되지 않은 경우
//     if (client->getRegisteredStatus() == false)
//         throw Cmd::CmdException(server.makeMsg(PREFIX_SERVER, ERR_NOTREGISTERED(client->getNickname())));
// 	client->updateLastPongSent();
// }

void Cmd::cmdPong() {
    // 명령어를 보낸 클라이언트가 register 되지 않은 경우
    if (client->getRegisteredStatus() == false)
        throw Cmd::CmdException(server.makeMsg(PREFIX_SERVER, ERR_NOTREGISTERED(client->getNickname())));

    // 클라이언트의 마지막 PONG 응답 시간 업데이트
    client->updateLastPongReceived();
}