#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <sys/socket.h>
# include <netinet/in.h>
# include <exception>

# include "Socket.hpp"
# include "Client.hpp"
# include "Epoll.hpp"
# include "config/ServerConf.hpp"

# define MAXCONNECT 30

class Server
{
	private:
		int						_port;
		Socket					_socket;
		std::map<int, Client*>	_clients;



	public:
		Server(int port);
		~Server(void);

		Socket	&getSocket(void);
		Client	*getClient(int clientFd);

		void	acceptClient(Epoll &epoll);
		void	readFrom(int clientFd);
		void	sendTo(int clientFd);
		void	closeConnection(int clientFd);
		
		bool	isConfigKnown(std::string serverName);

		std::map<std::string serverName, ServerConf*>	confs;
		// class SocketCreationErrException : public std::exception {
		// 	public:
		// 		virtual const char *what() const throw();
		// };
};

#endif
