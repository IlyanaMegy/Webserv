#include "Client.hpp"

Client::Client(void) : _shouldClose(false) {}

Client::Client(int serverFd) : _shouldClose(false)
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

Socket &Client::getSocket(void)
{
	return _socket;
}


bool	Client::getShouldClose(void) const
{
	return _shouldClose;
}

void	Client::read(void)
{
	char	buffer[BUFFER_SIZE] = "";
	ssize_t	res;

	res = recv(_socket.getFd(), buffer, BUFFER_SIZE, 0);
	if (res == 0)
		_shouldClose = true;
	else
		;
}
