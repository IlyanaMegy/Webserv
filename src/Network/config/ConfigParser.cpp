#include "../../../inc/Network/config/ConfigParser.hpp"

ConfigParser::~ConfigParser()
{
	for (std::vector<ServerConf*>::iterator it = _servers.begin(); it != _servers.end(); it++)
		delete *it;
}

/**
 * @brief  Read file
 * @note   it reads a file..
 * @param  `path`
 * @retval file's content
 */
static std::string readFile(std::string path) {
	if (path.empty() || path.length() == 0) return (NULL);
	std::ifstream file(path.c_str());
	if (!file || !file.is_open()) return (NULL);
	std::stringstream stream_binding;
	stream_binding << file.rdbuf();
	return (stream_binding.str());
}

/**
 * @brief  Split content of each servers' block
 * @note	Each block start with a big 'server' loop
 * @param  `content`= config file
 * @retval None
 */
void ConfigParser::splitBlocks(std::string &content) {
	size_t start = 0;
	size_t end = 1;

	if (content.find("server", 0) == std::string::npos)
		throw std::runtime_error("Error: Couldn't find server");
	while (start != end && start < content.length()) {
		start = findStartServer(start, content);
		end = findEndServer(start, content);
		if (start == end || !(content[start] == '{' && content[end] == '}'))
			throw std::runtime_error("Error: Wrong server scope{}");
		_servers_config.push_back(content.substr(start, end - start + 1));
		_nb_server++;
		start = end + 1;
	}
}

/**
 * @brief  Check config file and then split servers into clusters,
 * @note   then create ServerConf objs
 * @param  `config_file`
 * @retval 0=all's good
 */
int ConfigParser::createCluster(const std::string &config_file) {
	if (getTypePath(config_file) != 1)
		throw std::runtime_error("Error: Invalid configuration file");
	if (checkFile(config_file, 4) == -1)
		throw std::runtime_error("Error: Inaccessible configuration file");
	std::string content = readFile(config_file);
	if (content.empty())
		throw std::runtime_error("Error: Empty configuration file");
	std::cout << MAGENTA << "[CONFIG] First checks passed !\n         Reading " << BOLD << config_file << RESET << std::endl;

	removeComments(content);
	removeWhiteSpace(content);
	std::cout << MAGENTA << "[CONFIG] Comments and whitespaces removed" << RESET << std::endl;

	splitBlocks(content);
	if (_servers_config.size() != _nb_server)
		throw std::runtime_error("Error: Coudn't split config file");
	std::cout << MAGENTA << "[CONFIG] Success spliting config file" << RESET << std::endl;
	
	for (size_t i = 0; i < _nb_server; i++)
	{
		ServerConf *server = new ServerConf();
		if (!server)
			throw std::runtime_error("Error: Couldn't create server");
		
		createServer(_servers_config[i], server);
		_servers.push_back(server);
	}
	if (_nb_server > 1)
		std::cout << MAGENTA << "[CONFIG] " << _nb_server << " servers detected" << RESET << std::endl;
	else
		std::cout << MAGENTA << "[CONFIG] " << _nb_server << " server detected" << RESET << std::endl;
	return 0;
}

std::vector<std::string> splitParametrs(std::string line, std::string sep)
{
	std::vector<std::string>	str;
	std::string::size_type		start, end;

	start = end = 0;
	while (1)
	{
		end = line.find_first_of(sep, start);
		if (end == std::string::npos)
			break;
		std::string tmp = line.substr(start, end - start);
		str.push_back(tmp);
		start = line.find_first_not_of(sep, end);
		if (start == std::string::npos)
			break;
	}
	return (str);
}

/**
 * @brief  Split the block into parameters, check each parameter,
 * @note   set server's data
 * @param  `config`= config file
 * @param  `server`
 * @retval None
 */
