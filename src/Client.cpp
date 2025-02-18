#include "Client.hpp"

Client::Client(void) : _request(NULL), _shouldClose(false), leftoverMessage("") {}

Client::Client(int serverFd, std::string leftoverMessage) : _request(NULL), _shouldClose(false), leftoverMessage(leftoverMessage)
{
	int	clientFd;
	struct sockaddr_in addr;
	socklen_t	addrLen = sizeof(struct sockaddr_in);

	clientFd = accept(serverFd, (struct sockaddr *)&addr, &addrLen);
	if (clientFd == -1)
		throw std::runtime_error("Failed to accept client");
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
	_request = new Request(leftoverMessage);
	if (!_request)
		throw std::exception();
}

void	Client::deleteRequest(void)
{
	delete _request;
	_request = NULL;
}
