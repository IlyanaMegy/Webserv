#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "../Webserv.hpp"
# include "Socket.hpp"

class Client {

	private:
		char	_buffer[BUFFER_SIZE];
		// HTTPReq request;
		// HTTPRep reponse;
		Socket	_socket;

	public:
		Client(void);
		Client(int serverFd);
		Client(Client const &ref);
		~Client(void);

		Client	&operator=(Client const &ref);

		Socket	&getSocket(void);

		void	read(void);

};

#endif