#include "ConfigParser.hpp"

#include "ParserTools.hpp"

ConfigParser::~ConfigParser()
{
	for (std::vector<ServerConf*>::iterator it = _servers.begin(); it != _servers.end(); it++)
		if (*it)
			delete *it;
}

static std::string readFile(std::string path) {
	if (path.empty() || path.length() == 0) return (NULL);
	std::ifstream file(path.c_str());
	if (!file || !file.is_open()) return (NULL);
	std::stringstream stream_binding;
	stream_binding << file.rdbuf();
	return (stream_binding.str());
}

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

void ConfigParser::createCluster(const std::string &config_file) {
	if (getTypePath(config_file) != 1)
		throw std::runtime_error("Error: Invalid configuration file");
	if (checkFile(config_file, 4) == -1)
		throw std::runtime_error("Error: Inaccessible configuration file");
	std::string content = readFile(config_file);
	if (content.empty())
		throw std::runtime_error("Error: Empty configuration file");
	std::cout << MAGENTA << "\n[CONFIG] Reading " << BOLD << config_file << RESET << std::endl << std::endl;

	removeComments(content);
	removeWhiteSpace(content);
	splitBlocks(content);
	if (_servers_config.size() != _nb_server)
		throw std::runtime_error("Error: Coudn't split config file");
	for (size_t i = 0; i < _nb_server; i++)
	{
		ServerConf *server = new ServerConf();
		if (!server)
			throw std::runtime_error("Error: Couldn't create server");

		_servers.push_back(server);
		createServer(_servers_config[i], server);
	}

	checkServersDuplicate();
}

void ConfigParser::checkServersDuplicate()
{
	for (std::vector<ServerConf*>::iterator it = _servers.begin(); it != _servers.end(); it++)
		for (std::vector<ServerConf*>::iterator it2 = _servers.begin(); it2 != _servers.end(); it2++)
			if (it != it2 && (*it)->getServerName() == (*it2)->getServerName() && (*it)->getPort() == (*it2)->getPort())
				throw std::runtime_error("Error: Server is duplicated");
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

void ConfigParser::createServer(std::string &config, ServerConf *server)
{
	std::vector<std::string>		params;
	std::vector<std::string>		error_codes;

	Location loca;
	bool	flag_autoindex = false;
	bool	flag_max_size = false;

	params = splitParametrs(config += ' ', std::string(" \n\t"));
	if (params.size() < 3)
		throw std::runtime_error("Failed server validation");

	for (size_t i = 0; i < params.size(); i++)
	{
		if (params[i] == "listen" && (i + 1) < params.size())
		{
			if (server->getPort())
				throw std::runtime_error("Port is duplicated");
			i++;
			checkToken(params[i]);
			params[i].erase(params[i].size() - 1);
			server->setPort(params[i]);
		}
		else if (params[i] == "location" && (i + 1) < params.size())
		{
			std::string	path;
			bool		isTilde = false;

			if (params[++i] == "{" || params[i] == "}")
				throw std::runtime_error("Wrong character in server scope{}");
			if (params[i] == "~") {
				i++;
				isTilde = true;
			}
			else if (params[i][0] != '/')
				throw std::runtime_error("Bad location");
			path += params[i];
			std::vector<std::string> codes;
			if (params[++i] != "{")
				throw std::runtime_error("Wrong character in server scope{}");
			i++;
			while (i < params.size() && params[i] != "}")
				codes.push_back(params[i++]);
			server->setLocation(path, codes, isTilde);
			if (i < params.size() && params[i] != "}")
				throw std::runtime_error("Wrong character in server scope{}");
		}
		else if (params[i] == "root" && (i + 1) < params.size())
		{
			if (!server->getRoot().empty())
				throw std::runtime_error("Root is duplicated");
			i++;
			checkToken(params[i]);
			params[i].erase(params[i].size() - 1);
			server->setRoot(params[i]);
		}
		else if (params[i] == "error_page" && (i + 1) < params.size() && (i + 2) < params.size())
		{
			checkToken(params[i + 2]);
			params[i + 2].erase(params[i + 2].size() - 1);
			if (server->getPathErrorPage(params[i + 1]).empty())
				throw std::runtime_error("[CONFIG] Error : Unsupported error_page");
            server->setErrorPage(params[i + 1], params[i + 2]);
			i+=2;
		}
		else if (params[i] == "client_max_body_size" && (i + 1) < params.size()) {
			if (flag_max_size)
				throw std::runtime_error("Client_max_body_size is duplicated");
			i++;
			checkToken(params[i]);
			params[i].erase(params[i].size() - 1);
			server->setClientMaxBodySize(params[i]);
			flag_max_size = true;
		}
		else if (params[i] == "server_name" && (i + 1) < params.size())
		{
			if (!server->getServerName().empty())
				throw std::runtime_error("Server_name is duplicated");
			i++;
			checkToken(params[i]);
			params[i].erase(params[i].size() - 1);
			server->setServerName(params[i]);
		}
		else if (params[i] == "index" && (i + 1) < params.size())
		{
			if (!server->getDefaultIndex().empty())
				throw std::runtime_error("Index is duplicated");
			i++;
			checkToken(params[i]);
			params[i].erase(params[i].size() - 1);
			server->setDefaultIndex(params[i]);
		}
		else if (params[i] == "autoindex" && (i + 1) < params.size())
		{
			if (flag_autoindex)
				throw std::runtime_error("Autoindex of server is duplicated");
			i++;
			checkToken(params[i]);
			params[i].erase(params[i].size() - 1);
			server->setAutoindex(params[i]);
			flag_autoindex = true;
		}
		else if (params[i] == "return" && (i + 1) < params.size())
		{
			std::string statusCode;
            std::string hostname;

			if (!(params[i + 1] == "300" || params[i + 1] == "301" || params[i + 1] == "302" || params[i + 1] == "303"
					|| params[i + 1] == "304" || params[i + 1] == "307" || params[i + 1] == "308"))
				throw std::runtime_error("[CONFIG] Error: Invalid status code in return directive");
			statusCode = params[i + 1];
			i++;

            if ((i + 1) < params.size() && params[i + 1] != "}") {
				i++;
				if (!params[i].empty() && params[i][params[i].size() - 1] == ';')
					params[i].erase(params[i].size() - 1);
                hostname = params[i];
            }

			server->setDefaultRedirStatusCode(statusCode);
			server->setDefaultRedirHostname(hostname);
		}
		else if (params[i] != "}" && params[i] != "{")
			throw std::runtime_error("Unsupported directive");
	}
	if (server->getDefaultIndex().empty())
		server->setDefaultIndex("index.html");
	if (server->getServerName().empty())
		server->setServerName("default_server");
	if (server->getRoot().empty())
		server->setRoot("./");
	if(!server->is_setted_loca_root)
		server->addRootToLocations(server->getRoot());
	if (!server->getPort())
		throw std::runtime_error("[CONFIG] Error : port not found or unreadable");
	server->addIndexToLocations(server->getDefaultIndex());
	if (!server->getDefaultRedirStatusCode().empty())
		server->addRedirToLocations(server->getDefaultRedirStatusCode(), server->getDefaultRedirHostname());
	if (server->checkLocationsDuplicate())
		throw std::runtime_error("[CONFIG] Error : location is duplicated");
}
