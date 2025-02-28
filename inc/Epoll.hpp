#ifndef EPOLL_HPP
# define EPOLL_HPP

# include <unistd.h>
# include <sys/epoll.h>
# include <exception>
# include <iostream>
# include <map>

# define MAX_EVENTS 100

class	Server;

class Epoll
{
	private:
		int					_epollFd;
		int					_ReadyFdsNb;
		struct epoll_event	_events[MAX_EVENTS];

	public:
		Epoll(std::map<int, Server*>& servers);
		~Epoll(void);

		int	getEpollFd(void) const;
		int	getEvent(int i) const;
		int	getReadyFdsNb(void) const;
		int	getFd(int i) const;

		void	addFd(int fd, int flags);
		void	deleteFd(int fd);
		void	wait(void);
};

#endif
