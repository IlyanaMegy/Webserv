#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <sys/socket.h>
# include <netinet/in.h>
# include <exception>

# include "Socket.hpp"
# include "Client.hpp"
# include "Epoll.hpp"

# define MAXCONNECT 30

class ServerConf;

class Server
{
	private:
		unsigned int						_port;
		Socket								_socket;
		std::map<int, Client*>				_clients;
		std::map<std::string, ServerConf *> _confs;
		ServerConf*							_defaultConf;


	public:
		Server(unsigned int port, ServerConf* defaultConf);
		~Server(void);

		unsigned int	getPort(void);
		Socket			&getSocket(void);
		Client			*getClient(int clientFd);

		void			acceptClient(Epoll &epoll);
		void			readFrom(int clientFd);
		void			sendTo(int clientFd);
		void			closeConnection(int clientFd);

		bool 			isConfigKnown(std::string serverName);
		bool			isClientKnown(int clientFd);
		void 			addConfig(ServerConf *conf);
		ServerConf* getDefaultConf() const;
	
		
		// class SocketCreationErrException : public std::exception {
		// 	public:
		// 		virtual const char *what() const throw();
		// };
};

#endif
