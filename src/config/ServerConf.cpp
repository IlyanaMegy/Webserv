#include "../../inc/config/ServerConf.hpp"

#include "Request.hpp"
#include "ParserTools.hpp"

ServerConf::ServerConf() {
	_port = 0;
	_host = 0;
	_server_name = "";
	_root = "";
	_max_body_size = MAXBODYOCTETS;
	_index = "";
	_autoindex = false;
}

void ServerConf::setServerName(std::string server_name) {
	checkToken(server_name);
	if (!server_name.empty() && server_name[server_name.size() - 1] == ';')
		server_name.erase(server_name.size() - 1);
	_server_name = server_name;
}

void ServerConf::setHost(std::string param) {
	checkToken(param);
	if (param == "localhost" || param == "localhost;") param = "127.0.0.1";

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
	if (!autoindex.empty() && autoindex	[autoindex.size() - 1] == ';')
		autoindex.erase(autoindex.size() - 1);
	if (autoindex != "on" && autoindex != "off")
		throw std::runtime_error("Wrong syntax: autoindex");
	if (autoindex == "on") _autoindex = true;
}

void ServerConf::setLocation(std::string path,  std::vector<std::string> params)
{
	Location new_loca;
	std::vector<std::string> methods;
	bool flag_methods = false;
	bool flag_autoindex = false;
	bool flag_max_size = false;
	int valid;

	new_loca.setPath(path);
	for (size_t i = 0; i < params.size(); i++) {
		if (params[i] == "root" && (i + 1) < params.size()) {
			if (!new_loca.getRootLocation().empty())
				throw std::runtime_error("Root of location is duplicated");
			checkToken(params[++i]);
			if (getTypePath(params[i]) == 2)
				new_loca.setRootLocation(params[i]);
			else
				new_loca.setRootLocation(_root + params[i]);

		} else if ((params[i] == "allow_methods" || params[i] == "methods") &&
				   (i + 1) < params.size()) {
			if (flag_methods)
				throw std::runtime_error(
					"Allow_methods of location is duplicated");
			std::vector<std::string> methods;
			while (++i < params.size()) {
				if (findChar(params[i], ';') != -1) {
					checkToken(params[i]);
					if (!params[i].empty() && params[i][params[i].size() - 1] == ';')
						params[i].erase(params[i].size() - 1);
					methods.push_back(params[i]);
					break;
				} else {
					methods.push_back(params[i]);
					if (i + 1 >= params.size())
						throw std::runtime_error("Token is invalid");
				}
			}
			new_loca.setMethods(methods);
			flag_methods = true;
		} else if (params[i] == "autoindex" && (i + 1) < params.size()) {
			if (path == "/cgi-bin")
				throw std::runtime_error("params autoindex not allow for CGI");
			if (flag_autoindex)
				throw std::runtime_error("Autoindex of location is duplicated");
			checkToken(params[++i]);
			new_loca.setAutoindex(params[i]);
			flag_autoindex = true;
		} else if (params[i] == "index" && (i + 1) < params.size()) {
			if (!new_loca.getIndexLocation().empty())
				throw std::runtime_error("Index of location is duplicated");
			checkToken(params[++i]);
			new_loca.setIndexLocation(params[i]);
		} else if (params[i] == "return" && (i + 1) < params.size()) {
			if (path == "/cgi-bin")
				throw std::runtime_error("params return not allow for CGI");
			if (!new_loca.getReturn().empty())
				throw std::runtime_error("Return of location is duplicated");
			checkToken(params[++i]);
			new_loca.setReturn(params[i]);
		} else if (params[i] == "alias" && (i + 1) < params.size()) {
			if (path == "/cgi-bin")
				throw std::runtime_error("params alias not allow for CGI");
			if (!new_loca.getAlias().empty())
				throw std::runtime_error("Alias of location is duplicated");
			checkToken(params[++i]);
			new_loca.setAlias(params[i]);
		} else if (params[i] == "cgi_ext" && (i + 1) < params.size()) {
			std::vector<std::string> extension;
			while (++i < params.size()) {
				if (params[i].find(";") != std::string::npos) {
					checkToken(params[i]);
					extension.push_back(params[i]);
					break;
				} else {
					extension.push_back(params[i]);
					if (i + 1 >= params.size())
						throw std::runtime_error("Token is invalid");
				}
			}
			new_loca.setCgiExtension(extension);
		} else if (params[i] == "cgi_path" && (i + 1) < params.size()) {
			std::vector<std::string> path;
			while (++i < params.size()) {
				if (params[i].find(";") != std::string::npos) {
					checkToken(params[i]);
					path.push_back(params[i]);
					break;
				} else {
					path.push_back(params[i]);
					if (i + 1 >= params.size())
						throw std::runtime_error("Token is invalid");
				}
				if (params[i].find("/python") == std::string::npos &&
					params[i].find("/bash") == std::string::npos)
					throw std::runtime_error("cgi_path is invalid");
			}
			new_loca.setCgiPath(path);
		} else if (params[i] == "client_max_body_size" &&
				   (i + 1) < params.size()) {
			if (flag_max_size)
				throw std::runtime_error(
					"Maxbody_size of location is duplicated");
			checkToken(params[++i]);
			new_loca.setMaxBodySize(params[i]);
			flag_max_size = true;
		} else if (i < params.size())
			throw std::runtime_error("params in a location is invalid");
	}
	if (new_loca.getPath() != "/cgi-bin" && new_loca.getIndexLocation().empty())
		new_loca.setIndexLocation(_index);
	if (!flag_max_size) new_loca.setMaxBodySize(_max_body_size);
	valid = isValidLocation(new_loca);
	if (valid == 1)
		throw std::runtime_error("Failed CGI validation");
	else if (valid == 2)
		throw std::runtime_error("Failed path in locaition validation");
	else if (valid == 3)
		throw std::runtime_error(
			"Failed redirection file in locaition validation");
	else if (valid == 4)
		throw std::runtime_error("Failed alias file in locaition validation");
	if (checkLocations()) throw std::runtime_error("Location is duplicated");
	_locations.push_back(new_loca);
}

