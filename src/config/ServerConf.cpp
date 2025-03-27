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
    _error_pages["400"] = "www/error/400.html";
    _error_pages["404"] = "www/error/404.html";
    _error_pages["405"] = "www/error/405.html";
    _error_pages["408"] = "www/error/408.html";
    _error_pages["411"] = "www/error/411.html";
    _error_pages["413"] = "www/error/413.html";
    _error_pages["500"] = "www/error/500.html";
    _error_pages["501"] = "www/error/501.html";
    _error_pages["502"] = "www/error/502.html";
    _error_pages["505"] = "www/error/505.html";
}

void ServerConf::setServerName(std::string server_name) {
	checkToken(server_name);
	if (!server_name.empty() && server_name[server_name.size() - 1] == ';')
		server_name.erase(server_name.size() - 1);
	_server_name = server_name;
}

void ServerConf::setHost(std::string param) {
	checkToken(param);
	if (!param.empty() && param[param.size() - 1] == ';')
		param.erase(param.size() - 1);
	if (param == "localhost") param = "127.0.0.1";
	struct sockaddr_in hostBinary;
	if (!(inet_pton(AF_INET, param.c_str(), &(hostBinary.sin_addr))))
		throw std::runtime_error("Wrong syntax: host");
	_host = inet_addr(param.data());
}

void ServerConf::setRoot(std::string root) {
	checkToken(root);
	if (!root.empty() && root[root.size() - 1] == ';')
		root.erase(root.size() - 1);
	if (getTypePath(root) == 2) {
		_root = root;
		return;
	}
	if (root.length() >= 2 && root[0] == '.' && root[1] == '/')
		root.erase(0, 1);
	else if (root[0] != '/')
		root = root + "/";
	char dir[1024];
	getcwd(dir, 1024);
	std::string complete_root = dir + root;
	if (getTypePath(complete_root) != 2)
		throw std::runtime_error("Error: Wrong syntax: root");
	_root = complete_root;
}

void ServerConf::setPort(std::string params) {
	unsigned int port = 0;

	checkToken(params);
	if (!params.empty() && params[params.size() - 1] == ';')
		params.erase(params.size() - 1);
	for (size_t i = 0; i < params.length(); i++)
		if (!std::isdigit(params[i]))
			throw std::runtime_error("Wrong syntax: port");
	port = ft_stoi((params));
	if (port < 1 || port > 65636)
		throw std::runtime_error("Wrong syntax: port");
	_port = port;
}

void ServerConf::setClientMaxBodySize(std::string params) {
	checkToken(params);
	if (!params.empty() && params[params.size() - 1] == ';')
		params.erase(params.size() - 1);
	for (size_t i = 0; i < params.length(); i++)
		if (params[i] < '0' || params[i] > '9')
			throw std::runtime_error("Wrong syntax: client_max_body_size");
	if (!ft_stoi(params))
		throw std::runtime_error("Wrong syntax: client_max_body_size");
	_max_body_size = ft_stoi(params);
}

void ServerConf::setIndex(std::string index) {
	checkToken(index);
	if (!index.empty() && index[index.size() - 1] == ';')
		index.erase(index.size() - 1);
	_index = index;
}

void ServerConf::setAutoindex(std::string autoindex) {
	checkToken(autoindex);
	if (!autoindex.empty() && autoindex[autoindex.size() - 1] == ';')
		autoindex.erase(autoindex.size() - 1);
	if (autoindex != "on" && autoindex != "off")
		throw std::runtime_error("Wrong syntax: autoindex");
	if (autoindex == "on") _autoindex = true;
}

void ServerConf::setErrorPage(std::string err_code, std::string err_page) { _error_pages[err_code] = err_page; }

