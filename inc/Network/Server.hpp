#ifndef SERVER_HPP
#define SERVER_HPP

#include "config/ConfigParser.hpp"

class ServerConf;
/**
 * @brief  manage running server(s) with configuration
 * from config file
 * @note access to ServerConf servers + Client clients
 */
class Server {
   private:
	ServerConf		_serverConfig;
	// // std::map<int, ServerConf> _servers_map;
	// std::map<int, Client>	_clients;

   public:
   // Server(ServerConf config);
	Server(ServerConf config);
	~Server(void);

	Socket &getSocket(void);
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
