#include "../../inc/Network/Socket.hpp"

Socket::Socket(void) : _opt(1), _fd(-1) {}

Socket::Socket(int port) : _opt(1) {
	if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw std::runtime_error("Failed to create socket");
	std::cout << "fd setted = " << _fd << std::endl;
	_setOpt();

	memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(port);
	_addr.sin_addr.s_addr = INADDR_ANY;
	
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
	_addr = ref._addr;
	return *this;
}

void	Socket::_setOpt(void)
{
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &_opt, sizeof(_opt)) < 0)
		throw std::runtime_error("Failed to set socket options");
	if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set socket to non-blocking");
}

void	Socket::setFd(int fd) { _fd = fd; }
int		Socket::getFd(void) { return _fd; }

void	Socket::fill(int fd, sockaddr_in addr)
{
	_fd = fd;
	_addr = addr;
	_setOpt();
}
