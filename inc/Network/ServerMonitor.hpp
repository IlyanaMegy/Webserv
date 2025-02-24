#ifndef SERVERMONITOR_HPP
#define SERVERMONITOR_HPP

#include "config/ConfigParser.hpp"
#include "Server.hpp"
#include "../Webserv.hpp"

class ServerConf;
class Server;
class ServerMonitor {
   private:
	std::vector<ServerConf *> _confs;


	// std::map<int, ServerConf> _servers_lst;
   public:
	ServerMonitor(std::string configFile);
	~ServerMonitor(void);

	void setupServers(std::vector<ServerConf *> serverConfig);
	void runServers(void);
	std::vector<ServerConf *> &getServers() { return _servers; }

	Server*		findServer(int port);
	std::map<int, Server*>	servers;
	//returns NULL when didnt find server

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