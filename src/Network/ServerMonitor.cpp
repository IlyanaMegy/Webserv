#include "../../inc/Network/ServerMonitor.hpp"

ServerMonitor::ServerMonitor(std::string configFile)
{
	ConfigParser data;

	data.createCluster(configFile);
	_confs = data.getServers();

	for (std::vector<ServerConf*>::iterator it = _confs.begin(); it != _confs.end(); ++it)
	{
		Server *server;

		server = findServer((*it)->getPort());
		if (server == NULL) {
			server = new Server((*it)->getPort());
			if (!server)
				throw std::exception();
			servers[(*it)->getPort()] = server;
		}
		if (server->isConfigKnown((*it)->getServerName()))
			continue;
		std::cout << OLIV << "[CONFIG] Adding new server " << (*it)->getServerName() << " to _servers map of ServerMonitor" << std::endl;
		server[(*it)->getServerName()] = *it;
	}

		std::cout << MAGENTA << "[CONFIG] Server '" << (*server_it)->getServerName() << "' created." << RESET << std::endl;


}

ServerMonitor::~ServerMonitor()
{
	for (std::map<int, Server*>::iterator it = servers.begin(); it != servers.end(); it++) {
		if (it->second)
			delete it->second;
	}
	for (std::vector<ServerConf*>::iterator it = _confs.begin(); it != _confs.end(); it++) {
		if (*it)
			delete *it;
	}
}

void ServerMonitor::setupServers(std::vector<ServerConf*> serversConfig)
{
	_confs = serversConfig;
	
}

void ServerMonitor::runServers(void)
{
	// for(std::vector<ServerConf*>::iterator server = _servers.begin(); server != _servers.end(); ++server)
    // {
	// 	// std::cout << "test :\n" << std::endl;
	// 	// (*server)->listMethods();
	// 	// if ((*server)->isValidMethod("/drafts/", GET))
	// 	// 	std::cout << "valid location" << std::endl;
	// 	// else
	// 	// 	std::cout << "invalid location" << std::endl;

	// 	if (listen((*server)->getSocketFd(), MAXCONNECT) < 0)
	// 		throw std::runtime_error("Failed to listen on socket");

	// 	std::cout << MAGENTA << "\n[CONFIG] server's socket listening..." << std::endl;
	// 	std::cout << "[CONFIG] Server is running on port " << (*server)->getPort() << "..." << RESET << std::endl;
	// }
}


// Client &Server::getClient(int i)
// {
// 	return _clients[i];
// }

// void Server::acceptClient(void)
// {
// 	int		clientSocket;
// 	Client	client(_socket.getFd());

// 	clientSocket = client.getSocket().getFd();
// 	_clients[clientSocket] = client;
// }

// void Server::readFrom(int clientFd)
// {
// 	(void) clientFd;
// }

// void Server::sendTo(int clientFd)
// {
// 	(void) clientFd;
// }

// const char *Server::SocketCreationErrException::what() const throw() {
// 	std::cerr << "Erreur lors de la création du socket" << std::endl;
// }