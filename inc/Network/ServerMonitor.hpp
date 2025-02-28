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
	std::map<unsigned int, Server*>	_servers;


	// std::map<int, ServerConf> _servers_lst;
   public:
	ServerMonitor(std::string configFile);
	~ServerMonitor(void);

	// void runServers(void);
	// std::vector<ServerConf *> &getServers() { return _servers; }

	std::map<unsigned int, Server*>&	getServers(void);

	Server*		findServer(uint16_t port);
	void addServerToList(Server *server);
};

#endif