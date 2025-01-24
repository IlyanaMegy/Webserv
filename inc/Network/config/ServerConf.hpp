#ifndef SERVERCONF_HPP
#define SERVERCONF_HPP

#include "ConfigParser.hpp"
#include "../../Webserv.hpp"
#include "../Socket.hpp"

/**
 * @brief  Charge, check and valide the configuration file's data
 * @note   this class must read and extract data then store them
 */
class ServerConf {
   private:
	std::string _configFile;
	uint16_t _port;
	in_addr_t _host;
	std::string _server_name;
	std::string _root;
	unsigned long _client_max_body_size;
	std::string _index;
	bool _autoindex;
	// std::map<short, std::string> _error_pages;
	// std::vector<Location> _locations;
	Socket _socket;

   public:
	ServerConf(void);
	~ServerConf(void) {};

	void setServerName(std::string server_name);
	void setHost(std::string parametr);
	void setRoot(std::string root);
	void setPort(std::string parametr);
	void setClientMaxBodySize(std::string parametr);
	// void setErrorPages(std::vector<std::string> &parametr);
	void setIndex(std::string index);
	// void setLocation(std::string nameLocation, std::vector<std::string> parametr);
	void setAutoindex(std::string autoindex);

	// bool isValidErrorPages();
	// int isValidLocation(Location &location) const;

	const std::string &getServerName();
	const uint16_t &getPort();
	const in_addr_t &getHost();
	const size_t &getClientMaxBodySize();
	// const std::vector<Location> &getLocations();
	const std::string &getRoot();
	// const std::map<short, std::string> &getErrorPages();
	const std::string &getIndex();
	const bool &getAutoindex();
	// const std::string &getPathErrorPage(short key);
	// const std::vector<Location>::iterator getLocationKey(std::string key);

	// Retourne la valeur d'une directive
	// std::string getValue(const std::string &key) const;
	// const std::vector<ConfigBlock> &getConfigBlocks() const;
};

#endif
