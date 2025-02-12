#include "../inc/Network/Epoll.hpp"
#include "../inc/Network/Server.hpp"

int runServer(const ServerConf serverConfig) {
	Server server(serverConfig);
	std::cout << PINK << "Server created" << RESET << std::endl;
	// Epoll epoll(server.getSocket().getFd());

	// while (true) {
	// 	try {
	// 		epoll.wait();
	// 	} catch (std::exception& e) {
	// 		std::cout << e.what() << std::endl;
	// 	}

	// 	// for (int i = 0; i < epoll.getReadyFd(); i++) {
	// 	// 	if (epoll.getFd(i) == server.getSocket().getFd()) {
	// 	// 		try {
	// 	// 			server.acceptClient();
	// 	// 		} catch (std::exception& e) {
	// 	// 			std::cout << e.what() << std::endl;
	// 	// 		}
	// 	// 	} else {
	// 	// 		server.readFrom(i);
	// 	// 		server.sendTo(i);
	// 	// 	}
	// 	// }
	// }
	return 0;
}

int main(int ac, char** av) {
	if (ac != 1 && ac != 2)
		return (std::cout << "Error: wrong number of arguments." << std::endl, 1);

	ConfigParser parser;
	try {
		parser.createCluster(ac == 1 ? "config/webserv.conf" : av[1]);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	for (size_t i = 0; i < parser.getNbServer(); i++) {
		std::cout << LIME << "\ndealing with server n." << i << RESET << std::endl;
		std::cout << OLIV << "Port: " << parser.getServerPort(i) << RESET << std::endl;

		try {
			runServer(parser.getServerConfig(i));
		} catch (std::exception& e) {
			std::cout << e.what() << std::endl;
			return 1;
		}
		std::cout << "Closing server, bye !" << std::endl;
	}

	return 0;
}
