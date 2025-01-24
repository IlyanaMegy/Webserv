#ifndef SERVERCONF_HPP
#define SERVERCONF_HPP


#include "../../Webserv.hpp"
#include "../Socket.hpp"

/**
 * @brief  Charge, check and valide the configuration file's data
 * @note   this class must read and extract data then store them
 */
class ServerConf {
   private:
	// std::string _configFile;
	// uint16_t _port;
	// in_addr_t _host;
	// std::string _server_name;
	// std::string _root;
	// unsigned long _client_max_body_size;
	// std::string _index;
	// bool _autoindex;
	// // std::map<short, std::string> _error_pages;
	// // std::vector<Location> _locations;
	// Socket _socket;
	// std::vector<ConfigBlock> _configBlocks;

	// void parseConfigFile(void);
	// void validateConfigFile(const ConfigBlock &block);

   public:
   ServerConf(void) {};
	// ServerConf(const std::string filePath);
	~ServerConf(void) {};

	// Retourne la valeur d'une directive
	// std::string getValue(const std::string &key) const;
	// const std::vector<ConfigBlock> &getConfigBlocks() const;
};

#endif
