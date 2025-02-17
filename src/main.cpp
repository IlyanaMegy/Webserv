#include "../inc/Network/Epoll.hpp"
#include "../inc/Network/ServerMonitor.hpp"

// int runServer(std::vector<ServerConf> serversConfig) {
	
// 	std::cout << PINK << "Server created" << RESET << std::endl;


// 	// Epoll epoll(server.getSocket().getFd());

// 	// while (true) {
// 	// 	try {
// 	// 		epoll.wait();
// 	// 	} catch (std::exception& e) {
// 	// 		std::cout << e.what() << std::endl;
// 	// 	}

// 	// 	// for (int i = 0; i < epoll.getReadyFd(); i++) {
// 	// 	// 	if (epoll.getFd(i) == server.getSocket().getFd()) {
// 	// 	// 		try {
// 	// 	// 			server.acceptClient();
// 	// 	// 		} catch (std::exception& e) {
// 	// 	// 			std::cout << e.what() << std::endl;
// 	// 	// 		}
// 	// 	// 	} else {
// 	// 	// 		server.readFrom(i);
// 	// 	// 		server.sendTo(i);
// 	// 	// 	}
// 	// 	// }
// 	// }
// 	return 0;
// }

int main(int ac, char** av) {
	if (ac != 1 && ac != 2)
		return (std::cout << "Error: wrong number of arguments." << std::endl, 1);

	ConfigParser data;
	ServerMonitor servers;

	
	try {
		//	Parse Config
		data.createCluster(ac == 1 ? "config/webserv.conf" : av[1]);

		//	Init Servers
		servers.setupServers(data.getServers());

		// 	Run Servers
		servers.runServers();
	} catch (std::exception& e) {
		return (std::cout << e.what() << std::endl, 1);
	}


	return 0;
}
