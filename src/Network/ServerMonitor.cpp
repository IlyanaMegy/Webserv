#include "../../inc/Network/ServerMonitor.hpp"
#include <iostream>

ServerMonitor::ServerMonitor(){}
ServerMonitor::~ServerMonitor(){}

void ServerMonitor::setupServers(std::vector<ServerConf*> serversConfig)
{
	_servers = serversConfig;
	for (std::vector<ServerConf*>::iterator server_it = _servers.begin(); server_it != _servers.end(); ++server_it)
	{
		// bool serverDub = false;
		// // for (std::vector<ServerConf>::iterator otherserver_it = _servers.begin(); otherserver_it != server_it; ++otherserver_it)
		// // {
		// // 	if (otherserver_it->getHost() == server_it->getHost() && otherserver_it->getPort() == server_it->getPort())
		// // 	{
		// // 		std::cout << "Duuuuuuuuuuub" << std::endl;
		// // 		server_it->setSocketServer(true, otherserver_it->getSocketFd());
		// // 		serverDub = true;
		// // 	}
		// // }
		// if (!serverDub)
		(*server_it)->setSocketServer();

		
		// _servers_lst.insert(std::make_pair(server_it->getSocketFd(), *server_it));
		std::cout << MAGENTA << "[CONFIG] Server '" << (*server_it)->getServerName() << "' created." << RESET << std::endl;
	}
}

void ServerMonitor::runServers(void)
{
	for(std::vector<ServerConf*>::iterator server = _servers.begin(); server != _servers.end(); ++server)
    {
		std::cout << (*server)->getServerName() << " fd = " << (*server)->getSocketFd() << std::endl;
		if (listen((*server)->getSocketFd(), MAXCONNECT) < 0)
		{
			std::cout << "err =" << strerror(errno) << std::endl;
			throw std::runtime_error("Failed to listen on socket");
		}

		std::cout << "server's socket listening..." << std::endl;
		std::cout << "Server is running on port " << (*server)->getPort() << "..." << std::endl;
	}
}
