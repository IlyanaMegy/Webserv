#include "../inc/Socket.hpp"

#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>

Socket::Socket(void) {}

Socket::Socket(int port) : _opt(1) {
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == -1) 
		throw std::runtime_error("Failed to create socket");

	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(port);
	_addr.sin_addr.s_addr = INADDR_ANY;

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
	//machin fcntl
}

// Fonction pour mettre un socket en mode non-bloquant
// int make_socket_non_blocking(int socket_fd) {
// 	int flags = fcntl(socket_fd, F_GETFL, 0);
// 	if (flags == -1) {
// 		perror("fcntl(F_GETFL)");
// 		return -1;
// 	}

// 	flags |= O_NONBLOCK;
// 	if (fcntl(socket_fd, F_SETFL, flags) == -1) {
// 		perror("fcntl(F_SETFL)");
// 		return -1;
// 	}

// 	return 0;
// }

void	Socket::fill(int fd, struct sockaddr_in addr)
{
	_fd = fd;
	_addr = addr;
}

// SERVER
// Listening socket
// void Socket::listen_socket(int backlog) {
// 	if (listen(_fd, backlog) < 0)
// 		throw std::runtime_error("Failed to listen on socket");
// 	std::cout << "server's socket listening..." << std::endl;
// }

int Socket::getFd(void) const
{
	return _fd;
}
