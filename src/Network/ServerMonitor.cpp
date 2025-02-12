#include "../../inc/Network/ServerMonitor.hpp"
#include <iostream>

ServerMonitor::ServerMonitor(){}
ServerMonitor::~ServerMonitor(){}

void ServerMonitor::createServers(std::vector<ServerConf> servers)
{
	for (std::vector<ServerConf>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		bool serverDub = false;
		// for (std::vector<ServerConf>::iterator it2 = servers.begin(); it2 != it; ++it2)
		// {
		// 	if (it2->getHost() == it->getHost() && it2->getPort() == it->getPort())
		// 	{
		// 		it->setFd(it2->getFd());
		// 		serverDub = true;
		// 	}
		// }
		if (!serverDub)
			it->setSocketServer();
		std::cout << "" << std::endl;
		std::cout << MAGENTA << "Server n." << it->getServerName() << " created." << RESET << std::endl;
	}
}
