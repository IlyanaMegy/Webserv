#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <netinet/in.h>

#include "Request.hpp"

class Location {
   public:
	
	std::map<std::string, std::string> _extPath;
	Location(void);
	~Location();

	void setPath(std::string param);
	void setRootLocation(std::string param);
	void setIndexLocation(std::string param);
	void setReturn(std::string param);
	void setAlias(std::string param);
	void setAutoindex(std::string param);
	void setMethods(std::vector<std::string> methods);
	void setCgiPath(std::vector<std::string> path);
	void setCgiExtension(std::vector<std::string> extension);
	void setMaxBodySize(std::string param);
	void setMaxBodySize(unsigned long param);

	std::string getPath() const;
	std::string getRootLocation() const;
	std::vector<Request::Method> getMethods() const;
	bool getAutoindex() const;
	std::string getIndexLocation() const;
	std::string getReturn() const;
	std::string getAlias() const;
	std::vector<std::string> getCgiPath() const;
	std::vector<std::string> getCgiExtension() const;
	std::map<std::string, std::string> getExtensionPath() const;
	unsigned long getClientMBS() const;

   private:
	// bool setted_root = 1;
	std::string _path;
	std::string _root;
	std::string _index;
	std::string _return;
	std::string _alias;
	bool _autoindex;
	std::vector<Request::Method> _methods;
	std::vector<std::string> _cgiPath;
	std::vector<std::string> _cgiExt;
	unsigned long _clientMBS;
};

#endif
