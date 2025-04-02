#include "Socket.hpp"

#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "style.hpp"

Socket::Socket(void) : _opt(1), _fd(-1) {}

Socket::Socket(int port) : _opt(1) {
	_fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
	if (_fd == -1)
		throw std::runtime_error("Failed to create socket");

	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(port);
	_addr.sin_addr.s_addr = INADDR_ANY;

	_setOpt();

	if (bind(_fd, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
		throw std::runtime_error("Failed to bind socket");
}

Socket::~Socket()
{
	close(_fd);
}

void	Socket::_setOpt(void)
{
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &_opt, sizeof(_opt)) < 0)
		throw std::runtime_error("Failed to set socket options");
	if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set socket to non-blocking");
}

void	Socket::fill(int fd, sockaddr_in addr)
{
	_fd = fd;
	_addr = addr;
	_setOpt();
}

int Socket::getFd(void) const
{
	return _fd;
}

struct sockaddr_in	Socket::getAddr(void) const
{
	return _addr;
}