int ServerConf::isValidLocation(Location &location) const {
	if (location.getPath() == "/cgi-bin") {
		if (location.getCgiPath().empty() || location.getCgiExtension().empty() || location.getIndexLocation().empty())
			return (1);
		if (checkFile(location.getIndexLocation(), 4) < 0) {
			std::string path = location.getRootLocation() + location.getPath() + "/" + location.getIndexLocation();
			if (getTypePath(path) != 1) {
				std::string root = getcwd(NULL, 0);
				location.setRootLocation(root);
				path = root + location.getPath() + "/" + location.getIndexLocation();
			}
			if (path.empty() || getTypePath(path) != 1 || checkFile(path, 4) < 0)
				return (1);
		}
		if (location.getCgiPath().size() != location.getCgiExtension().size())
			return (1);
		std::vector<std::string>::const_iterator it;
		for (it = location.getCgiPath().begin(); it != location.getCgiPath().end(); ++it)
			if (getTypePath(*it) < 0)
				return (1);
		std::vector<std::string>::const_iterator it_path;
		for (it = location.getCgiExtension().begin(); it != location.getCgiExtension().end(); ++it) {
			std::string tmp = *it;
			if (tmp != ".py" && tmp != ".sh" && tmp != "*.py" && tmp != "*.sh")
				return (1);
			for (it_path = location.getCgiPath().begin();  it_path != location.getCgiPath().end(); ++it_path) {
				std::string tmp_path = *it_path;
				if (tmp == ".py" || tmp == "*.py") {
					if (tmp_path.find("python") != std::string::npos)
						location._extPath.insert(std::make_pair(".py", tmp_path));
				} else if (tmp == ".sh" || tmp == "*.sh")
					if (tmp_path.find("bash") != std::string::npos)
						location._extPath[".sh"] = tmp_path;
			}
		}
		if (location.getCgiPath().size() != location.getExtensionPath().size())
			return (1);
	} else {
		if (location.getPath()[0] != '/') return (2);
		if (location.getRootLocation().empty() && _root.empty())
			location.setRootLocation(getcwd(NULL, 0));
		if (location.getRootLocation().empty() && !_root.empty())
			location.setRootLocation(_root);
		if (isFileExistAndReadable( location.getRootLocation() + location.getPath(), location.getIndexLocation()))
			return (5);
		if (!location.getReturn().empty())
			if (isFileExistAndReadable(location.getRootLocation(), location.getReturn()))
				return (3);
		if (!location.getAlias().empty())
			if (isFileExistAndReadable(location.getRootLocation(), location.getAlias()))
				return (4);
	}
	return (0);
}

bool ServerConf::checkLocations() const {
	if (_locations.size() < 2) return (false);
	std::vector<Location>::const_iterator first;
	std::vector<Location>::const_iterator second;
	for (first = _locations.begin(); first != _locations.end() - 1; first++)
		for (second = first + 1; second != _locations.end(); second++)
			if (first->getPath() == second->getPath())
				return (true);
	return (false);
}

std::string ServerConf::getServerName() const { return (_server_name); }
unsigned int ServerConf::getPort() const { return (_port); }
in_addr_t ServerConf::getHost() const { return (_host); }
unsigned int ServerConf::getMaxBodySize() const { return (_max_body_size); }
std::vector<Location> ServerConf::getLocations() const { return (_locations); }
std::string ServerConf::getRoot() const { return (_root); }
std::string ServerConf::getIndex() const { return (_index); }
bool ServerConf::getAutoindex() const { return (_autoindex); }

const std::vector<Location>::iterator ServerConf::getLocationFromUri(std::string uri) {
	std::vector<Location>::iterator it;
	for (it = _locations.begin(); it != _locations.end(); it++) {
		if (it->getPath() == uri) return (it);
	}
	throw std::runtime_error("Error: path to location not found");
}

bool ServerConf::isValidMethod(std::string uri, Request::Method method) {
	std::vector<Request::Method> locationMethods = getLocationFromUri(uri)->getMethods();

	if (std::find(locationMethods.begin(), locationMethods.end(), method) !=
		locationMethods.end())
		return (true);
	return (false);
}

