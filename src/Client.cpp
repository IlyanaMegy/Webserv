#include "Client.hpp"

Client::Client(void) : _buffer("") {}

Client::Client(int serverFd) : _buffer("")
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

{
}

Socket &Client::getSocket(void)
{
	return _socket;
}
