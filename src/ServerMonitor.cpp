#include "../../inc/ServerMonitor.hpp"

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
			server = new Server((*it)->getPort(), *it);
			if (!server)
				throw std::runtime_error("Failed to create Server object");
			addServerToList(server);
		}
		if (server->isConfigKnown((*it)->getServerName()))
			continue;
		server->addConfig(*it);
		std::cout << MAGENTA << "[CONFIG] Server " << (*it)->getServerName() << " is listening on port " << (*it)->getPort() << RESET << std::endl;
		std::cout << std::endl;

		std::cout << OLIV << "testing function getCgiPathForScript()\ntesting path : /hello/test.py" << std::endl;
		std::cout << "-> " << (*it)->getCgiPathForScript("/hello/test.py") << RESET << std::endl;
		std::cout << std::endl;
		std::cout << OLIV << "RETRY testing function getCgiPathForScript()\ntesting path : /hello/test.py" << std::endl;
		std::cout << "-> " << (*it)->getCgiPathForScript("/hello/test.py") << RESET << std::endl;
		std::cout << std::endl;
		std::cout << OLIV << "testing function getCgiPathForScript()\ntesting path : /hello/test2.py" << std::endl;
		std::cout << "-> " << (*it)->getCgiPathForScript("/hello/toto.py") << RESET << std::endl;
	}
}

ServerMonitor::~ServerMonitor()
{
	for (std::map<int, Server*>::iterator it = _servers.begin(); it != _servers.end(); it++)
		if (it->second)
			delete it->second;

	for (std::vector<ServerConf*>::iterator it = _confs.begin(); it != _confs.end(); it++)
		if (*it)
			delete *it;
}


// void ServerMonitor::runServers(void)
// {
// 	// for(std::vector<ServerConf*>::iterator server = _servers.begin(); server != _servers.end(); ++server)
//     // {
// 	// 	// std::cout << "test :\n" << std::endl;
// 	// 	// (*server)->listMethods();
// 	// 	// if ((*server)->isValidMethod("/drafts/", GET))
// 	// 	// 	std::cout << "valid location" << std::endl;
// 	// 	// else
// 	// 	// 	std::cout << "invalid location" << std::endl;

// 	// 	if (listen((*server)->getSocketFd(), MAXCONNECT) < 0)
// 	// 		throw std::runtime_error("Failed to listen on socket");

// 	// 	std::cout << MAGENTA << "\n[CONFIG] server's socket listening..." << std::endl;
// 	// 	std::cout << "[CONFIG] Server is running on port " << (*server)->getPort() << "..." << RESET << std::endl;
// 	// }
// }

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