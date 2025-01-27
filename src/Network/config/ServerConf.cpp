#include "../../../inc/Network/config/ServerConf.hpp"

ServerConf::ServerConf() {
	_port = 0;
	_host = 0;
	_server_name = "";
	_root = "";
	_client_max_body_size = MAX_CONTENT_LENGTH;
	_index = "";
	_autoindex = false;
}

void ServerConf::setServerName(std::string server_name) {
	checkToken(server_name);
	_server_name = server_name;
}

/**
 * @brief  set host after binary conversion
 * @note   
 * @params  params: 
 * @retval None
 */
void ServerConf::setHost(std::string params) {
	checkToken(params);
	if (params == "localhost")
		params = "127.0.0.1";
	struct sockaddr_in hostBinary;
	if (!inet_pton(AF_INET, params.c_str(), &(hostBinary.sin_addr)))
		throw std::runtime_error("Error: Invalid IP address format for host");
	_host = hostBinary.sin_addr.s_addr;
}

/**
 * @brief  set root's absolute path
 * @retval None
 */
void ServerConf::setRoot(std::string root) {
	checkToken(root);
	if (getTypePath(root) == 2) {
		_root = root;
		return;
	}
	if (root.length() >= 2 && root[0] == '.' && root[1] == '/')
		root.erase(0, 1);
	else if (root[0] != '/')
		root = "/" + root;
	char dir[1024];
	getcwd(dir, 1024);
	std::string complete_root = dir + root;
	std::cout << "-> full root = " << complete_root << std::endl;
	if (getTypePath(complete_root) != 2)
		throw std::runtime_error("Error: Wrong syntax: root");
	_root = complete_root;
}

void ServerConf::setPort(std::string params) {unsigned int port;
	
	port = 0;
	checkToken(params);
	for (size_t i = 0; i < params.length(); i++)
		if (!std::isdigit(params[i]))
			throw std::runtime_error("Wrong syntax: port");
	port = ft_stoi((params));
	if (port < 1 || port > 65636)
		throw std::runtime_error("Wrong syntax: port");
	_port = (uint16_t) port;
}

/**
 * @brief  set max size of clients' body request
 * @params  `params`
 * @retval None
 */
void ServerConf::setClientMaxBodySize(std::string params) {checkToken(params);
	for (size_t i = 0; i < params.length(); i++)
		if (params[i] < '0' || params[i] > '9')
			throw std::runtime_error("Wrong syntax: client_max_body_size");

	if (!ft_stoi(params))
		throw std::runtime_error("Wrong syntax: client_max_body_size");
	_client_max_body_size = ft_stoi(params);
}

void ServerConf::setIndex(std::string index) {checkToken(index);
	_index = index;
}

void ServerConf::setAutoindex(std::string autoindex) {checkToken(autoindex);
	if (autoindex != "on" && autoindex != "off")
		throw std::runtime_error("Wrong syntax: autoindex");
	if (autoindex == "on")
		_autoindex = true;
}

void ServerConf::setLocation(std::string path, std::vector<std::string> params) {Location new_loca;
	std::vector<std::string> methods;
	bool flag_methods, flag_autoindex, flag_max_size = false;
	int valid;

	new_loca.setPath(path);
	for (size_t i = 0; i < params.size(); i++)
	{
		if (params[i] == "root" && (i + 1) < params.size())
		{
			if (!new_loca.getRootLocation().empty())
				throw std::runtime_error("Root of location is duplicated");
			checkToken(params[++i]);
			if (getTypePath(params[i]) == 2)
				new_loca.setRootLocation(params[i]);
			else
				new_loca.setRootLocation(_root + params[i]);
		}
		else if ((params[i] == "allow_methods" || params[i] == "methods") && (i + 1) < params.size())
		{
			if (flag_methods)
				throw std::runtime_error("Allow_methods of location is duplicated");
			std::vector<std::string> methods;
			while (++i < params.size())
			{
				if (params[i].find(';') != std::string::npos)
				{
					checkToken(params[i]);
					methods.push_back(params[i]);
					break;
				}
				else
				{
					methods.push_back(params[i]);
					if (i + 1 >= params.size()) throw std::runtime_error("Token is invalid");
				}
			}
			new_loca.setMethods(methods);
			flag_methods = true;
		}
		else if (params[i] == "autoindex" && (i + 1) < params.size())
		{
			if (path == "/cgi-bin")
					throw std::runtime_error("params autoindex not allow for CGI");
				if (flag_autoindex)
					throw std::runtime_error("Autoindex of location is duplicated");
				checkToken(params[++i]);
				new_loca.setAutoindex(params[i]);
				flag_autoindex = true;
			}
		else if (params[i] == "index" && (i + 1) < params.size())
		{
			if (!new_loca.getIndexLocation().empty()) throw std::runtime_error("Index of location is duplicated");
			checkToken(params[++i]);
			new_loca.setIndexLocation(params[i]);
		}
		else if (params[i] == "return" && (i + 1) < params.size())
		{
			if (path == "/cgi-bin") throw std::runtime_error("params return not allow for CGI");
			if (!new_loca.getReturn().empty()) throw std::runtime_error("Return of location is duplicated");
			checkToken(params[++i]);
			new_loca.setReturn(params[i]);
		}
		else if (params[i] == "alias" && (i + 1) < params.size())
		{
			if (path == "/cgi-bin") throw std::runtime_error("params alias not allow for CGI");
			if (!new_loca.getAlias().empty()) throw std::runtime_error("Alias of location is duplicated");
			checkToken(params[++i]);
			new_loca.setAlias(params[i]);
		}
		else if (params[i] == "cgi_ext" && (i + 1) < params.size())
		{
			std::vector<std::string> extension;
			while (++i < params.size())
			{
				if (params[i].find(";") != std::string::npos)
				{
					checkToken(params[i]);
					extension.push_back(params[i]);
					break ;
				}
				else
				{
					extension.push_back(params[i]);
					if (i + 1 >= params.size()) throw std::runtime_error("Token is invalid");
				}
			}
			new_loca.setCgiExtension(extension);
		}
		else if (params[i] == "cgi_path" && (i + 1) < params.size())
		{
			std::vector<std::string> path;
			while (++i < params.size())
			{
				if (params[i].find(";") != std::string::npos)
				{
					checkToken(params[i]);
					path.push_back(params[i]);
					break ;
				}
				else
				{
					path.push_back(params[i]);
					if (i + 1 >= params.size()) throw std::runtime_error("Token is invalid");
				}
				if (params[i].find("/python") == std::string::npos && params[i].find("/bash") == std::string::npos)
					throw std::runtime_error("cgi_path is invalid");
			}
			new_loca.setCgiPath(path);
		}
		else if (params[i] == "client_max_body_size" && (i + 1) < params.size())
		{
			if (flag_max_size) throw std::runtime_error("Maxbody_size of location is duplicated");
			checkToken(params[++i]);
			new_loca.setMaxBodySize(params[i]);
			flag_max_size = true;
		}
		else if (i < params.size()) throw std::runtime_error("params in a location is invalid");
	}
	if (new_loca.getPath() != "/cgi-bin" && new_loca.getIndexLocation().empty())
		new_loca.setIndexLocation(_index);
	if (!flag_max_size)
		new_loca.setMaxBodySize(_client_max_body_size);
	valid = isValidLocation(new_loca);
	if (valid == 1)
		throw std::runtime_error("Failed CGI validation");
	else if (valid == 2)
		throw std::runtime_error("Failed path in locaition validation");
	else if (valid == 3)
		throw std::runtime_error("Failed redirection file in locaition validation");
	else if (valid == 4)
		throw std::runtime_error("Failed alias file in locaition validation");
	_locations.push_back(new_loca);
}

