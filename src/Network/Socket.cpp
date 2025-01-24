#include "../../inc/Webserv.hpp"

Socket::Socket(void) : _opt (1), _fd(-1) {}

Socket::Socket(int port) : _opt(1) {
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == -1) 
		throw std::runtime_error("Failed to create socket");

	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(port);
	_addr.sin_addr.s_addr = INADDR_ANY;

	_setOpt();

	if (bind(_fd, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
		throw std::runtime_error("Failed to bind socket");
}

Socket::Socket(Socket const& ref)
{
	*this = ref;
}

Socket::~Socket()
{
	if (_fd >= 0)
		close(_fd);
}

Socket	&Socket::operator=(const Socket &ref)
{
	_opt = ref._opt;
	_fd = ref._fd;
	_addr.sin_family = ref._addr.sin_family;
	_addr.sin_port = ref._addr.sin_port;
	_addr.sin_addr.s_addr = ref._addr.sin_addr.s_addr;
	return *this;
}

void	Socket::_setOpt(void)
{
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &_opt, sizeof(_opt)) < 0)
		throw std::runtime_error("Failed to set socket options");
	if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set socket options");
}

void	Socket::fill(int fd, struct sockaddr_in addr)
{
	_fd = fd;
	_addr = addr;
	_setOpt();
}

int Socket::getFd(void) const
{
	return _fd;
}
