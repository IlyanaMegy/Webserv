#include "../../../inc/Network/config/Location.hpp"

Location::Location(void) {
	_path = "";
	_root = "";
	_autoindex = false;
	_index = "";
	_return = "";
	_alias = "";
	_clientMBS = MAX_CONTENT_LENGTH;
	_methods.reserve(5);
	_methods.push_back(1);
	for (size_t i = 0; i < 5; i++) _methods.push_back(0);
}

Location::~Location() {}

void Location::setPath(std::string param) { _path = param; }

void Location::setRootLocation(std::string param) {
	if (getTypePath(param) != 2)
		throw std::runtime_error("Error: Invalid IP address format for host");
	_root = param;
}

void Location::setIndexLocation(std::string param) { _index = param; }
void Location::setReturn(std::string param) { _return = param; }
void Location::setAlias(std::string param) { _alias = param; }

void Location::setAutoindex(std::string param) {
	if (param == "on" || param == "off")
		_autoindex = (param == "on");
	else
		throw std::runtime_error("Wrong autoindex");
}

void Location::setMethods(std::vector<std::string> methods) {
	std::fill(_methods.begin(), _methods.end(), 0);
	
	std::map<std::string, int> methodMap;
    methodMap["GET"] = 0;
    methodMap["POST"] = 1;
    methodMap["DELETE"] = 2;
    methodMap["PUT"] = 3;
    methodMap["HEAD"] = 4;

	for (size_t i = 0; i < methods.size(); i++) {
		if (methodMap.find(methods[i]) != methodMap.end())
			_methods[methodMap.at(methods[i])] = 1;
		else
			throw std::runtime_error("Method not supported: " + methods[i]);
	}
}

void Location::setCgiPath(std::vector<std::string> path) { _cgiPath = path; }
void Location::setCgiExtension(std::vector<std::string> extension) {
	_cgiExt = extension;
}

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

const std::string &Location::getPath() const {return _path; }
const std::string &Location::getRootLocation() const {return _root; }
const std::vector<short> &Location::getMethods() const {return _methods; }
const bool &Location::getAutoindex() const {return _autoindex; }
const std::string &Location::getIndexLocation() const {return _index; }
const std::string &Location::getReturn() const {return _return; }
const std::string &Location::getAlias() const {return _alias; }
const std::vector<std::string> &Location::getCgiPath() const {return _cgiPath; }
const std::vector<std::string> &Location::getCgiExtension() const {return _cgiExt; }
const std::map<std::string, std::string> &Location::getExtensionPath() const {return _extPath; }
const unsigned long &Location::getClientMBS() const {return _clientMBS; }
