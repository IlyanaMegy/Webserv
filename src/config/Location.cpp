#include "../../inc/config/Location.hpp"

Location::Location(void) {}

Location::Location(bool isTilde) {
	_isTilde = isTilde;
	_path = "";
	_root = "";
	_autoindex = false;
	_index = "";
	_clientMBS = MAXBODYOCTETS;
	_isCgiLocation = false;
}
Location::~Location() {}

void Location::setPath(std::string param) { _path = param; }

void Location::setRootLocation(std::string param) {
	if (!param.empty() && param[param.size() - 1] == '/')
        param.erase(param.size() - 1);
	_root = param;
}

void Location::setIndexLocation(std::string param)
{
	_index = param;
}

void Location::setAutoindex(std::string param) {
	if (param == "on")
		_autoindex = true;
	else if (param == "off")
		_autoindex = false;
	else
		throw std::runtime_error("[CONFIG] Error : autoindex is invalid");
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
			throw std::runtime_error("[CONFIG] Error : method not supported: " + methods[i]);
	}
}

void Location::setCgiPath(std::string path) { _cgiPath = path; }

void Location::setMaxBodySize(std::string param) {
	unsigned long body_size = 0;

	for (size_t i = 0; i < param.length(); i++)
		if (param[i] < '0' || param[i] > '9')
			throw std::runtime_error("[CONFIG] Error : client_max_body_size is invalid");
	if (!ft_stoi(param))
		throw std::runtime_error("[CONFIG] Error : client_max_body_size is invalid");
	body_size = ft_stoi(param);
	this->_clientMBS = body_size;
}

void Location::setMaxBodySize(unsigned long param) { _clientMBS = param; }
void Location::setIsCgiLocation(bool isCgiLocation) { _isCgiLocation = isCgiLocation; }
void Location::setRedirStatusCode(std::string code) { _redirStatusCode = code; }
void Location::setRedirHostname(std::string hostname) { _redirHostname = hostname; }
std::string Location::getPath() const {return _path; }
std::string Location::getRootLocation() const {return _root; }
bool Location::getAutoindex() const {return _autoindex; }
std::string Location::getIndexLocation() const {return _index; }
std::string Location::getRedirStatusCode(void) const { return _redirStatusCode; }
std::string Location::getRedirHostname(void) const { return _redirHostname; }
std::string Location::getCgiPath() const {return _cgiPath; }
std::map<std::string, std::string> Location::getExtensionPath() const {return _extPath; }
std::vector<Request::Method> Location::getMethods() const {return _methods; }
unsigned long Location::getClientMBS() const {return _clientMBS; }
bool Location::getIsTilde() const {return _isTilde;}
bool Location::isCgiLocation() const {return _isCgiLocation; }
bool Location::isRedirLocation() const {return !_redirStatusCode.empty();}
