#include "Server.hpp"

#include <iostream>

Server::Server(int port) : _port(port), _socket(port)
{
	(void) _port;
	if (listen(_socket.getFd(), MAXCONNECT) < 0)
		throw std::runtime_error("Failed to listen on socket");
	// std::cout << "server's socket listening..." << std::endl;
	std::cout << "Server is running on port " << port << "..." << std::endl;
}

Server::~Server(void)
{
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		if (it->second)
			delete it->second;
	}
}

void	Server::closeConnection(int clientFd)
{
	delete _clients[clientFd];
	_clients.erase(clientFd);
}

Socket	&Server::getSocket(void)
{
	return _socket;
}

Client	*Server::getClient(int clientFd)
{
	return _clients[clientFd];
}

void	Server::acceptClient(Epoll &epoll)
{
	int		clientSocket;
	Client	*client = new Client(_socket.getFd(), "");

	if (!client)
		throw std::exception();
	clientSocket = client->getSocket().getFd();
	_clients[clientSocket] = client;
	std::cout << "New client accepted!" << std::endl;
	epoll.addFd(clientSocket, EPOLLIN | EPOLLOUT);
}

void	Server::readFrom(int clientFd)
{
	char	buffer[BUFFER_SIZE] = "";
	ssize_t	res;

	res = recv(_clients[clientFd]->getSocket().getFd(), buffer, BUFFER_SIZE - 1, 0);
	if (res == 0) {
		_clients[clientFd]->setShouldClose(true);
		return ;
	}
	if (!_clients[clientFd]->getRequest())
		_clients[clientFd]->createNewRequest(_clients[clientFd]->leftoverMessage);
	_clients[clientFd]->getRequest()->parse(std::string(buffer));
	std::cout << "End of reading!" << std::endl;
}

void	Server::sendTo(int clientFd)
{
	Response	response;
	std::string	responseMessage;

	if (!_clients[clientFd]->getRequest() || !_clients[clientFd]->getRequest()->getResponse().getIsComplete())
		return ;
	response = _clients[clientFd]->getRequest()->getResponse();
	response.createMessage();
	if (response.getShouldClose())
		_clients[clientFd]->setShouldClose(true);
	else
		_clients[clientFd]->leftoverMessage = _clients[clientFd]->getRequest()->getUntreatedMessage();
	responseMessage = response.getMessage();
	if (send(clientFd, responseMessage.c_str(), responseMessage.length(), 0) == -1)
		throw std::exception();
	_clients[clientFd]->deleteRequest();
	std::cout << "Sent to client " << CYAN << clientFd << RESET << std::endl;
}


// const char *Server::SocketCreationErrException::what() const throw() {
// 	std::cerr << "Erreur lors de la création du socket" << std::endl;
// }
