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

#define ERROR_DIR "www/site/error"

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
	std::map<std::string, Location*>	_pathToLocation;

   public:
	ServerConf(void);
	~ServerConf(void) {};

	void 							initializeErrorPages();

	void 							setServerName(std::string server_name);
	void 							setRoot(std::string root);
	void 							setPort(std::string params);
	void 							setClientMaxBodySize(std::string params);
	void 							setDefaultIndex(std::string index);
	void 							setLocation(std::string path, std::vector<std::string> params, bool isTilde);
	void 							setAutoindex(std::string autoindex);
	void 							setDefaultRedirStatusCode(std::string code);
	void 							setDefaultRedirHostname(std::string hostname);
	void 							setErrorPage(std::string err_code, std::string err_page);

	std::string 						getServerName(void) const;
	unsigned int						getPort(void) const;
	in_addr_t 							getHost(void) const;
	unsigned int 						getMaxBodySize(void) const;
	std::vector<Location> 				getLocations(void) const;
	std::string 						getRoot(void) const;
	std::string 						getDefaultIndex(void) const;
	bool 								getAutoindex(void) const;
	std::string 						getDefaultRedirStatusCode() const;
	std::string 						getDefaultRedirHostname() const;


	bool							isRedir(std::string path);
	std::string						getRedirStatusCode(std::string path);
	std::string						getRedirHostname(std::string path);
	std::string						getDefaultFile(std::string path);
	std::string						getIndex(std::string path);
	
	const std::vector<Location>::iterator getLocationFromUri(std::string uri);

	bool 							is_setted_loca_root;

	bool							isAutoindexOn(std::string path);
	bool							isValidMethod(std::string uri, Request::Method method);
	std::vector<Request::Method>	getValidMethods(std::string uri);
	bool 							isCgi(std::string scriptPath);
	bool							checkLocationsDuplicate();

	void							addRootToLocations(std::string root);
	void 							addIndexToLocations(std::string index);
	void							addRedirToLocations(std::string statusCode, std::string hostname);

	Location* 						findMatchingLocation(const std::string& path);
	std::string						getCompletePath(std::string path);

	std::string						getCgiExecutable(std::string path);

	std::string 					getPathErrorPage(std::string key);
};

#endif
