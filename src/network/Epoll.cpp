#include "Epoll.hpp"

#include "Server.hpp"

Epoll::Epoll(std::map<int, Server*>& servers)
{
	_epollFd = epoll_create1(EPOLL_CLOEXEC);
	if (_epollFd == -1)
		throw std::runtime_error("The server encountered an error");

	for (std::map<int, Server*>::iterator it = servers.begin(); it != servers.end(); it++)
		addFd(it->second->getSocket().getFd(), EPOLLIN);
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

int	Epoll::getReadyFd(int i) const
{
	return _events[i].data.fd;
}

int	Epoll::getTimeoutFdsNb(void) const
{
	return _timeoutFds.size();
}

int	Epoll::getTimeoutFd(int i) const
{
	return _timeoutFds[i];
}

void	Epoll::addFd(int fd, int flags, int timeout)
{
	struct epoll_event event;

	event.events = flags;
	event.data.fd = fd;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &event) == -1)
		throw std::runtime_error("The server encountered an error");
	if (timeout != - 1)
		_addTimer(fd, timeout);
}

void	Epoll::_addTimer(int fd, int timeout)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL))
		throw std::runtime_error("The server encountered an error");
	tv.tv_sec+= timeout;
	_timeouts[fd] = tv;
}

void	Epoll::deleteFd(int fd)
{
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL);
	if (_timeouts.find(fd) != _timeouts.end())
		_timeouts.erase(fd);
}

void	Epoll::wait(void)
{
	_ReadyFdsNb = epoll_wait(_epollFd, _events, MAX_EVENTS, EPOLL_TIMEOUT);
	if (_ReadyFdsNb == -1)
		throw std::runtime_error("The server encountered an error");

	_timeoutFds.clear();
	_checkTimers();
	_updateTimers();
}

void	Epoll::_checkTimers(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL))
		throw std::runtime_error("The server encountered an error");
	for (std::map<int, struct timeval>::iterator it = _timeouts.begin(); it != _timeouts.end(); it++) {
		if (_isTimeout(tv, it->second))
			_timeoutFds.push_back(it->first);
	}
}

void	Epoll::_updateTimers(void)
{
	for (int i = 0; i < _ReadyFdsNb; i++) {
		if ((_events[i].events == EPOLLIN || _events[i].events == (EPOLLIN | EPOLLOUT))
				&& _timeouts.find(_events[i].data.fd) != _timeouts.end())
			_timeouts[_events[i].data.fd].tv_sec+= TIMEOUT;
	}

}

bool	Epoll::_isTimeout(struct timeval& tv, struct timeval& maxTv)
{
	return tv.tv_sec > maxTv.tv_sec || (tv.tv_sec == maxTv.tv_sec && tv.tv_usec > maxTv.tv_usec);
}


