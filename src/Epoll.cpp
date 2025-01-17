#include "../inc/Epoll.hpp"

Epoll::Epoll(int serverFd)
{
  _epollFd = epoll_create(1);
  if (_epollFd == -1)
    throw std::exception();

  try {
    addFd(serverFd, EPOLLIN);
  }
  catch (std::exception &e) {
    // epoll_ctl
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

int Epoll::getReadyFd(void) const
{
	return _ReadyFdsNb;
}

void Epoll::addFd(int fd, int flags)
{
  struct epoll_event event;

  event.events = flags;
  event.data.fd = fd;
  if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &event) == -1)
    throw std::exception();
}

void Epoll::wait(void)
{
  _ReadyFdsNb = epoll_wait(_epollFd, _events, MAX_EVENTS, -1);
  if (_ReadyFdsNb == -1)
    throw std::exception();
}
