#include "../../inc/config/ServerConf.hpp"
#include "../../inc/messages/Request.hpp"
#include "../../inc/config/ParserTools.hpp"

ServerConf::ServerConf() {
	_port = 0;
	_host = 0;
	_server_name = "";
	_root = "";
	_max_body_size = MAXBODYOCTETS;
	_index = "";
	_autoindex = false;
	is_setted_loca_root = 0;
	initializeErrorPages();
}

void ServerConf::initializeErrorPages() {
    _error_pages["400"] = ERROR_DIR+std::string("/400.html");
    _error_pages["403"] = ERROR_DIR+std::string("/403.html");
    _error_pages["404"] = ERROR_DIR+std::string("/404.html");
    _error_pages["405"] = ERROR_DIR+std::string("/405.html");
    _error_pages["408"] = ERROR_DIR+std::string("/408.html");
    _error_pages["411"] = ERROR_DIR+std::string("/411.html");
    _error_pages["413"] = ERROR_DIR+std::string("/413.html");
    _error_pages["500"] = ERROR_DIR+std::string("/500.html");
    _error_pages["501"] = ERROR_DIR+std::string("/501.html");
    _error_pages["502"] = ERROR_DIR+std::string("/502.html");
    _error_pages["505"] = ERROR_DIR+std::string("/505.html");
}

void ServerConf::setServerName(std::string server_name) {
	_server_name = server_name;
}

void ServerConf::setRoot(std::string root) {
	if (root[root.size() - 1] == '/')
		root.erase(root.size() - 1);
	_root = root;
}

void ServerConf::setPort(std::string params) {
	unsigned int port = 0;
	size_t res = 0;
	std::string host;
	std::string port_str = params;

	if ((res = params.find(":")) != std::string::npos)
	{
		host = params.substr(0, res);
		port_str = params.substr(res + 1);
		if (host != "localhost" && host != "127.0.0.1")
			throw std::runtime_error("Wrong syntax: host");
	}
	for (size_t i = 0; i < port_str.length(); i++)
		if (!std::isdigit(port_str[i]))
			throw std::runtime_error("Wrong syntax: port");
	port = ft_stoi((port_str));
	if (port < 1 || port > 65636)
		throw std::runtime_error("Wrong syntax: port");
	_port = port;
}

void ServerConf::setClientMaxBodySize(std::string params) {
	for (size_t i = 0; i < params.length(); i++)
		if (params[i] < '0' || params[i] > '9')
			throw std::runtime_error("Wrong syntax: client_max_body_size");
	if (!ft_stoi(params))
		throw std::runtime_error("Wrong syntax: client_max_body_size");
	_max_body_size = ft_stoi(params);
}

void ServerConf::setDefaultIndex(std::string index) {
	_index = index;
}

void ServerConf::setAutoindex(std::string autoindex) {
	if (autoindex != "on" && autoindex != "off")
		throw std::runtime_error("Wrong syntax: autoindex");
	if (autoindex == "on") _autoindex = true;
}

void ServerConf::setDefaultRedirStatusCode(std::string code) {_redirStatusCode = code;};
void ServerConf::setDefaultRedirHostname(std::string hostname) {_redirHostname = hostname;};

void ServerConf::setErrorPage(std::string err_code, std::string err_page) { _error_pages[err_code] = err_page; }

