#ifndef EPOLL_HPP
# define EPOLL_HPP

# include <unistd.h>
# include <sys/epoll.h>
# include <exception>
# include <iostream>

# include "Server.hpp"

# define MAX_EVENTS 100

class Epoll {
	private:
		// Server				_server;
		int                 _epollFd;
		int                 _ReadyFdsNb;
		struct epoll_event  _events[MAX_EVENTS];

	public:
		Epoll(int serverFd);
		~Epoll(void);

		int getEpollFd(void) const;
		int getEvent(int i) const;
		int getReadyFd(void) const;
		int getFd(int i) const;

		void addFd(int fd, int flags);
		void wait(void);

		// exceptions
};

#endif
