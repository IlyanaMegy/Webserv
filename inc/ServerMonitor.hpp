#ifndef SERVERMONITOR_HPP
#define SERVERMONITOR_HPP

#include "ConfigParser.hpp"
#include "Server.hpp"

class ServerConf;
class Server;
class ConfigParser;
class ServerMonitor {
   private:
	ConfigParser data;
	std::vector<ServerConf *> _confs;
	std::map<int, Server*>	_servers;


	// std::map<int, ServerConf> _servers_lst;
   public:
	ServerMonitor(std::string configFile);
	~ServerMonitor(void);

	// void runServers(void);
	// std::vector<ServerConf *> &getServers() { return _servers; }

	std::map<int, Server*>&	getServers(void);

	Server*		findServer(uint16_t port);
	void addServerToList(Server *server);
};

#endif