void ServerConf::setLocation(std::string path,  std::vector<std::string> params, bool isTilde)
{
	Location new_loca(isTilde);
	std::vector<std::string> methods;
	bool flag_methods = false;
	bool flag_autoindex = false;
	bool flag_max_size = false;

	new_loca.setPath(path);
	for (size_t i = 0; i < params.size(); i++) {
		if (params[i] == "root" && (i + 1) < params.size()) {
			if (!new_loca.getRootLocation().empty())
				throw std::runtime_error("Root is duplicated");
			i++;
			checkToken(params[i]);
			params[i].erase(params[i].size() - 1);
			new_loca.setRootLocation(params[i]);
		} else if ((params[i] == "allow_methods" || params[i] == "methods") && (i + 1) < params.size()) {
			if (flag_methods)
				throw std::runtime_error("[CONFIG] Error : allow_methods of location is duplicated");
			std::vector<std::string> methods;
			while (++i < params.size()) {
				if (params[i].find(";") != std::string::npos) {
					checkToken(params[i]);
					params[i].erase(params[i].size() - 1);
					methods.push_back(params[i]);
					break;
				} else {
					methods.push_back(params[i]);
					if (i + 1 >= params.size())
						throw std::runtime_error("[CONFIG] Error : allow_methods is invalid");
				}
			}
			new_loca.setMethods(methods);
			flag_methods = true;
		} else if (params[i] == "autoindex" && (i + 1) < params.size()) {
			if (flag_autoindex)
				throw std::runtime_error("[CONFIG] Error : autoindex of location is duplicated");
			checkToken(params[++i]);
			params[i].erase(params[i].size() - 1);
			new_loca.setAutoindex(params[i]);
			flag_autoindex = true;
		} else if (params[i] == "index" && (i + 1) < params.size()) {
			if (!new_loca.getIndexLocation().empty())
				throw std::runtime_error("[CONFIG] Error : index of location is duplicated");
			checkToken(params[++i]);
			params[i].erase(params[i].size() - 1);
			new_loca.setIndexLocation(params[i]);
		} else if (params[i] == "cgi_path" && (i + 1) < params.size()) {
			i++;
			checkToken(params[i]);
			params[i].erase(params[i].size() - 1);
			new_loca.setCgiPath(params[i]);
			new_loca.setIsCgiLocation(true);
		} else if (params[i] == "client_max_body_size" && (i + 1) < params.size()) {
			if (flag_max_size)
				throw std::runtime_error("[CONFIG] Error : client_max_body_size is duplicated");
			checkToken(params[++i]);
			params[i].erase(params[i].size() - 1);
			new_loca.setMaxBodySize(params[i]);
			flag_max_size = true;
		} else if (params[i] == "return" && (i + 1) < params.size() && (i + 2) < params.size()) {
			std::string statusCode;
            std::string hostname;
			i++;

			if (i < params.size() && params[i] == "}")
				throw std::runtime_error("[CONFIG] Error: Invalid config");

			if (!(params[i] == "300" || params[i] == "301" || params[i] == "302" || params[i] == "303"
					|| params[i] == "304" || params[i] == "307" || params[i] == "308"))
				throw std::runtime_error("[CONFIG] Error: Invalid status code in return directive");
			statusCode = params[i];
			i++;

			if (i < params.size() && params[i] == "}")
				throw std::runtime_error("[CONFIG] Error: Invalid config");

			checkToken(params[i]);
			params[i].erase(params[i].size() - 1);
            hostname = params[i];

			new_loca.setRedirStatusCode(statusCode);
			new_loca.setRedirHostname(hostname);

		} else if (i < params.size())
			throw std::runtime_error("[CONFIG] Error : parameter in location is invalid : " + params[i]);
	}
	if (!flag_max_size) new_loca.setMaxBodySize(_max_body_size);
	if (!flag_methods) {
		std::vector<std::string> vector;
		vector.push_back("GET");
		vector.push_back("POST");
		vector.push_back("DELETE");
		new_loca.setMethods(vector);
	}
	if (checkLocationsDuplicate()) throw std::runtime_error("[CONFIG] Error : location is duplicated");
	_locations.push_back(new_loca);
}

std::string ServerConf::getServerName() const { return (_server_name); }
unsigned int ServerConf::getPort() const { return (_port); }
in_addr_t ServerConf::getHost() const { return (_host); }
unsigned int ServerConf::getMaxBodySize() const { return (_max_body_size); }
std::vector<Location> ServerConf::getLocations() const { return (_locations); }
std::string ServerConf::getRoot() const { return (_root); }
std::string ServerConf::getDefaultIndex() const { return (_index); }
bool ServerConf::getAutoindex() const { return (_autoindex); }
std::string ServerConf::getDefaultRedirStatusCode() const {return _redirStatusCode;}
std::string ServerConf::getDefaultRedirHostname() const {return _redirHostname;}

std::string ServerConf::getPathErrorPage(std::string statusCode) {
    std::map<std::string, std::string>::const_iterator it = _error_pages.find(statusCode);
    if (it != _error_pages.end())
		return (*it).second;
    else
        return "";
}