void ServerConf::listMethods() const {
	for (std::vector<Location>::const_iterator it = _locations.begin(); it != _locations.end(); ++it) {
		std::cout << LIME "\nLocation: " << it->getPath() << std::endl;
		std::vector<Request::Method> methods = it->getMethods();
		std::cout << "Available methods: ";
		for (std::vector<Request::Method>::const_iterator method_it = methods.begin(); method_it != methods.end(); ++method_it)
			std::cout << *method_it << " ";
		std::cout << RESET << std::endl;
	}
}

void ServerConf::addRootToLocations(std::string root) {
	std::vector<Location>::iterator it;
	for (it = _locations.begin(); it != _locations.end(); it++)
		if (it->getRootLocation().empty())
			it->setRootLocation(root);
	is_setted_loca_root = 1;
}

// std::string ServerConf::getCompletePath(std::string path) {
// 	if (path.empty()) return (_root);
//     if (path[0] != '/') {
//         throw std::runtime_error("Invalid path: must start with '/'");
//     }
//     std::string longest_match;
//     std::string matched_root;
//     for (std::vector<Location>::iterator it = _locations.begin(); it != _locations.end(); ++it) {
//         if (path.find(it->getPath()) == 0 && it->getPath().length() > longest_match.length()) {
//             longest_match = it->getPath();
//             matched_root = it->getRootLocation();
//         }
//     }
// 	std::cout << RED << "here " << matched_root + path.substr(longest_match.length()) << RESET << std::endl;
//     if (!longest_match.empty()) {
//         if (*--matched_root.end() == '/')
//             return matched_root + path.substr(longest_match.length());
//         else
//             return matched_root + "/" + path.substr(longest_match.length());
//     }
// 	std::cout << "hzeeeeere" << std::endl;
//     std::string root_path = _root;
//     if (*(root_path.end() - 1) == '/')
//         root_path.erase(root_path.end() - 1);
//     if (path.find(root_path) == 1) // Check if path starts with root_path
//     	return _root + path.substr(root_path.length() + 1);
//     if (*(root_path.end() - 1) == '/')
//         return _root + path.substr(1);
//     else
//         return _root + path;
// }


// const std::string &ServerConfig::getPathErrorPage(short key)
// {
// 	std::map<short, std::string>::iterator it = _error_pages.find(key);
// 	if (it == _error_pages.end())
// 		throw std::runtime_error("Error_page does not exist");
// 	return (it->second);
// }

// /* find location by a name */ //do not using in parser, created for server
// manager const std::vector<Location>::iterator

size_t ServerConf::findMatchingLocation(const std::string& uri, const Location*& bestMatch) const {
    size_t bestMatchLength = 0;
    bestMatch = NULL;

    for (std::vector<Location>::const_iterator it = _locations.begin(); it != _locations.end(); ++it) {
        if (uri.find(it->getPath()) == 0 && it->getPath().length() > bestMatchLength) {
            bestMatch = &(*it);
            bestMatchLength = it->getPath().length();
        }
    }

    return bestMatchLength;
}


std::string ServerConf::getCompletePath(std::string uri) {
    const Location* location;
    size_t matchLength = findMatchingLocation(uri, location);
    std::string root;
    std::string relativePath;

    if (location) {
		std::cout << "\nYEAH\n" << std::endl;
        root = location->getRootLocation();
        relativePath = uri.substr(matchLength);
    } else {
        root = _root;
        relativePath = uri;
    }

    if (!root.empty() && root[root.length() - 1] == '/') {
        return root + relativePath;
    } else {
        return root + "/" + relativePath;
    }
}


// std::string ServerConf::getCompletePath(std::string uri)
// {
// 	// std::vector<Location>::iterator it;
// 	// for (it = _locations.begin(); it != _locations.end(); it++)
// 	// {
// 	// 	std::cout << BLUE << "\nkey = " << uri << "\npath = " << it->getPath() << "\nrootloca = " << it->getRootLocation() << RESET << std::endl;
// 	// 	if (it->getPath() == uri)
// 	// 	{
// 	// 		std::cout << RED << "\npath = " << it->getPath() << "\nrootloca = " << it->getRootLocation() << RESET << std::endl;
// 	// 		return (it->getRootLocation());}
// 	// }
// 	// throw std::runtime_error("Error: path to location not found");
// 	const Location* location; 
// 	size_t matchLength = findMatchingLocation(uri, location);
	
//     std::string root;
//     std::string relativePath;

// 	 if (location) {
//         root = location->getRootLocation();
// 		std::cout << BLUE << "\nroot = " << root << " uri = " << uri << RESET << std::endl;

//         relativePath = root.substr(matchLength);
// 		std::cout << BLUE << " relative = " << relativePath << RESET << std::endl;

//     } else {
//         root = _root;
//         relativePath = uri;
//     }

//     if (!root.empty() && root[root.length() - 1] == '/') {
//         return root + relativePath;
//     } else {
//         return root + "/" + relativePath;
//     }
// }

