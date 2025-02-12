#ifndef SERVERMONITOR_HPP
#define SERVERMONITOR_HPP

#include "config/ConfigParser.hpp"

class ServerConf;
class ServerMonitor {
	private:
		std::map<int, ServerConf> _servers;
	public:
		ServerMonitor(void);
		~ServerMonitor(void);

		void createServers(std::vector<ServerConf> servers);
};

#endif