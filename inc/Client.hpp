#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <stdexcept>

# include "Socket.hpp"
# include "Request.hpp"

# define BUFFER_SIZE 42

class Client
{

	private:
		Request _request;
		Socket	_socket;
		bool	_shouldClose;

	public:
		Client(void);
		Client(int serverFd);
		~Client(void);

		Request	&getRequest(void);
		Socket	&getSocket(void);
		bool	getShouldClose(void) const;

		void	setShouldClose(bool shouldClose);

};

#endif