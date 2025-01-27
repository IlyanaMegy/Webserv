#include "../../../inc/Network/config/ConfigParser.hpp"

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
		this->_server_config.push_back(content.substr(start, end - start + 1));
		this->_nb_server++;
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
		throw std::runtime_error("Error: Invalid configuration file.");
	if (checkFile(config_file, 4) == -1)
		throw std::runtime_error("Error: Inaccessible configuration file.");
	std::string content = readFile(config_file);
	if (content.empty())
		throw std::runtime_error("Error: Empty configuration file.");
	std::cout << OLIV << "[CONFIG] First checks passed !" << RESET << std::endl;

	removeComments(content);
	removeWhiteSpace(content);
	std::cout << OLIV << "[CONFIG] Comments and whitespaces removed." << RESET << std::endl;

	splitBlocks(content);
	if (this->_server_config.size() != this->_nb_server)
		throw std::runtime_error("Error: Coudn't split config file.");
	std::cout << OLIV << "[CONFIG] Success spliting config file." << RESET << std::endl;
	
	for (size_t i = 0; i < this->_nb_server; i++)
	{
		ServerConf server;
		createServer(this->_server_config[i], server);
		this->_servers.push_back(server);
	}
	std::cout << OLIV << "[CONFIG] Created server(s)." << RESET << std::endl;
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
void ConfigParser::createServer(std::string &config, ServerConf &server)
{
	(void)server;
	std::vector<std::string>	parametrs;
	std::vector<std::string>	error_codes;
	Location loca;
	int		flag_loc = 1;
	bool	flag_autoindex = false;
	bool	flag_max_size = false;
	parametrs = splitParametrs(config += ' ', std::string(" \n\t;"));
	if (parametrs.size() < 3)
		throw std::runtime_error("Failed server validation");

	for (size_t i = 0; i < parametrs.size(); i++)
		std::cout << "parametre[" << SLVR << i << RESET << "] = " << parametrs[i] << std::endl;
	for (size_t i = 0; i < parametrs.size(); i++)
	{
		if (parametrs[i] == "listen" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (server.getPort())
				throw std::runtime_error("Port is duplicated");
			server.setPort(parametrs[++i]);
		}
		else if (parametrs[i] == "location" && (i + 1) < parametrs.size())
		{
			std::string	path;
			i++;
			if (parametrs[i] == "{" || parametrs[i] == "}")
				throw std::runtime_error("Wrong character in server scope{}");
			path = parametrs[i];
			std::vector<std::string> codes;
			if (parametrs[++i] != "{")
				throw std::runtime_error("Wrong character in server scope{}");
			i++;
			while (i < parametrs.size() && parametrs[i] != "}")
				codes.push_back(parametrs[i++]);
			server.setLocation(path, codes);
			if (i < parametrs.size() && parametrs[i] != "}")
				throw std::runtime_error("Wrong character in server scope{}");
			flag_loc = 0;
		}
		else if (parametrs[i] == "host" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (server.getHost())
				throw std::runtime_error("Host is duplicated");
			server.setHost(parametrs[++i]);
		}
		else if (parametrs[i] == "root" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (!server.getRoot().empty())
				throw std::runtime_error("Root is duplicated");
			server.setRoot(parametrs[++i]);
		}
		else if (parametrs[i] == "error_page" && (i + 1) < parametrs.size() && flag_loc)
		{
			while (++i < parametrs.size())
			{
				error_codes.push_back(parametrs[i]);
				if (parametrs[i].find(';') != std::string::npos)
					break ;
				if (i + 1 >= parametrs.size())
					throw std::runtime_error("Wrong character out of server scope{}");
			}
		}
		else if (parametrs[i] == "client_max_body_size" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (flag_max_size)
				throw std::runtime_error("Client_max_body_size is duplicated");
			server.setClientMaxBodySize(parametrs[++i]);
			flag_max_size = true;
		}
		else if (parametrs[i] == "server_name" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (!server.getServerName().empty())
				throw std::runtime_error("Server_name is duplicated");
			server.setServerName(parametrs[++i]);
		}
		else if (parametrs[i] == "index" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (!server.getIndex().empty())
				throw std::runtime_error("Index is duplicated");
			server.setIndex(parametrs[++i]);
		}
		else if (parametrs[i] == "autoindex" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (flag_autoindex)
				throw std::runtime_error("Autoindex of server is duplicated");
			server.setAutoindex(parametrs[++i]);
			flag_autoindex = true;
		}
		else if (parametrs[i] != "}" && parametrs[i] != "{")
		{
			if (!flag_loc)
				throw std::runtime_error("Parametrs after location");
			else
				throw std::runtime_error("Unsupported directive");
		}
	}
	if (server.getRoot().empty())
		server.setRoot("/;");
	if (server.getHost() == 0)
		server.setHost("localhost;");
	if (server.getIndex().empty())
		server.setIndex("index.html;");
	if (isFileExistAndReadable(server.getRoot(), server.getIndex()))
		throw std::runtime_error("Index from config file not found or unreadable");
	// if (server.checkLocaitons())
	// 	throw std::runtime_error("Locaition is duplicated");
	// if (!server.getPort())
	// 	throw std::runtime_error("Port not found");
	// server.setErrorPages(error_codes);
	// if (!server.isValidErrorPages())
	// 	throw std::runtime_error("Incorrect path for error page or number of error");
	// }
}

