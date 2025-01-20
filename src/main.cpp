#include "../inc/config/ServerConf.hpp"
#include "../inc/Epoll.hpp"
#include "../inc/Server.hpp"
#include "../inc/tools/Debugger.hpp"

// #define PORT 8080

// #define FILE "configs/default.conf"

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

#define FILE "configs/default.conf"

void runServer(void) {
	Server server(PORT);
	Epoll epoll(server.getSocket().getFd());
}

int main(void) {
	try 
	{
		ServerConf parser("conf/webserv.conf");
		std::cout << "Root: " << parser.getValue("root") << std::endl;
		std::cout << "Port: " << parser.getValue("listen") << std::endl;
		
	} 
	catch (std::exception& e) 
	{
		std::cerr << e.what() << std::endl;
	}

	try {
		runServer();
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}
	std::cout << "Closing server, bye !" << std::endl;
	return 0;
}
