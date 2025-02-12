#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "ServerConf.hpp"

class ConfigParser {
   private:
	std::vector<ServerConf> _servers;
	std::vector<std::string> _servers_config;
	size_t _nb_server;

   public:
	ConfigParser() : _nb_server(0) { (void)_nb_server; };
	~ConfigParser() {};

	int createCluster(const std::string &config_file);
	void splitBlocks(std::string &content);
	void createServer(std::string &config, ServerConf &server);

	int getServerFd(int serverId);
	std::vector<ServerConf> &getServers() { return _servers; }
	ServerConf	&getServerConfig(int serverId);
	uint16_t	getServerPort(int serveurId);
	size_t	getNbServer() const;
	// void addDirective(const std::string &key, const std::string &value) {
	// 	_directives[key] = value;
	// }
	// void addMultiDirectives(const std::string &key,
	// 						const std::vector<std::string> &values) {
	// 	_multiDirectives[key] = values;
	// }
	// void addChild(const ConfigParser &child) { _children.push_back(child); }
	// bool hasDirective(const std::string &directive) const {
	// 	return _directives.find(directive) != _directives.end();
	// }
	// std::string getDirective(const std::string &key) const {
	// 	if (!hasDirective(key))
	// 		throw std::runtime_error("Directive " + key + " not found");
	// 	return _directives.at(key);
	// }
	// const std::vector<ConfigParser> &getChildren() const { return _children;
	// }
};

int isFileExistAndReadable(std::string const path, std::string const index);
int getTypePath(std::string const path);

#endif
