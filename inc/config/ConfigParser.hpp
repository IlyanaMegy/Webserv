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

	void createCluster(const std::string &config_file);
	void checkServersDuplicate();
	void splitBlocks(std::string &content);
	void createServer(std::string &config, ServerConf *server);

	std::vector<ServerConf *> &getServers() { return _servers; }
};

int isFileExistAndReadable(std::string const path, std::string const index);
int getTypePath(std::string const path);

#endif
