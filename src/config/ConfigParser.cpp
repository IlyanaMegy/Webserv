#include "../../inc/config/ConfigParser.hpp"

int getTypePath(std::string const path) {
	struct stat buff;
	int res = stat(path.c_str(), &buff);
	std::cout << "res = " << (buff.st_mode  & S_IFREG) << std::endl;
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

int ConfigParser::createCluster(const std::string &config_file) {
	if (getTypePath(config_file) != 1)
		throw std::runtime_error("Error: Invalid configuration file.");
	if (checkFile(config_file, 4) != 1)
		throw std::runtime_error("Error: Inaccessible configuration file.");
	std::string content = readFile(config_file);
	if (content.empty())
		throw std::runtime_error("Error: Empty configuration file.");
	
	std::cout << OLIV << "first checks passed !" << RESET << std::endl;
	return 1;
	//	check type path
	//	check file
	// read file
	//	check if empty
	//	remove comments
	//	remove white spaces

	//	split servers
	//	check _server_conf size && nb_servers
	//	create each server
	// check servers if multiple
}