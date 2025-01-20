#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <sys/socket.h>
# include <netinet/in.h>
# include <fcntl.h>

class Socket
{
	private:
		int _opt;
		int _fd;
		struct sockaddr_in _addr;

		void	_setOpt(void);

	public:
		Socket(void);
		Socket(int port);
		Socket(Socket const &ref);
		~Socket(void);

		Socket	&operator=(const Socket &ref);

		int getFd(void) const;

		void	fill(int fd, struct sockaddr_in addr);
};

#endif
