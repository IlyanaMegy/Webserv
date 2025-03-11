#ifndef SOCKET_HPP
#define SOCKET_HPP

# include <sys/socket.h>
# include <netinet/in.h>
# include <fcntl.h>

class Socket {
	private:
		int					_opt;
		int					_fd;
		struct sockaddr_in	_addr;

		void	_setOpt(void);

	public:
		Socket(void);
		Socket(int port);
		~Socket(void);

		int					getFd(void) const;
		struct sockaddr_in	getAddr(void) const;

		void	fill(int fd, sockaddr_in addr);
		void	initSocket(int port);
};

#endif
