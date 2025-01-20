#include "../inc/Client.hpp"

Client::~Client(){}
Client::Client(int serverFd)
{
	int	clientFd;
	struct sockaddr_in addr;
	socklen_t	addrLen;

	clientFd = accept(serverFd, (struct sockaddr *)&addr, &addrLen);
	if (clientFd == -1)
		throw std::runtime_error("Failed to accept client");
	_socket.fill(clientFd, addr);
}

Socket &Client::getSocket(void)
{
	return _socket;
}
