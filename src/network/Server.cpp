#include "../../inc/network/Server.hpp"
#include "../../inc/messages/Request.hpp"
#include "../../inc/messages/ServerConf.hpp"

#include <iostream>

Server::Server(unsigned int port, ServerConf* defaultConf) : _port(port), _socket(port), _defaultConf(defaultConf)
{
	(void) _port;
	(void) _defaultConf;
	if (listen(_socket.getFd(), MAXCONNECT) < 0)
		throw std::runtime_error("Failed to listen on socket");
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

unsigned int	Server::getPort(void)
{
	return _port;
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
	Client	*client = new Client(this);

	if (!client)
		throw std::exception();
	clientSocket = client->getSocket().getFd();
	_clients[clientSocket] = client;
	std::cout << GOLD << "[SERVER] New client " << clientSocket << " accepted on port " << _port << "!" << RESET << std::endl;
	epoll.addFd(clientSocket, EPOLLIN | EPOLLOUT);
}

void	Server::readFrom(int clientFd)
{
	char		buffer[BUFFER_SIZE] = "";
	ssize_t		res;

	res = recv(clientFd, buffer, BUFFER_SIZE - 1, 0);
	if (res == -1)
		throw std::exception();
	if (res == 0) {
		_clients[clientFd]->setShouldClose(true);
		return ;
	}
	if (!_clients[clientFd]->getRequest())
		_clients[clientFd]->createNewRequest(_clients[clientFd]->leftoverMessage);
	_clients[clientFd]->getRequest()->add(std::string(buffer, res));
	std::cout << GOLD << "[SERVER] New Request added for client " << BOLD << clientFd << RESET << GOLD << " !\n>>\n" << _clients[clientFd]->getRequest()->getUntreatedMessage() << RESET << std::endl;
	while (_clients[clientFd]->getRequest()->getStage() != Request::DONE && _clients[clientFd]->getRequest()->getState() == Request::TREATING_MESSAGE)
		_clients[clientFd]->getRequest()->parse();
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
	std::cout << GOLD << "[SERVER] Response sent to client " << BOLD << clientFd << RESET << std::endl;
}

bool Server::isConfigKnown(std::string serverName)
{
	return _confs.find(serverName) != _confs.end();
}

bool Server::isClientKnown(int clientFd)
{
	return _clients.find(clientFd) != _clients.end();
}

void Server::addConfig(ServerConf *conf)
{
	_confs[conf->getServerName()] = conf;
}

ServerConf* Server::getDefaultConf() const
{
	return _defaultConf;
}

