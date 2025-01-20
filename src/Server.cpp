#include "../inc/Server.hpp"

#include <iostream>

Server::Server(int port) : _port(port), _socket(port)
{
	(void) _port;
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

void Server::acceptClient(Epoll &epoll)
{
	int		clientSocket;
	Client	client(_socket.getFd());

	clientSocket = client.getSocket().getFd();
	_clients[clientSocket] = client;
	std::cout << "New client accepted!" << std::endl;
	epoll.addFd(clientSocket, EPOLLIN | EPOLLOUT);
}

void Server::readFrom(int clientFd)
{
	(void) clientFd;
}

void Server::sendTo(int clientFd)
{
	(void) clientFd;
}

// const char *Server::SocketCreationErrException::what() const throw() {
// 	std::cerr << "Erreur lors de la création du socket" << std::endl;
// }
