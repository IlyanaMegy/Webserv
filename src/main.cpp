#include "Signal.hpp"
#include "Epoll.hpp"
#include "ServerMonitor.hpp"
#include "CGI.hpp"

void	runServer(std::string configFile)
{
	ServerMonitor 	serverMonitor(configFile);
	Epoll			epoll(serverMonitor.getServers());

	while (!Signal::shouldStopServer) {

		epoll.wait();

		for (int i = 0; i < epoll.getTimeoutFdsNb(); i++) {
			for (std::map<int, Server*>::iterator it = serverMonitor.getServers().begin(); it != serverMonitor.getServers().end(); it++)
				if (it->second->isClientKnown(epoll.getTimeoutFd(i))) {
					if (!it->second->getClient(epoll.getTimeoutFd(i))->getRequest())
						it->second->getClient(epoll.getTimeoutFd(i))->createNewRequest("", &epoll);
					it->second->getClient(epoll.getTimeoutFd(i))->getRequest()->getResponse().fillError("408", "Request Timeout");
				}
				else if (Request* request = it->second->findCGIRequest(epoll.getTimeoutFd(i))) {
					request->treatCGI();
					epoll.deleteFd(epoll.getTimeoutFd(i));
				}
		}

		for (int i = 0; i < epoll.getReadyFdsNb(); i++) {
			for (std::map<int, Server*>::iterator it = serverMonitor.getServers().begin(); it != serverMonitor.getServers().end(); it++) {
				if (epoll.getReadyFd(i) == it->first)
					it->second->acceptClient(epoll);
				else if (it->second->isClientKnown(epoll.getReadyFd(i))) {
					if (epoll.getEvent(i) == (EPOLLIN | EPOLLOUT)
					|| epoll.getEvent(i) == EPOLLIN)
						it->second->readFrom(epoll.getReadyFd(i), &epoll);
					if (epoll.getEvent(i) == (EPOLLIN | EPOLLOUT)
					|| epoll.getEvent(i) == EPOLLOUT)
						it->second->sendTo(epoll.getReadyFd(i));

					if (it->second->getClient(epoll.getReadyFd(i))->getShouldClose()) {
						epoll.deleteFd(epoll.getReadyFd(i));
						it->second->closeConnection(epoll.getReadyFd(i));
					}
				}
				else if (Request* request = it->second->findCGIRequest(epoll.getReadyFd(i))) {
					if (epoll.getReadyFd(i) == request->getCGI()->getReadFd())
						it->second->readFrom(epoll.getReadyFd(i), &epoll, request);
					else
						it->second->sendTo(epoll.getReadyFd(i), &epoll, request);
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
		if (Signal::shouldStopServer)
			return 0;
		std::cout << e.what() << std::endl;
		return 1;
	}
	return 0;
}
