#include "../../inc/Network/ServerMonitor.hpp"
#include <iostream>

ServerMonitor::ServerMonitor(){}
ServerMonitor::~ServerMonitor(){}

void ServerMonitor::setupServers(std::vector<ServerConf> serversConfig)
{
	_servers = serversConfig;
	for (std::vector<ServerConf>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		_servers_lst.insert(std::make_pair(it->getSocketFd(), *it));
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
		
		std::cout << MAGENTA << "[CONFIG] Server '" << it->getServerName() << "' created." << RESET << std::endl;
	}
}
