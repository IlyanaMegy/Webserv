#include "../../inc/ServerMonitor.hpp"

ServerMonitor::ServerMonitor(std::string configFile)
{
	data.createCluster(configFile);
	_confs = data.getServers();

	for (std::vector<ServerConf*>::iterator it = _confs.begin(); it != _confs.end(); ++it)
	{
		Server *server;

		server = findServer((*it)->getPort());
		if (server == NULL) {
			server = new Server((*it)->getPort(), *it);
			if (!server)
				throw std::runtime_error("Failed to create Server object");
			addServerToList(server);
		}
		if (server->isConfigKnown((*it)->getServerName()))
			continue;
		server->addConfig(*it);
		std::cout << MAGENTA << "[CONFIG] Server " << (*it)->getServerName() << " is listening on port " << (*it)->getPort() << RESET << std::endl;
	}
}

ServerMonitor::~ServerMonitor()
{
	for (std::map<int, Server*>::iterator it = _servers.begin(); it != _servers.end(); it++)
		if (it->second)
			delete it->second;
}



Server *ServerMonitor::findServer(uint16_t port)
{
	for (std::map<int, Server*>::iterator it = _servers.begin(); it != _servers.end(); ++it)
        if (it->second->getPort() == port)
            return it->second;
    return NULL;
}

void ServerMonitor::addServerToList(Server *server)
{
	_servers[server->getSocket().getFd()] = server;
}

std::map<int, Server*>& ServerMonitor::getServers(void)
{
	return _servers;
}
