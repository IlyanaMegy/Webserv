#include "Client.hpp"

#include "Server.hpp"
#include "Epoll.hpp"

Client::Client(void) : _request(NULL), _shouldClose(false), _server(NULL), leftoverMessage("") {}

Client::Client(Server* server) : _request(NULL), _shouldClose(false), _server(server), leftoverMessage("")
{
	int	clientFd;
	struct sockaddr_in addr;
	socklen_t	addrLen = sizeof(struct sockaddr_in);

	clientFd = accept4(server->getSocket().getFd(), (struct sockaddr *)&addr, &addrLen, SOCK_CLOEXEC);
	if (clientFd == -1)
		throw std::runtime_error("[CLIENT] Error : Failed to accept client");
	_socket.fill(clientFd, addr);
}

Client::~Client(void)
{
	if (_request)
		delete _request;
}

Request	*Client::getRequest(void)
{
	return _request;
}

Socket	&Client::getSocket(void)
{
	return _socket;
}

bool	Client::getShouldClose(void) const
{
	return _shouldClose;
}

void	Client::setShouldClose(bool shouldClose)
{
	_shouldClose = shouldClose;
}

void	Client::createNewRequest(std::string leftoverMessage, Epoll* epoll)
{
	_request = new Request(_server, this, epoll, leftoverMessage);
	if (!_request)
		throw std::runtime_error("[CLIENT] Error : Failed to create new request");
}

void	Client::deleteRequest(void)
{
	delete _request;
	_request = NULL;
}