const std::vector<Location>::iterator ServerConf::getLocationFromUri(std::string uri) {
	std::vector<Location>::iterator it;

	for (it = _locations.begin(); it != _locations.end(); it++)
		if (it->getPath() == uri) return (it);
	throw std::runtime_error("Error: path to location not found");
}

bool ServerConf::isValidMethod(std::string path, Request::Method method) {
	Location									*location;
	std::map<std::string, Location*>::iterator	it;
	std::vector<Request::Method>				validMethods;

	it = _pathToLocation.find(path);
	if (it != _pathToLocation.end()) {
		if (!it->second)
			return true;
		validMethods = it->second->getMethods();
		return std::find(validMethods.begin(), validMethods.end(), method) != validMethods.end();
	}
	if (!(location = findMatchingLocation(path))) {
		_pathToLocation[path] = NULL;
		return true;
	}
	_pathToLocation[path] = location;
	validMethods = location->getMethods();
	return std::find(validMethods.begin(), validMethods.end(), method) != validMethods.end();
}

std::vector<Request::Method>	ServerConf::getValidMethods(std::string path)
{
	Location									*location;
	std::map<std::string, Location*>::iterator	it;
	std::vector<Request::Method>				defaultMethods;

	defaultMethods.push_back(Request::GET);
	defaultMethods.push_back(Request::POST);
	defaultMethods.push_back(Request::DELETE);

	it = _pathToLocation.find(path);
	if (it != _pathToLocation.end()) {
		if (!it->second)
			return defaultMethods;
		return it->second->getMethods();
	}
	if (!(location = findMatchingLocation(path))) {
		_pathToLocation[path] = NULL;
		return defaultMethods;
	}
	_pathToLocation[path] = location;
	return location->getMethods();
}

bool ServerConf::checkLocationsDuplicate() {
	std::vector<Location>::iterator first;
	std::vector<Location>::const_iterator second;

	if (_locations.size() < 2) return (false);
	for (first = _locations.begin(); first != _locations.end() - 1; first++)
		for (second = first + 1; second != _locations.end(); second++)
			if (first->getPath() == second->getPath())
				return (true);
	return (false);
}


void ServerConf::addRootToLocations(std::string root) {
	std::vector<Location>::iterator it;
	for (it = _locations.begin(); it != _locations.end(); it++)
		if (it->getRootLocation().empty())
			it->setRootLocation(root);
	is_setted_loca_root = 1;
}

void ServerConf::addIndexToLocations(std::string index) {
	std::vector<Location>::iterator it;
	for (it = _locations.begin(); it != _locations.end(); it++)
		if (it->getIndexLocation().empty())
			it->setIndexLocation(index);
}

void	ServerConf::addRedirToLocations(std::string statusCode, std::string hostname)
{
	std::vector<Location>::iterator it;
	for (it = _locations.begin(); it != _locations.end(); it++)
	if (!it->isRedirLocation()) {
			it->setRedirStatusCode(statusCode);
			it->setRedirHostname(hostname);
	}
}

Location* ServerConf::findMatchingLocation(const std::string& path) {
	Location	*location = NULL;
	size_t 		bestMatchLength = 0;
	size_t 		bestMatchLengthTilde = 0;

	for (std::vector<Location>::iterator it = _locations.begin(); it != _locations.end(); ++it) {
		if (it->getIsTilde() && (bestMatchLength == 0 || !it->getCgiPath().empty() || (location && location->getPath() == "/"))
				&& path.rfind(it->getPath()) == (path.length() - it->getPath().length())
				&& it->getPath().length() > bestMatchLengthTilde) {
			location = &(*it);
			bestMatchLengthTilde = it->getPath().length();
		}	
		if (!it->getIsTilde() && path.find(it->getPath()) == 0 && it->getPath().length() > bestMatchLength) {
			location = &(*it);
			bestMatchLength = it->getPath().length();
		}
	}
	return location;
}

bool	ServerConf::isAutoindexOn(std::string path)
{
	Location*									location;
	std::map<std::string, Location*>::iterator	it;

	it = _pathToLocation.find(path);
	if (it != _pathToLocation.end()) {
		if (!it->second)
			return _autoindex;
		return it->second->getAutoindex();
	}
	if (!(location = findMatchingLocation(path))) {
		_pathToLocation[path] = NULL;
		return _autoindex;
	}
	_pathToLocation[path] = location;
	return location->getAutoindex();
}