int ServerConf::isValidLocation(Location &location) const
{
	if (location.getPath() == "/cgi-bin")
	{
		if (location.getCgiPath().empty() || location.getCgiExtension().empty() || location.getIndexLocation().empty()) return (1);

		if (checkFile(location.getIndexLocation(), 4) < 0)
		{
			std::string path = location.getRootLocation() + location.getPath() + "/" + location.getIndexLocation();
			if (getTypePath(path) != 1)
			{				
				std::string root = getcwd(NULL, 0);
				location.setRootLocation(root);
				path = root + location.getPath() + "/" + location.getIndexLocation();
			}
			if (path.empty() || getTypePath(path) != 1 || checkFile(path, 4) < 0) return (1);
		}
		if (location.getCgiPath().size() != location.getCgiExtension().size()) return (1);
		std::vector<std::string>::const_iterator it;
		for (it = location.getCgiPath().begin(); it != location.getCgiPath().end(); ++it)
			if (getTypePath(*it) < 0) return (1);
		std::vector<std::string>::const_iterator it_path;
		for (it = location.getCgiExtension().begin(); it != location.getCgiExtension().end(); ++it)
		{
			std::string tmp = *it;
			if (tmp != ".py" && tmp != ".sh" && tmp != "*.py" && tmp != "*.sh") return (1);
			for (it_path = location.getCgiPath().begin(); it_path != location.getCgiPath().end(); ++it_path)
			{
				std::string tmp_path = *it_path;
				if (tmp == ".py" || tmp == "*.py")
				{
					if (tmp_path.find("python") != std::string::npos)
						location._extPath.insert(std::make_pair(".py", tmp_path));
				}
				else if (tmp == ".sh" || tmp == "*.sh")
					if (tmp_path.find("bash") != std::string::npos)
						location._extPath[".sh"] = tmp_path;
			}
		}
		if (location.getCgiPath().size() != location.getExtensionPath().size()) return (1);
	}
	else
	{
		if (location.getPath()[0] != '/') return (2);
		if (location.getRootLocation().empty()) location.setRootLocation(_root);
		if (isFileExistAndReadable(location.getRootLocation() + location.getPath() + "/", location.getIndexLocation())) return (5);
		if (!location.getReturn().empty())
			if (isFileExistAndReadable(location.getRootLocation(), location.getReturn())) return (3);
		if (!location.getAlias().empty()) if (isFileExistAndReadable(location.getRootLocation(), location.getAlias())) return (4);
	}
	return (0);
}

const std::string &ServerConf::getServerName() {return (_server_name); }
const uint16_t &ServerConf::getPort() {return (_port); }
const in_addr_t &ServerConf::getHost() {return (_host); }
const size_t &ServerConf::getClientMaxBodySize() {return (_client_max_body_size); }
const std::vector<Location> &ServerConf::getLocations() {return (_locations); }
const std::string &ServerConf::getRoot() {return (_root); }
const std::string &ServerConf::getIndex() {return (_index); }
const bool &ServerConf::getAutoindex() {return (_autoindex); }
int	ServerConf::getFd() {return (_socket.getFd()); }

// const std::string &ServerConfig::getPathErrorPage(short key)
// {
// 	std::map<short, std::string>::iterator it = _error_pages.find(key);
// 	if (it == _error_pages.end())
// 		throw std::runtime_error("Error_page does not exist");
// 	return (it->second);
// }

// /* find location by a name */ //do not using in parser, created for server manager
// const std::vector<Location>::iterator ServerConfig::getLocationKey(std::string key)
// {
// 	std::vector<Location>::iterator it;
// 	for (it = _locations.begin(); it != _locations.end(); it++)
// 		if (it->getPath() == key) return (it);
// 	throw std::runtime_error("Error: path to location not found");
// }
