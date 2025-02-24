#ifndef SERVER_HPP
#define SERVER_HPP

#include "../Webserv.hpp"
#include "Client.hpp"
#include "Epoll.hpp"
#include "Socket.hpp"
#include "config/ServerConf.hpp"

#define MAXCONNECT 30

class Server {
   private:
	uint16_t _port;
	Socket _socket;
	std::map<int, Client *> _clients;
	std::map<std::string, ServerConf *> _confs;

   public:
	Server(uint16_t port);
	~Server(void);

	Socket &getSocket(void);
	Client *getClient(int clientFd);

	void acceptClient(Epoll &epoll);
	void readFrom(int clientFd);
	void sendTo(int clientFd);
	void closeConnection(int clientFd);

	bool isConfigKnown(std::string serverName);

	// class SocketCreationErrException : public std::exception {
	// 	public:
	// 		virtual const char *what() const throw();
	// };
};

#endif