std::string	ServerConf::getCompletePath(std::string path)
{
	Location*									location;
	std::map<std::string, Location*>::iterator	it;

	it = _pathToLocation.find(path);
	if (it != _pathToLocation.end()) {
		if (!it->second)
			return _root+path;
		return it->second->getRootLocation()+path;
	}
	if (!(location = findMatchingLocation(path))) {
		_pathToLocation[path] = NULL;
		return _root+path;
	}
	_pathToLocation[path] = location;
	return location->getRootLocation()+path;
}

bool	ServerConf::isCgi(std::string path)
{
	Location*									location;
	std::map<std::string, Location*>::iterator	it;

	it = _pathToLocation.find(path);
	if (it != _pathToLocation.end()) {
		if (!it->second)
			return false;
		return it->second->isCgiLocation();
	}
	if (!(location = findMatchingLocation(path))) {
		_pathToLocation[path] = NULL;
		return false;
	}
	_pathToLocation[path] = location;
	return location->isCgiLocation();
}

std::string	ServerConf::getCgiExecutable(std::string path)
{
	Location*									location;
	std::map<std::string, Location*>::iterator	it;

	it = _pathToLocation.find(path);
	if (it != _pathToLocation.end()) {
		if (!it->second)
			return "";
		return it->second->getCgiPath();
	}
	if (!(location = findMatchingLocation(path))) {
		_pathToLocation[path] = NULL;
		return "";
	}
	_pathToLocation[path] = location;
	return location->getCgiPath();
}

bool	ServerConf::isRedir(std::string path)
{
	Location*									location;
	std::map<std::string, Location*>::iterator	it;

	it = _pathToLocation.find(path);
	if (it != _pathToLocation.end()) {
		if (!it->second)
			return (!_redirStatusCode.empty());
		return it->second->isRedirLocation();
	}

	if (!(location = findMatchingLocation(path))) {
		_pathToLocation[path] = NULL;
		return (!_redirStatusCode.empty());
	}
	_pathToLocation[path] = location;
	return location->isRedirLocation();
}

std::string	ServerConf::getRedirStatusCode(std::string path)
{
	Location*									location;
	std::map<std::string, Location*>::iterator	it;

	it = _pathToLocation.find(path);
	if (it != _pathToLocation.end()) {
		if (!it->second)
			return _redirStatusCode;
		return it->second->getRedirStatusCode();
	}

	if (!(location = findMatchingLocation(path))) {
		_pathToLocation[path] = NULL;
		return (_redirStatusCode);
	}
	_pathToLocation[path] = location;
	return location->getRedirStatusCode();
}

std::string	ServerConf::getRedirHostname(std::string path)
{
	Location*									location;
	std::map<std::string, Location*>::iterator	it;

	it = _pathToLocation.find(path);
	if (it != _pathToLocation.end()) {
		if (!it->second)
			return _redirHostname;
		return it->second->getRedirHostname();
	}

	if (!(location = findMatchingLocation(path))) {
		_pathToLocation[path] = NULL;
		return (_redirHostname);
	}
	_pathToLocation[path] = location;
	return location->getRedirHostname();
}

std::string	ServerConf::getIndex(std::string path)
{
	Location*									location;
	std::map<std::string, Location*>::iterator	it;

	it = _pathToLocation.find(path);
	if (it != _pathToLocation.end()) {
		if (!it->second)
			return _index;
		return it->second->getIndexLocation();
	}

	if (!(location = findMatchingLocation(path))) {
		_pathToLocation[path] = NULL;
		return (_index);
	}
	_pathToLocation[path] = location;
	return location->getIndexLocation();
}

std::string	ServerConf::getDefaultFile(std::string path)
{
	Location*									location;
	std::map<std::string, Location*>::iterator	it;

	it = _pathToLocation.find(path);
	if (it != _pathToLocation.end()) {
		if (!it->second)
			return _root+"/"+_index;
		return (it->second->getRootLocation() + "/" + it->second->getIndexLocation());
	}

	if (!(location = findMatchingLocation(path))) {
		_pathToLocation[path] = NULL;
		return (_root+"/"+_index);
	}
	_pathToLocation[path] = location;
	return location->getRootLocation()+"/"+location->getIndexLocation();
}

