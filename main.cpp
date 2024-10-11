#include "Server.hpp"

int main(int ac, char** av) {
	if (ac != 3) {
		std::cerr << "error" << std::endl;
		return 1;
	}

	int port = std::atoi(av[1]);
	std::string password = av[2];

	IrcServer server(port, password);
	server.init();

	server.run();

	return 0;
}
