#ifndef CLIENT_HPP
#define CLIENT_HPP

# include <stdexcept>
# include <arpa/inet.h>


# include "Socket.hpp"
# include "Request.hpp"

# define BUFFER_SIZE 1024

class	Server;
class	Epoll;

class Client
{

	private:
		Request*_request;
		Socket	_socket;
		bool	_shouldClose;
		Server*	_server;


	public:
		Client(void);
		Client(Server* _server);
		~Client(void);

		std::string	leftoverMessage;

		Request	*getRequest(void);
		Socket	&getSocket(void);
		bool	getShouldClose(void) const;

		void	setShouldClose(bool shouldClose);

		void	createNewRequest(std::string leftoverMessage, Epoll* epoll);
		void	deleteRequest(void);

};

#endif