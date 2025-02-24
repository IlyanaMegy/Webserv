#include "../inc/Network/Epoll.hpp"
#include "../inc/Network/ServerMonitor.hpp"

void epollSetup(ServerMonitor servers) {
	for(std::vector<ServerConf*>::iterator server = servers.getServers().begin(); server != servers.getServers().end(); ++server)
    {
		Epoll epoll((*server)->getSocket().getFd());
		int ready_fds = epoll.wait();
		for (int i = 0; i < epoll.getReadyFd(); i++) {
			// if (epoll.getFd(i) == ((*server)->getSocket().getFd())) {
			// 	try {
			// 		(*server).acceptClient();
			// 	} catch (std::exception& e) {
			// 		std::cout << e.what() << std::endl;
			// 	}
			// } else {
			// 	(*server).readFrom(i);
			// 	(*server).sendTo(i);
			// }
			std::cout << LIME << "here " << ready_fds << RESET << std::endl;
		}
	}
}

int main(int ac, char** av) {
	if (ac != 1 && ac != 2)
		return (std::cout << "Error: wrong number of arguments." << std::endl, 1);



	ConfigParser data;
	ServerMonitor servers;

	
	try {
		servers.setupServers(data.getServers());

		// 	Run Servers
		servers.runServers();
	} catch (std::exception& e) {
		return (std::cout << e.what() << std::endl, 1);
	}

	while (true) {
		try
		{
			epollSetup(servers);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}

	return 0;
}

void	runServer(std::string configFile)
{
	ServerMonitor serverMonitor(configFile);
	// Epoll	epoll(server.getSocket().getFd());

	// while (true) {
	// 	try {
	// 		epoll.wait();
	// 	} catch (std::exception &e) {
	// 		std::cout << e.what() << std::endl;
	// 	}

	// 	for (int i = 0; i < epoll.getReadyFdsNb(); i++) {
	// 		if (epoll.getFd(i) == server.getSocket().getFd()) {
	// 			try {
	// 				server.acceptClient(epoll);
	// 			} catch (std::exception &e) {
	// 				std::cout << e.what() << std::endl;
	// 			}
	// 		}

	// 		else {
	// 			if (epoll.getEvent(i) == (EPOLLIN | EPOLLOUT)
	// 			|| epoll.getEvent(i) == EPOLLIN)
	// 				server.readFrom(epoll.getFd(i));
	// 			if (epoll.getEvent(i) == (EPOLLIN | EPOLLOUT)
	// 			|| epoll.getEvent(i) == EPOLLOUT)
	// 				server.sendTo(epoll.getFd(i));

	// 			if (server.getClient(epoll.getFd(i))->getShouldClose()) {
	// 				epoll.deleteFd(epoll.getFd(i));
	// 				server.closeConnection(epoll.getFd(i));
	// 			}
	// 		}
	// 	}
	// }
}


int	main(int ac, char** av)
{
	std::string configFile = "config/webserv.conf";

	if (ac != 1 && ac != 2)
		return (std::cout << "Error: wrong number of arguments." << std::endl, 1);
	if (ac == 1)
		configFile = av[1];

	try {
		runServer(configFile);
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		return 1;
	}
	return 0;
}