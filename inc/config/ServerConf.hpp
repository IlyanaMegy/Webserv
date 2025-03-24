#ifndef SERVERCONF_HPP
#define SERVERCONF_HPP

#include <arpa/inet.h>
#include <netinet/in.h>

#include <iostream>
#include <map>
#include <vector>

#include "Location.hpp"
#include "messages/Request.hpp"
#include "style.hpp"

class ServerConf {
   private:
	std::string _configFile;
	unsigned int _port;
	in_addr_t _host;
	std::string _server_name;
	std::string _root;
	unsigned int _max_body_size;
	std::string _index;
	bool _autoindex;
	std::vector<Location> _locations;
	std::map<std::string, std::string> _error_pages;

   public:
	ServerConf(void);
	~ServerConf(void) {};

	void initializeErrorPages();

	void setServerName(std::string server_name);
	void setHost(std::string param);
	void setRoot(std::string root);
	void setPort(std::string params);
	void setClientMaxBodySize(std::string params);
	void setIndex(std::string index);
	void setLocation(std::string path, std::vector<std::string> params);
	void setAutoindex(std::string autoindex);

	int isValidLocation(Location& location) const;
	bool checkLocations() const;

	std::string getServerName(void) const;
	unsigned int getPort(void) const;
	in_addr_t getHost(void) const;
	unsigned int getMaxBodySize(void) const;
	std::vector<Location> getLocations(void) const;
	std::string getRoot(void) const;
	std::string getIndex(void) const;
	bool getAutoindex(void) const;
	const std::vector<Location>::iterator getLocationFromUri(std::string uri);

	bool is_setted_loca_root;

	bool isValidMethod(std::string uri, Request::Method method);
	std::vector<Request::Method>	getValidMethods(std::string path);

	void addRootToLocations(std::string root);

	size_t findMatchingLocation(const std::string& uri,
								const Location*& bestMatch) const;
	std::string getCompletePath(std::string uri);

	// !TODO: Implement these functions
	void setErrorPages(std::vector<std::string>& params);
	bool isValidErrorPages(void);
	// const std::map<short, std::string> getErrorPages(void);
	std::string getPathErrorPage(std::string key);
};

#endif
