#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "ServerConf.hpp"

class ConfigParser {
   private:
	std::vector<ServerConf *> _servers;
	std::vector<std::string> _servers_config;
	size_t _nb_server;

   public:
	ConfigParser() : _nb_server(0) { (void)_nb_server; };
	~ConfigParser();

	int createCluster(const std::string &config_file);
	void splitBlocks(std::string &content);
	void createServer(std::string &config, ServerConf *server);

	int getServerFd(int serverId);
	std::vector<ServerConf *> &getServers() { return _servers; }
	ServerConf &getServerConfig(int serverId);
	uint16_t getServerPort(int serveurId);
	size_t getNbServer() const;
};

int isFileExistAndReadable(std::string const path, std::string const index);
int getTypePath(std::string const path);

#endif
