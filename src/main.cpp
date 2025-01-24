#include "../inc/Webserv.hpp"
#include "../inc/Network/config/ConfigParser.hpp"

// void	runServer(void)
// {
// 	Server	server(PORT);
// 	Epoll	epoll(server.getSocket().getFd());

// 	while (true) {
// 		try {
// 			epoll.wait();
// 		} catch (std::exception &e) {
// 			std::cout << e.what() << std::endl;
// 		}

// 		for (int i = 0; i < epoll.getReadyFd(); i++) {
// 			if (epoll.getFd(i) == server.getSocket().getFd()) {
// 				try {
// 					server.acceptClient();
// 				} catch (std::exception &e) {
// 					std::cout << e.what() << std::endl;
// 				}
// 			}
// 			else {
// 				server.readFrom(i);
// 				server.sendTo(i);
// 			}
// 		}
// 	}
// }

// int	main(void)
// {
// 	try {
// 		runServer();
// 	}
// 	catch (std::exception &e) {
// 		std::cout << e.what() << std::endl;
// 		return 1;
// 	}
// 	return 0;
// }

#define PORT 9001

// void runServer(void) {
// 	Server server(PORT);
// 	Epoll epoll(server.getSocket().getFd());
// }

int main(int ac, char** av) {
	if (ac == 1 || ac == 2) {
		ConfigParser parser;
		try
		{
			parser.createCluster(ac == 1 ? "config/webserv.conf" : av[1]);
			// std::cout << "Root: " << parser.getValue("root") << std::endl;
			// std::cout << "Port: " << parser.getValue("listen") << std::endl;
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}

		// try {
		// 	runServer();
		// } catch (std::exception& e) {
		// 	std::cout << e.what() << std::endl;
		// 	return 1m 
		// }
		// std::cout << "Closing server, bye !" << std::endl;
		return 0;
	}

	return (std::cout << "Error: wrong number of arguments." << std::endl, 1);
}
