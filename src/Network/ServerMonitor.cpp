#include "../../inc/Network/ServerMonitor.hpp"
#include <iostream>

ServerMonitor::ServerMonitor(){}
ServerMonitor::~ServerMonitor(){}

void ServerMonitor::setupServers(std::vector<ServerConf*> serversConfig)
{
	_servers = serversConfig;
	for (std::vector<ServerConf*>::iterator server_it = _servers.begin(); server_it != _servers.end(); ++server_it)
	{
		bool serverDub = false;
		for (std::vector<ServerConf*>::iterator otherserver_it = _servers.begin(); otherserver_it != server_it; ++otherserver_it)
		{
			if ((*otherserver_it)->getHost() == (*server_it)->getHost() && (*otherserver_it)->getPort() == (*server_it)->getPort())
			{
				std::cout << PURP << "Duuub server detected : " << (*otherserver_it)->getServerName() << RESET << std::endl;				
				(*server_it)->getSocket().setFd((*otherserver_it)->getSocketFd());
				serverDub = true;
			}
		}
		if (!serverDub)
			(*server_it)->setSocketServer();
		
		std::cout << MAGENTA << "[CONFIG] Server '" << (*server_it)->getServerName() << "' created." << RESET << std::endl;
	}
}

void ServerMonitor::runServers(void)
{
	for(std::vector<ServerConf*>::iterator server = _servers.begin(); server != _servers.end(); ++server)
    {
		if (listen((*server)->getSocketFd(), MAXCONNECT) < 0)
		{
			std::cout << "err =" << strerror(errno) << std::endl;
			throw std::runtime_error("Failed to listen on socket");
		}

		fcntl((*server)->getSocketFd(), F_SETFL, O_NONBLOCK);
		std::cout << MAGENTA << "\n[CONFIG] server's socket listening..." << std::endl;
		std::cout << "[CONFIG] Server is running on port " << (*server)->getPort() << "..." << RESET << std::endl;
	}
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