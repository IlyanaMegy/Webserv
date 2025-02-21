#ifndef SERVERCONF_HPP
#define SERVERCONF_HPP

#include "../../Webserv.hpp"
#include "../Socket.hpp"
#include "Location.hpp"

class Location;
/**
 * @brief  Charge, check and valide the configuration file's data
 * @note   this class must read and extract data then store them
 */
class ServerConf : public Socket{
   private:
	std::string _configFile;
	uint16_t _port;
	in_addr_t _host;
	std::string _server_name;
	std::string _root;
	unsigned int _max_body_size;
	std::string _index;
	bool _autoindex;
	// std::map<short, std::string> _error_pages;
	std::vector<Location> _locations;
	Socket _socket;
	int _socketFd;

   public:
	ServerConf(void);
	~ServerConf(void) {};

	void setServerName(std::string server_name);
	void setHost(std::string param);
	void setRoot(std::string root);
	void setPort(std::string params);
	void setClientMaxBodySize(std::string params);
	// void setErrorPages(std::vector<std::string> &params);
	void setIndex(std::string index);
	void setLocation(std::string path, std::vector<std::string> params);
	void setAutoindex(std::string autoindex);
	void setSocketServer(void);

	// bool isValidErrorPages(void);
	int isValidLocation(Location &location) const;
	bool checkLocations() const;


	// ! \ remove & from return type
	const std::string &getServerName(void);
	const uint16_t &getPort(void);
	const in_addr_t &getHost(void);
	unsigned int getMaxBodySize(void) const;
	const std::vector<Location> &getLocations(void);
	const std::string &getRoot(void);
	// const std::map<short, std::string> &getErrorPages(void);
	const std::string &getIndex(void);
	const bool &getAutoindex(void);
	int getSocketFd(void);
	Socket &getSocket(void);

	// const std::string &getPathErrorPage(short key);
	// const std::vector<Location>::iterator getLocationKey(std::string key);
};

#endif
