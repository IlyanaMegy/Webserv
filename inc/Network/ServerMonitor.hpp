#ifndef SERVERMONITOR_HPP
#define SERVERMONITOR_HPP

#include "config/ConfigParser.hpp"

class ServerConf;
class ServerMonitor {
   private:
	std::vector<ServerConf> _servers;
	std::map<int, ServerConf> _servers_lst;

   public:
	ServerMonitor(void);
	~ServerMonitor(void);

	void setupServers(std::vector<ServerConf> serverConfig);
	void runServers(void);
};

#endif