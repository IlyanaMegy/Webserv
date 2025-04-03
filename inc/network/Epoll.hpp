#ifndef EPOLL_HPP
# define EPOLL_HPP

# include <unistd.h>
# include <sys/epoll.h>
# include <exception>
# include <iostream>
# include <map>
# include <vector>
# include <sys/time.h>

# define MAX_EVENTS 100
# define TIMEOUT 30

# define EPOLL_TIMEOUT 100

class	Server;

class Epoll
{
	private:
		int					_epollFd;

		int					_ReadyFdsNb;
		struct epoll_event	_events[MAX_EVENTS];

		std::map<int, struct timeval>	_timeouts;
		std::vector<int>				_timeoutFds;

		void	_addTimer(int fd, int timeout);
		void	_checkTimers(void);
		void	_updateTimers(void);

		static bool	_isTimeout(struct timeval& tv, struct timeval& maxTv);

	public:
		Epoll(std::map<int, Server*>& servers);
		~Epoll(void);

		int	getEpollFd(void) const;

		int	getEvent(int i) const;
		int	getReadyFdsNb(void) const;
		int	getReadyFd(int i) const;

		int	getTimeoutFdsNb(void) const;
		int	getTimeoutFd(int i) const;

		void	addFd(int fd, int flags, int timeout = -1);
		void	deleteFd(int fd);
		void	wait(void);
};

#endif
