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

void ServerMonitor::runServers(void)
{
	for(std::vector<ServerConf>::iterator it = _servers.begin(); it != _servers.end(); ++it)
    {
        //Now it calles listen() twice on even if two servers have the same host:port
        if (listen(it->getSocketFd(), MAXCONNECT) < 0)
        {
          	throw std::runtime_error("Failed to listen on socket");
            exit(EXIT_FAILURE);
        }
		std::cout << "server's socket listening..." << std::endl;
		std::cout << "Server is running on port " << it->getPort() << "..." << std::endl;
	}

}
