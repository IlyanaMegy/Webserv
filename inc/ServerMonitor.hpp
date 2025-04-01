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

   public:
	ServerMonitor(std::string configFile);
	~ServerMonitor(void);

	std::map<int, Server*>&	getServers(void);

	Server*		findServer(uint16_t port);
	void addServerToList(Server *server);
};

#endif