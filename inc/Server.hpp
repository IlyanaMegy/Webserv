#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <sys/socket.h>
# include <netinet/in.h>
# include <exception>

# include "Socket.hpp"
# include "Client.hpp"

# define MAXCONNECT 30

class Server {
	private:
		int _port;
		Socket	_socket;
		std::map<int, Client> _clients;	

	public:
		Server(int port);
		~Server(void);

		Socket &getSocket(void);
		Client  &getClient(int i);

		void acceptClient(void);
		void readFrom(int clientFd);
		void sendTo(int clientFd);
		
		class SocketCreationErrException : public std::exception {
			public:
				virtual const char *what() const throw();
		};
};

#endif
