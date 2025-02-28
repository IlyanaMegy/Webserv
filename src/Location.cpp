#include "Location.hpp"

#include "ParserTools.hpp"
#include "Request.hpp"

Location::Location(void) {
	_path = "";
	_root = "";
	_autoindex = false;
	_index = "";
	_return = "";
	_alias = "";
	_clientMBS = MAXBODYOCTETS;
}

Location::~Location() {}

void Location::setPath(std::string param) { _path = param; }

void Location::setRootLocation(std::string param) {
	if (getTypePath(param) != 2)
		throw std::runtime_error("Error: Invalid IP address format for host");
	if (!param.empty() && param[param.size() - 1] == ';')
        param.erase(param.size() - 1);
	if (!param.empty() && param[param.size() - 1] == '/')
        param.erase(param.size() - 1);
	_root = param;
}

void Location::setIndexLocation(std::string param)
{
	if (!param.empty() && param[param.size() - 1] == ';')
        param.erase(param.size() - 1);
	_index = param;
}

void Location::setReturn(std::string param) { _return = param; }
void Location::setAlias(std::string param) { _alias = param; }

void Location::setAutoindex(std::string param) {
	if (param == "on;" || param == "off;")
		_autoindex = (param == "on");
	else
		throw std::runtime_error("Wrong autoindex");
}

void Location::setMethods(std::vector<std::string> methods) {
	
	std::map<std::string, Request::Method> methodMap;
    methodMap["GET"] = Request::GET;
    methodMap["POST"] = Request::POST;
    methodMap["DELETE"] = Request::DELETE;

	for (size_t i = 0; i < methods.size(); i++) {
		if (methodMap.find(methods[i]) != methodMap.end())
			_methods.push_back(methodMap[methods[i]]);
		else
			throw std::runtime_error("Method not supported: " + methods[i]);
	}
}

void Location::setCgiPath(std::vector<std::string> path) { _cgiPath = path; }
void Location::setCgiExtension(std::vector<std::string> extension) { _cgiExt = extension; }

void Location::setMaxBodySize(std::string param) {
	unsigned long body_size = 0;

	for (size_t i = 0; i < param.length(); i++)
		if (param[i] < '0' || param[i] > '9')
			throw std::runtime_error("Wrong syntax: client_max_body_size");
	if (!ft_stoi(param))
		throw std::runtime_error("Wrong syntax: client_max_body_size");
	body_size = ft_stoi(param);
	this->_clientMBS = body_size;
}

void Location::setMaxBodySize(unsigned long param) { _clientMBS = param; }

std::string Location::getPath() const {return _path; }
std::string Location::getRootLocation() const {return _root; }
bool Location::getAutoindex() const {return _autoindex; }
std::string Location::getIndexLocation() const {return _index; }
std::string Location::getReturn() const {return _return; }
std::string Location::getAlias() const {return _alias; }
std::vector<std::string> Location::getCgiPath() const {return _cgiPath; }
std::vector<std::string> Location::getCgiExtension() const {return _cgiExt; }
std::map<std::string, std::string> Location::getExtensionPath() const {return _extPath; }
std::vector<Request::Method> Location::getMethods() const {return _methods; }
unsigned long Location::getClientMBS() const {return _clientMBS; }