void ConfigParser::createServer(std::string &config, ServerConf *server)
{
	std::vector<std::string>	params;
	std::vector<std::string>	error_codes;
	Location loca;
	bool	flag_autoindex = false;
	bool	flag_max_size = false;

	params = splitParametrs(config += ' ', std::string(" \n\t;"));
	if (params.size() < 3)
		throw std::runtime_error("Failed server validation");

	for (size_t i = 0; i < params.size(); i++)
	{
		if (params[i] == "listen" && (i + 1) < params.size())
		{
			if (server->getPort())
				throw std::runtime_error("Port is duplicated");
			server->setPort(params[++i]);
		}
		else if (params[i] == "location" && (i + 1) < params.size())
		{
			std::string	path;
			i++;
			if (params[i] == "{" || params[i] == "}")
				throw std::runtime_error("Wrong character in server scope{}");
			path = params[i];
			std::vector<std::string> codes;
			if (params[++i] != "{")
				throw std::runtime_error("Wrong character in server scope{}");
			i++;
			while (i < params.size() && params[i] != "}")
				codes.push_back(params[i++]);
			server->setLocation(path, codes);
			if (i < params.size() && params[i] != "}")
				throw std::runtime_error("Wrong character in server scope{}");
		}
		else if (params[i] == "host" && (i + 1) < params.size())
		{
			if (server->getHost())
				throw std::runtime_error("Host is duplicated");
			server->setHost(params[++i]);
		}
		else if (params[i] == "root" && (i + 1) < params.size())
		{
			if (!server->getRoot().empty())
				throw std::runtime_error("Root is duplicated");
			server->setRoot(params[++i]);
		}
		else if (params[i] == "error_page" && (i + 1) < params.size())
		{
			while (++i < params.size())
			{
				error_codes.push_back(params[i]);
				if (params[i].find(';') != std::string::npos)
					break ;
				if (i + 1 >= params.size())
					throw std::runtime_error("Wrong character out of server scope{}");
			}
		}
		else if (params[i] == "client_max_body_size" && (i + 1) < params.size())
		{
			if (flag_max_size)
				throw std::runtime_error("Client_max_body_size is duplicated");
			server->setClientMaxBodySize(params[++i]);
			flag_max_size = true;
		}
		else if (params[i] == "server_name" && (i + 1) < params.size())
		{
			if (!server->getServerName().empty())
				throw std::runtime_error("Server_name is duplicated");
			server->setServerName(params[++i]);
		}
		else if (params[i] == "index" && (i + 1) < params.size())
		{
			if (!server->getIndex().empty())
				throw std::runtime_error("Index is duplicated");
			server->setIndex(params[++i]);
		}
		else if (params[i] == "autoindex" && (i + 1) < params.size())
		{
			if (flag_autoindex)
				throw std::runtime_error("Autoindex of server is duplicated");
			server->setAutoindex(params[++i]);
			flag_autoindex = true;
		}
		else if (params[i] != "}" && params[i] != "{")
		{
			// if (!flag_loca)
			// 	throw std::runtime_error("Parametrs after location");
			// else
			throw std::runtime_error("Unsupported directive");
		}
	}
	if (!server->getPort())
		throw std::runtime_error("Port not found");
	if (server->getRoot().empty())
		server->setRoot("/;");
	if (server->getHost() == 0)
		server->setHost("localhost;");
	if (server->getIndex().empty())
		server->setIndex("index.html;");
	if (isFileExistAndReadable(server->getRoot(), server->getIndex()))
		throw std::runtime_error("Index from config file not found or unreadable");
	if (server->checkLocations())
		throw std::runtime_error("Location is duplicated");

	// server->setErrorPages(error_codes);
	// if (!server->isValidErrorPages())
	// 	throw std::runtime_error("Incorrect path for error page or number of error");
}

int	ConfigParser::getServerFd(int serveurId) {
	return _servers[serveurId]->getSocketFd();
}

uint16_t	ConfigParser::getServerPort(int serveurId) {
	return _servers[serveurId]->getPort();
}

size_t	ConfigParser::getNbServer() const {
	return _nb_server;
}

ServerConf	&ConfigParser::getServerConfig(int serverId){ return *_servers[serverId]; }
