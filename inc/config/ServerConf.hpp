#ifndef SERVERCONF_HPP
#define SERVERCONF_HPP

#include <arpa/inet.h>
#include <netinet/in.h>

#include <iostream>
#include <map>
#include <vector>

#include "../config/Location.hpp"
#include "Request.hpp"
#include "../style.hpp"

class ServerConf {
   private:
	std::string 		_configFile;
	unsigned int 		_port;
	in_addr_t 			_host;
	std::string 		_server_name;
	std::string 		_root;
	unsigned int 		_max_body_size;
	std::string 		_index;
	bool 				_autoindex;
	std::string			_redirStatusCode;
	std::string			_redirHostname;
	std::vector<Location> _locations;
	std::map<std::string, std::string> _error_pages;
	std::map<std::string, std::string> _cgiPathSaves;

   public:
	ServerConf(void);
	~ServerConf(void) {};

	void 							initializeErrorPages();

	void 							setServerName(std::string server_name);
	void 							setHost(std::string param);
	void 							setRoot(std::string root);
	void 							setPort(std::string params);
	void 							setClientMaxBodySize(std::string params);
	void 							setIndex(std::string index);
	void 							setLocation(std::string path, std::vector<std::string> params);
	void 							setAutoindex(std::string autoindex);
	void 							setRedirStatusCode(std::string code);
	void 							setRedirHostname(std::string hostname);
	void 							setErrorPage(std::string err_code, std::string err_page);

	std::string 					getServerName(void) const;
	unsigned int					getPort(void) const;
	in_addr_t 						getHost(void) const;
	unsigned int 					getMaxBodySize(void) const;
	std::vector<Location> 			getLocations(void) const;
	std::string 					getRoot(void) const;
	std::string 					getIndex(void) const;
	bool 							getAutoindex(void) const;
	std::string 					getRedirStatusCode() const;
	std::string 					getRedirHostname() const;
	
	const std::vector<Location>::iterator getLocationFromUri(std::string uri);

	bool 							is_setted_loca_root;

	bool 							isAutoindexOnInLocation(std::string path) const;
	bool							isValidMethod(std::string uri, Request::Method method);
	std::vector<std::string>		getValidMethod(std::string uri);
	int								isValidLocation(Location& location);
	// bool 						isLocationCgi(std::vector<Location>::iterator location) const;
	bool 							isCgi(std::string scriptPath);
	bool							checkLocationsDuplicate();

	void							addRootToLocations(std::string root);
	void 							addIndexToLocations(std::string index);
	void							addRedirToLocations(std::string statusCode, std::string hostname);

	size_t 							findMatchingLocation(const std::string& uri, Location* bestMatch);
	std::string 					getLocationCompletePath(std::string uri);
	std::string 					getIndexLocation(std::string uri);

	size_t							findMatchingCgiLocation(std::string scriptPath, Location* bestMatch);
	std::string 					getCgiCompletePath(std::string scriptPath);
	std::string 					getCgiPathForScript(std::string scriptPath);

	std::string 					getPathErrorPage(std::string key);
};

#endif
