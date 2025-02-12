#include "../../inc/Network/Server.hpp"


Server::Server(ServerConf config) : _serverConfig(config)
{
	std::cout << PINK << "Server Socket Fd = " << _serverConfig.getSocketFd() << RESET << std::endl;
	if (listen(_serverConfig.getSocketFd(), MAXCONNECT) < 0)
	{
		std::cerr << "Failed to listen on socket: " << strerror(errno) << std::endl;
		throw std::runtime_error("Failed to listen on socket");	
	}
	std::cout << "server's socket listening..." << std::endl;
	std::cout << "Server is running on port " << config.getPort() << "..." << std::endl;
}

Server::~Server(void) {}

Socket &Server::getSocket(void)
{
	return _serverConfig.getSocket();
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
