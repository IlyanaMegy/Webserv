#ifndef SERVER_HPP
#define SERVER_HPP

#include "config/ServerConf.hpp"

/**
 * @brief  manage running server(s) with configuration
 * from config file
 * @note access to ServerConf servers + Client clients
 */
class Server {
   private:
	// int						_port;
	// std::vector<ServerConf> _servers;
	// // std::map<int, ServerConf> _servers_map;
	// std::map<int, Client>	_clients;

   public:
	Server(void) {};
	// Server(int port);
	~Server(void) {};

	// Socket &getSocket(void);
	// Client &getClient(int i);

	// void acceptClient(void);
	// void readFrom(int clientFd);
	// void sendTo(int clientFd);

	// class SocketCreationErrException : public std::exception {
	// 	public:
	// 		virtual const char *what() const throw();
	// };
};

#endif
