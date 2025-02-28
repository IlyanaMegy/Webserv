#include "Epoll.hpp"

#include "Server.hpp"

Epoll::Epoll(std::map<unsigned int, Server*>& servers)
{
	_epollFd = epoll_create(1);
	if (_epollFd == -1)
		throw std::exception();

	try {
		for (std::map<unsigned int, Server*>::iterator it = servers.begin(); it != servers.end(); it++) {
			addFd(it->second->getSocket().getFd(), EPOLLIN);
		}
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}

Epoll::~Epoll(void)
{
	close(_epollFd);
}

int Epoll::getEpollFd(void) const 
{
	return _epollFd;
}

int Epoll::getEvent(int i) const
{
	return _events[i].events;
}

int Epoll::getReadyFdsNb(void) const
{
	return _ReadyFdsNb;
}

int	Epoll::getFd(int i) const
{
	return _events[i].data.fd;
}

void	Epoll::addFd(int fd, int flags)
{
	struct epoll_event event;

	event.events = flags;
	event.data.fd = fd;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &event) == -1)
		throw std::exception();
}

void	Epoll::deleteFd(int fd)
{
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL);
}

void	Epoll::wait(void)
{
	_ReadyFdsNb = epoll_wait(_epollFd, _events, MAX_EVENTS, TIMEOUT);
	if (_ReadyFdsNb == -1)
		throw std::exception();
}
