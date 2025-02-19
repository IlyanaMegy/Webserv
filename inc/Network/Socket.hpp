#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "../Webserv.hpp"

class Socket
{
	private:
		int					_opt;
		int 				_fd;
		struct sockaddr_in	_addr;

		void	_setOpt(void);

	public:
		Socket(void);
		Socket(int port);
		Socket(Socket const &ref);
		~Socket(void);

		Socket	&operator=(const Socket &ref);

		void	setFd(int fd);
		int	getFd(void);

		void	fill(int fd, sockaddr_in addr);
};

#endif
