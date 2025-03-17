#include "../../inc/network/Client.hpp"
#include "../../inc/network/Server.hpp"

Client::Client(void) : _request(NULL), _shouldClose(false), _server(NULL), leftoverMessage("") {}

Client::Client(Server* server) : _request(NULL), _shouldClose(false), _server(server), leftoverMessage("")
{
	int	clientFd;
	struct sockaddr_in addr;
	socklen_t	addrLen = sizeof(struct sockaddr_in);

	clientFd = accept(server->getSocket().getFd(), (struct sockaddr *)&addr, &addrLen);
	if (clientFd == -1)
		throw std::runtime_error("[CLIENT] Error : Failed to accept client");
	_socket.fill(clientFd, addr);
}

Client::~Client(void){}

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

void	Client::createNewRequest(std::string leftoverMessage)
{
	_request = new Request(_server, leftoverMessage, _server->getDefaultConf());
	if (!_request)
		throw std::runtime_error("[CLIENT] Error : Failed to create new request");
}

void	Client::deleteRequest(void)
{
	delete _request;
	_request = NULL;
}
