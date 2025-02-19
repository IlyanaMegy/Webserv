#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "../../Webserv.hpp"

class Location {
  

   public:
   enum Method {
		GET,
		POST,
		DELETE
   };
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

	const std::string &getPath() const;
	const std::string &getRootLocation() const;
	const std::vector<short> &getMethods() const;
	const bool &getAutoindex() const;
	const std::string &getIndexLocation() const;
	const std::string &getReturn() const;
	const std::string &getAlias() const;
	const std::vector<std::string> &getCgiPath() const;
	const std::vector<std::string> &getCgiExtension() const;
	const std::map<std::string, std::string> &getExtensionPath() const;
	const unsigned long &getClientMBS() const;

	private:
	std::string _path;
	std::string _root;
	std::string _index;
	std::string _return;
	std::string _alias;
	bool _autoindex;
	std::vector<Method> _methods;
	std::vector<std::string> _cgiPath;
	std::vector<std::string> _cgiExt;
	unsigned long _clientMBS;
};

#endif
