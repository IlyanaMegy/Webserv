#include "../../../inc/Webserv.hpp"
#include "../../../inc/Network/config/ConfigParser.hpp"

int getTypePath(std::string const path) {
	struct stat buff;
	int res = stat(path.c_str(), &buff);
	if (!res) {
		if (buff.st_mode & S_IFREG)
			return 1;
		else if (buff.st_mode & S_IFDIR)
			return 2;
		return 3;
	}
	return -1;
}

int checkFile(std::string const path, int mode) {
	return (access(path.c_str(), mode));
}

int isFileExistAndReadable(std::string const path, std::string const index) {
	if (getTypePath(index) == 1 && checkFile(index, 4) == 0) return (0);
	if (getTypePath(path + index) == 1 && checkFile(path + index, 4) == 0)
		return (0);
	return (-1);
}

std::string readFile(std::string path) {
	if (path.empty() || path.length() == 0) return (NULL);
	std::ifstream file(path.c_str());
	if (!file || !file.is_open()) return (NULL);
	std::stringstream stream_binding;
	stream_binding << file.rdbuf();
	return (stream_binding.str());
}

void ConfigParser::splitServers(std::string &content) {
	size_t start = 0;
	size_t end = 1;

	if (content.find("server", 0) == std::string::npos)
		throw std::runtime_error("Error: Couldn't find server");
	while (start != end && start < content.length()) {
		start = findStartServer(start, content);
		end = findEndServer(start, content);
		if (start == end)
			throw std::runtime_error("Error: Wrong server scope{}");
		this->_server_config.push_back(content.substr(start, end - start + 1));
		this->_nb_server++;
		start = end + 1;
	}
}

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

	splitServers(content);
	if (this->_server_config.size() != this->_nb_server)
		throw std::runtime_error("Error: Coudn't split config file.");
	std::cout << OLIV << "[CONFIG] Success spliting config file." << RESET << std::endl;
	
	// for (size_t i = 0; i < this->_nb_server; i++)
	// {
	// 	ServerConf server;
	// 	// createServer(this->_server_config[i], server);
	// 	this->_servers.push_back(server);
	// }
	return 0;
}
