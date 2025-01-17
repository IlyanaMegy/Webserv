#include "../inc/Server.hpp"

#include <iostream>

Server::Server(int port) : _socket(port)
{
	if (listen(_socket.getFd(), MAXCONNECT) < 0)
		throw std::runtime_error("Failed to listen on socket");
	// std::cout << "server's socket listening..." << std::endl;
	std::cout << "Server is running on port " << port << "..." << std::endl;
}

Server::~Server(void) {}

Socket &Server::getSocket(void)
{
	return _socket;
}

Client &Server::getClient(int i)
{
	return _clients[i];
}

void Server::acceptClient(void)
{
	int		clientSocket;
	Client	client(_socket.getFd());

	clientSocket = client.getSocket().getFd();
	_clients[clientSocket] = client;
}

void Server::readFrom(int clientFd) {}

void Server::sendTo(int clientFd) {}

const char *Server::SocketCreationErrException::what() const throw() {
	std::cerr << "Erreur lors de la création du socket" << std::endl;
	exit(EXIT_FAILURE);
}
