#include "Signal.hpp"
#include "Epoll.hpp"
#include "ServerMonitor.hpp"

void	runServer(std::string configFile)
{
	ServerMonitor 	serverMonitor(configFile);
	Epoll			epoll(serverMonitor.getServers());

	while (true) {
		if (Signal::shouldStopServer)
			return ;

		try {
			epoll.wait();
		} catch (std::exception &e) {
			std::cout << e.what() << std::endl;
		}

		for (int i = 0; i < epoll.getReadyFdsNb(); i++) {
			for (std::map<int, Server*>::iterator it = serverMonitor.getServers().begin(); it != serverMonitor.getServers().end(); it++) {
				if (epoll.getFd(i) == it->first) {
					try {
						it->second->acceptClient(epoll);
					} catch (std::exception &e) {
						std::cout << e.what() << std::endl;
					}
				}
				else if (it->second->isClientKnown(epoll.getFd(i))) {
					if (epoll.getEvent(i) == (EPOLLIN | EPOLLOUT)
					|| epoll.getEvent(i) == EPOLLIN)
						it->second->readFrom(epoll.getFd(i), &epoll);
					if (epoll.getEvent(i) == (EPOLLIN | EPOLLOUT)
					|| epoll.getEvent(i) == EPOLLOUT)
						it->second->sendTo(epoll.getFd(i));

					if (it->second->getClient(epoll.getFd(i))->getShouldClose()) {
						epoll.deleteFd(epoll.getFd(i));
						it->second->closeConnection(epoll.getFd(i));
					}
				}
				else if (Request* request = it->second->findCGIRequest(epoll.getFd(i))) {
					it->second->readFrom(epoll.getFd(i), &epoll, request);
				}
			}
		}
	}
}

int	main(int ac, char** av)
{
	std::string configFile;
	
	if (ac != 1 && ac != 2)
		return (std::cout << "Error: wrong number of arguments." << std::endl, 1);
	configFile = (ac == 1) ? "config/webserv.conf" : av[1];
	try
	{
		Signal::setHandler();
		runServer(configFile);
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}
	return 0;
}
