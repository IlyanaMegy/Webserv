#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <stdexcept>
# include <cstring>

# include "Socket.hpp"

# define BUFFER_SIZE 42

class Client
{

	private:
		// HTTPReq request;
		Socket	_socket;
		bool	_shouldClose;

	public:
		Client(void);
		Client(int serverFd);
		~Client(void);

		Socket	&getSocket(void);
		bool	getShouldClose(void) const;

		void	read(void);

};

#endif