#include "ArgParser.hpp"
#include "Server.hpp"

int main(int ac, char** av) {
	try {
		ArgParser arg(ac, av);
		IrcServer server(arg.getPort(), arg.getPassword());
		server.init();
		server.run();
	} catch (const ArgParser::ArgException& e) {
		std::cerr << C_ERR << "Error: " << e.what() << C_RESET << std::endl;
		return 1;
	} catch (const IrcServer::ServerException &e) {
		std::cerr << C_ERR << "Error: " << e.what() << C_RESET << std::endl;
		return 1;
	}
	return 0;
}
