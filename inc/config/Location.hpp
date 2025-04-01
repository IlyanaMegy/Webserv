#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <netinet/in.h>
#include <iostream>
#include <charconv>
#include <map>
#include <vector>

#include "ParserTools.hpp"
#include "../messages/Request.hpp"

class Location {
   public:
	
	std::map<std::string, std::string> _extPath;
	Location(void);
	Location(bool isTilde);
	~Location();

	void setPath(std::string param);
	void setRootLocation(std::string param);
	void setIndexLocation(std::string param);
	void setAutoindex(std::string param);
	void setMethods(std::vector<std::string> methods);
	void setCgiPath(std::string path);
	void setMaxBodySize(std::string param);
	void setMaxBodySize(unsigned long param);
	void setIsCgiLocation(bool isCgiLocation);
	void setRedirStatusCode(std::string code);
	void setRedirHostname(std::string hostname);

	std::string getPath() const;
	std::string getRootLocation() const;
	std::vector<Request::Method> getMethods() const;
	bool getAutoindex() const;
	std::string getIndexLocation() const;
	std::string getCgiPath() const;
	std::map<std::string, std::string> getExtensionPath() const;
	unsigned long getClientMBS() const;
	std::pair<int, std::string> getReturn() const;
	bool isCgiLocation() const;
	bool isRedirLocation() const;
	std::string getRedirStatusCode() const;
	std::string getRedirHostname() const;
	bool		getIsTilde() const;

   private:
   	bool		_isTilde;
	std::string _path;
	std::string _root;
	std::string _index;
	bool _autoindex;
	std::vector<Request::Method> _methods;
	std::string _cgiPath;
	unsigned long _clientMBS;
	bool _isCgiLocation;
	std::string					_redirStatusCode;
	std::string					_redirHostname;
};

#endif
