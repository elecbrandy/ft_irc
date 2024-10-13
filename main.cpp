#include "ArgParser.hpp"
#include "Server.hpp"

int main(int ac, char** av) {
	try {
		ArgParser arg(ac, av);
		IrcServer server(arg.getPort(), arg.getPassword());
		server.init();
		server.run();
	} catch (const ArgParser::ArgException& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	// } catch (const Server::ServerException& e) {
	// 	std::cerr << "Error: " << e.what() << std::endl;
	// 	return 1;
	} catch (const std::exception& e) {
		std::cerr << "Error: Unkown..." << e.what() << std::endl;
		return 1;
	}
	return 0;
}
