#ifndef EPOLL_HPP
#define EPOLL_HPP

#include "../Webserv.hpp"

#define MAX_EVENTS 100

class Epoll {
   private:
	int _epollFd;
	int _ReadyFdsNb;
	struct epoll_event _events[MAX_EVENTS];

   public:
	Epoll(int serverFd);
	~Epoll(void);

	int getEpollFd(void) const;
	int getEvent(int i) const;
	int getReadyFd(void) const;
	int getFd(int i) const;

	void addFd(int fd, int flags);
	void wait(void);
};

#endif
