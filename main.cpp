#include "Server.hpp"

static void	signal_handler(int signal) {
	(void)signal;
	std::cerr << "SERV_LOG: " << C_ERR << "Server received signal (" << signal << ") - initiating shutdown process." << C_RESET << std::endl;
}

int main(int ac, char** av) {
	if (ac != 3) {
		std::cerr << C_ERR << "Error: " << ERR_ARG_COUNT << C_RESET << std::endl;
		return 1;
	}

	try {
		signal(SIGINT, signal_handler);
		signal(SIGQUIT, signal_handler);
		IrcServer server(av[1], av[2]);
		server.init();
		server.run();
	} catch (const IrcServer::ServerException &e) {
		std::cerr << C_ERR << "Error: " << e.what() << C_RESET << std::endl;
		return 1;
	}

	return 0;
}
