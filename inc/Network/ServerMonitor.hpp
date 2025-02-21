#ifndef SERVERMONITOR_HPP
#define SERVERMONITOR_HPP

#include "config/ConfigParser.hpp"

class ServerConf;
class ServerMonitor {
   private:
	std::vector<ServerConf *> _servers;
	std::map<int, ServerConf> _servers_lst;

   public:
	ServerMonitor(void);
	~ServerMonitor(void);

	void setupServers(std::vector<ServerConf *> serverConfig);
	void runServers(void);
	std::vector<ServerConf *> &getServers() { return _servers; }

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