void ServerConf::setLocation(std::string path,  std::vector<std::string> params)
{
	Location new_loca;
	std::vector<std::string> methods;
	bool flag_methods = false;
	bool flag_autoindex = false;
	bool flag_max_size = false;

	new_loca.setPath(path);
	for (size_t i = 0; i < params.size(); i++) {
		if (params[i] == "root" && (i + 1) < params.size()) {
			if (findChar(params[i+1], ';') < 1)
				throw std::runtime_error("[CONFIG] Error : Unsupported directive in location");
			if (!new_loca.getRootLocation().empty())
				throw std::runtime_error("[CONFIG] Error : root of location is duplicated");
			checkToken(params[++i]);
			if (getTypePath(params[i]) == 2)
				new_loca.setRootLocation(params[i]);
			else
				new_loca.setRootLocation(_root + params[i]);

		} else if ((params[i] == "allow_methods" || params[i] == "methods") && (i + 1) < params.size()) {
			if (flag_methods)
				throw std::runtime_error("[CONFIG] Error : allow_methods of location is duplicated");
			std::vector<std::string> methods;
			while (++i < params.size()) {
				if (findChar(params[i], ';') >= 1) {
					checkToken(params[i]);
					if (!params[i].empty() && params[i][params[i].size() - 1] == ';')
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
			if (findChar(params[i+1], ';') < 1)
				throw std::runtime_error("[CONFIG] Error : Unsupported directive in location");
			if (new_loca.isCgiLocation())
				throw std::runtime_error("[CONFIG] Error : autoindex not allow for CGI");
			if (flag_autoindex)
				throw std::runtime_error("[CONFIG] Error : autoindex of location is duplicated");
			checkToken(params[++i]);
			new_loca.setAutoindex(params[i]);
			flag_autoindex = true;
		} else if (params[i] == "index" && (i + 1) < params.size()) {
			if (findChar(params[i+1], ';') < 1)
				throw std::runtime_error("[CONFIG] Error : Unsupported directive in location");
			if (!new_loca.getIndexLocation().empty())
				throw std::runtime_error("[CONFIG] Error : index of location is duplicated");
			checkToken(params[++i]);
			new_loca.setIndexLocation(params[i]);
		} else if (params[i] == "cgi_ext" && (i + 1) < params.size()) {
			if (findChar(params[++i], ';') >= 1)
			{
				if (!params[i].empty() && params[i][params[i].size() - 1] == ';')
					params[i].erase(params[i].size() - 1);
				new_loca.setCgiExtension(params[i]);
			}
			else
				throw std::runtime_error("[CONFIG] Error : cgi_ext is invalid");
		} else if (params[i] == "cgi_path" && (i + 1) < params.size()) {
			if (findChar(params[++i], ';') >= 1) {
				checkToken(params[i]);
				if (!params[i].empty() && params[i][params[i].size() - 1] == ';')
					params[i].erase(params[i].size() - 1);
				if (params[i].find("/python") == std::string::npos && params[i].find("/bash") == std::string::npos)
					throw std::runtime_error("[CONFIG] Error : cgi_path is invalid");
			}
			else
				throw std::runtime_error("[CONFIG] Error : cgi_path is invalid" + params[i]);
			new_loca.setCgiPath(params[i]);
			new_loca.setIsCgiLocation(true);
		} else if (params[i] == "client_max_body_size" && (i + 1) < params.size()) {
			if (findChar(params[i+1], ';') < 1)
				throw std::runtime_error("[CONFIG] Error : Unsupported directive in location");
			if (flag_max_size)
				throw std::runtime_error("[CONFIG] Error : client_max_body_size is duplicated");
			checkToken(params[++i]);
			new_loca.setMaxBodySize(params[i]);
			flag_max_size = true;
		} else if (params[i] == "return" && (i + 1) < params.size()) {
			int statusCode = 0;
            std::string uri;

            if (std::isdigit(params[i + 1][0])) {
				i++;
				if (!params[i].empty() && params[i][params[i].size() - 1] == ';')
					params[i].erase(params[i].size() - 1);
                statusCode = ft_stoi(params[i]);
            } else
                throw std::runtime_error("[CONFIG] Error: Invalid status code in return directive");

            if ((i + 1) < params.size() && params[i + 1] != "}") {
				i++;
				if (!params[i].empty() && params[i][params[i].size() - 1] == ';')
					params[i].erase(params[i].size() - 1);
                uri = params[i];
            }
            new_loca.setReturn(statusCode, uri);
		} else if (i < params.size())
			throw std::runtime_error("[CONFIG] Error : parameter in location is invalid : " + params[i]);
	}
	if (!flag_max_size) new_loca.setMaxBodySize(_max_body_size);
	// if (!flag_methods) new_loca.setMethods(std::vector<std::string>({"GET", "POST", "DELETE"}));
	if (isValidLocation(new_loca)) throw std::runtime_error("No extension found in path after '~ '");
	if (checkLocationsDuplicate()) throw std::runtime_error("[CONFIG] Error : location is duplicated");
	_locations.push_back(new_loca);
}

std::string ServerConf::getServerName() const { return (_server_name); }
unsigned int ServerConf::getPort() const { return (_port); }
in_addr_t ServerConf::getHost() const { return (_host); }
unsigned int ServerConf::getMaxBodySize() const { return (_max_body_size); }
std::vector<Location> ServerConf::getLocations() const { return (_locations); }
std::string ServerConf::getRoot() const { return (_root); }
std::string ServerConf::getIndex() const { return (_index); }
bool ServerConf::getAutoindex() const { return (_autoindex); }

std::string ServerConf::getPathErrorPage(std::string statusCode) {
    std::map<std::string, std::string>::const_iterator it = _error_pages.find(statusCode);
    if (it != _error_pages.end())
		return (*it).second;
    else
        throw std::runtime_error("Error page not found for status code: " + statusCode);
}

const std::vector<Location>::iterator ServerConf::getLocationFromUri(std::string uri) {
	std::vector<Location>::iterator it;

	for (it = _locations.begin(); it != _locations.end(); it++)
		if (it->getPath() == uri) return (it);
	throw std::runtime_error("Error: path to location not found");
}

bool ServerConf::isValidMethod(std::string uri, Request::Method method) {
	Location	location;
	findMatchingLocation(uri, &location);
	std::vector<Request::Method> locationMethods = location.getMethods();

	if (locationMethods.empty())
		return (true);
	if (std::find(locationMethods.begin(), locationMethods.end(), method) != locationMethods.end())
		return (true);
	return (false);
}

std::vector<std::string> ServerConf::getValidMethod(std::string uri) {
	Location	location;
	findMatchingLocation(uri, &location);
	std::vector<Request::Method> locationMethods = location.getMethods();
	std::vector<std::string> validMethods;

    if (locationMethods.empty())
	{
		validMethods.push_back("GET");
		validMethods.push_back("POST");
		validMethods.push_back("DELETE");
	}
    else
	{
		for (std::vector<Request::Method>::const_iterator it = locationMethods.begin(); it != locationMethods.end(); ++it) {
			switch (*it) {
				case Request::GET: validMethods.push_back("GET"); break;
				case Request::POST: validMethods.push_back("POST"); break;
				case Request::DELETE: validMethods.push_back("DELETE"); break;
				default: break;
			}
		}
	}
    return validMethods;
}

int ServerConf::isValidLocation(Location &location) {
	if (location.getPath().find("~ .") != std::string::npos && !(location.getCgiPath().empty()) && location.getCgiExtension().empty()) {
		size_t pos = location.getPath().rfind('.');
		if (pos != std::string::npos)
			location.setCgiExtension(location.getPath().substr(pos));
		else
			return (1);
	}
	else {
		if (location.getIndexLocation().empty())
			location.setIndexLocation(_index);
		if (location.getPath()[0] != '/') return (2);
		if (location.getRootLocation().empty() && _root.empty())
			is_setted_loca_root = 0;
		if (location.getRootLocation().empty() && !_root.empty())
			location.setRootLocation(_root);
	}
	if (location.isCgiLocation() && location.getCgiExtension().empty())
		throw std::runtime_error("No extension found for location: " + location.getPath());
	return (0);
}

// bool ServerConf::isLocationCgi(std::vector<Location>::iterator location) const { return (location->isCgiLocation()); }

bool ServerConf::isCgi(std::string scriptPath) {
	for (std::vector<Location>::iterator it = _locations.begin(); it != _locations.end(); it++)
		if (it->isCgiLocation() && it->getCgiExtension() == scriptPath.substr(scriptPath.rfind('.')))
			return (true);
	return (false);
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

size_t ServerConf::findMatchingLocation(const std::string& uri, Location* bestMatch) {
    size_t bestMatchLength = 0;

    for (std::vector<Location>::const_iterator it = _locations.begin(); it != _locations.end(); ++it) {
        if (uri.find(it->getPath()) == 0 && it->getPath().length() > bestMatchLength) {
            *bestMatch = (*it);
            bestMatchLength = it->getPath().length();
        }
    }
    return bestMatchLength;
}

std::string ServerConf::getIndexLocation(std::string uri) {
	Location location;
	size_t matchLength = findMatchingLocation(uri, &location);
	if (matchLength <= 0)
		return NULL;
	return location.getIndexLocation();
}

std::string ServerConf::getLocationCompletePath(std::string uri) {
    Location location;
    size_t matchLength = findMatchingLocation(uri, &location);
    std::string root;
    std::string relativePath;

    if (!location.getPath().empty()) {
		root = location.getRootLocation();
        relativePath = uri.substr(matchLength);
    } else {
		root = _root;
        relativePath = uri;
    }
	if (!root.empty() && root[root.length() - 1] == '/' && !relativePath.empty() && relativePath[0] == '/')
		return root + relativePath.erase(0, 1);
    else if (!root.empty() && root[root.length() - 1] != '/' && !relativePath.empty() && relativePath[0] != '/')
		return root + "/" + relativePath;
	else
        return root + relativePath;
}

size_t ServerConf::findMatchingCgiLocation(std::string scriptPath, Location* bestMatch) {
    size_t bestMatchLength = 0;
    std::string scriptExtension = scriptPath.substr(scriptPath.rfind('.'));
    for (std::vector<Location>::iterator it = _locations.begin(); it != _locations.end(); ++it) {
		if (it->isCgiLocation() && it->getCgiExtension() == scriptExtension) {
			std::string completePath = it->getRootLocation() + scriptPath;
			if (completePath.length() > bestMatchLength) {
				*bestMatch = (*it);
				bestMatchLength = completePath.length();
			}
        }
    }
    return bestMatchLength;
}

std::string ServerConf::getCgiCompletePath(std::string scriptPath) {
	Location location;

	size_t matchLength = findMatchingCgiLocation(scriptPath, &location);
	if (matchLength <= 0)
		throw std::runtime_error("[CONFIG] Error: CGI path not found for script path: " + scriptPath);				//!\ ERROR : CGI path not found for script path
	return location.getRootLocation() + scriptPath;
}

std::string ServerConf::getCgiPathForScript(std::string scriptPath) {
    Location location;

    if (_cgiPathSaves.find(scriptPath) != _cgiPathSaves.end()) 
		return _cgiPathSaves[scriptPath];
    size_t matchLength = findMatchingCgiLocation(scriptPath, &location);
    if (matchLength <= 0)
		throw std::runtime_error("[CONFIG] Error: CGI path not found for script path: " + scriptPath);				//!\ ERROR : CGI path not found for script path
	
	std::cout << GRY2 << "in getLocationCompletePath()\npath = " << location.getCgiPath() << RESET << std::endl;
	_cgiPathSaves[scriptPath] = location.getCgiPath();
	return location.getCgiPath();   
}

bool ServerConf::isAutoindexOnInLocation(std::string path) const {
    // Parcourir les blocs location pour trouver celui correspondant au chemin donné
    for (std::vector<Location>::const_iterator it = _locations.begin(); it != _locations.end(); ++it) {
        if (it->getPath() == path)
            return it->getAutoindex();
    }
	return false;
}
