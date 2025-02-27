#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <stdexcept>

# include "Socket.hpp"
# include "Request.hpp"

# define BUFFER_SIZE 42
class	Server;

class Client
{

	private:
		Request	*_request;
		Socket	_socket;
		bool	_shouldClose;

		Server*		_server;

	public:
		Client(void);
		Client(Server* _server, std::string leftoverMessage);
		~Client(void);


		std::string	leftoverMessage;

		Request	*getRequest(void);
		Socket	&getSocket(void);
		bool	getShouldClose(void) const;

		void	setShouldClose(bool shouldClose);

		void	createNewRequest(std::string leftoverMessage);
		void	deleteRequest(void);

};

#endif