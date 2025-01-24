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

void ServerConf::setHost(std::string param)
{
	checkToken(param);
	if (param == "localhost")
		param = "127.0.0.1";
	if (!isValidHost(param))
		throw std::runtime_error("Error: Wrong syntax: host");
	// !__
	_host = inet_addr(param.data());
	// !__
	std::cout << "host = " << _host << std::endl;
}

void ServerConf::setRoot(std::string root) {
	checkToken(root);
	if (getTypePath(root) == 2) {
		_root = root;
		return;
	}
	char dir[1024];
	getcwd(dir, 1024);
	std::string complete_root = dir + root;
	std::cout << "-> full root = " << complete_root << std::endl;
	if (getTypePath(complete_root) != 2)
		throw std::runtime_error("Error: Wrong syntax: root");
	_root = complete_root;
}

void ServerConf::setPort(std::string parametr)
{
	unsigned int port;
	
	port = 0;
	checkToken(parametr);
	for (size_t i = 0; i < parametr.length(); i++)
		if (!std::isdigit(parametr[i]))
			throw std::runtime_error("Wrong syntax: port");
	port = ft_stoi((parametr));
	if (port < 1 || port > 65636)
		throw std::runtime_error("Wrong syntax: port");
	_port = (uint16_t) port;
}

void ServerConf::setClientMaxBodySize(std::string parametr)
{
	unsigned long body_size;
	
	body_size = 0;
	checkToken(parametr);
	for (size_t i = 0; i < parametr.length(); i++)
	{
		if (parametr[i] < '0' || parametr[i] > '9')
			throw std::runtime_error("Wrong syntax: client_max_body_size");
	}
	if (!ft_stoi(parametr))
		throw std::runtime_error("Wrong syntax: client_max_body_size");
	body_size = ft_stoi(parametr);
	_client_max_body_size = body_size;
}

void ServerConf::setIndex(std::string index)
{
	checkToken(index);
	_index = index;
}

void ServerConf::setAutoindex(std::string autoindex)
{
	checkToken(autoindex);
	if (autoindex != "on" && autoindex != "off")
		throw std::runtime_error("Wrong syntax: autoindex");
	if (autoindex == "on")
		_autoindex = true;
}

bool ServerConf::isValidHost(std::string host) const
{
	struct sockaddr_in sockaddr;
	// !__
  	return (inet_pton(AF_INET, host.c_str(), &(sockaddr.sin_addr)) ? true : false);
	// !__
}

// 	// std::ifstream file(_configFile.c_str());
// 	// if (!file.is_open())
// 	// 	throw std::runtime_error("Error: Unable to open config file: " +
// 	// 							 _configFile);

// 	// std::string line;
// 	// ConfigBlock currBlock;
// 	// // bool insideBlock = false;

// 	// while (std::getline(file, line)) {
// 	// 	std::stringstream ss(line);
// 	// 	std::string word;
// 	// 	ss >> word;
// 	// 	std::cout << GOLD << "word : " << RESET << word << std::endl;
// 	// }
// 	// file.close();
// 	// 	// Ignorer les lignes vides ou les commentaires
// 	// 	if (word.empty() || word[0] == '#') continue;

// 	// 	if (word == "{") {
// 	// 		if (insideBlock)
// 	// 			throw std::runtime_error(
// 	// 				"Error: Nested blocks are not supported.");
// 	// 		insideBlock = true;
// 	// 		continue;
// 	// 	}
// 	// 	std::cout << BLUE << "insideBlock = " << RESET << insideBlock <<
// std::endl;

// 	// 	if (word == "}") {
// 	// 		if (!insideBlock)
// 	// 			throw std::runtime_error("Error: Unmatched closing brace.");

// 	// 		//	valide le block
// 	// 		validateConfigFile(currBlock);
// 	// 		_configBlocks.push_back(currBlock);
// 	// 		currBlock = ConfigBlock();
// 	// 		insideBlock = false;
// 	// 		continue;
// 	// 	}

// 	// 	// Ajoutez les directives au bloc actuel
// 	// 	if (insideBlock) {
// 	// 		std::string value;
// 	// 		ss >> value;
// 	// 		std::cout << PEAC << "value = " << RESET << value << std::endl;
// 	// 		if (value.empty())
// 	// 			throw std::runtime_error(
// 	// 				"Error: Missing value for directive: " + word);
// 	// 		currBlock.addDirective(word, value);
// 	// 	} else
// 	// 		throw std::runtime_error(
// 	// 			"Error: Directive found outside of a block.");
// 	// }
// 	// if (insideBlock)
// 	// 	throw std::runtime_error("Error: Missing closing brace for block.");
// 	// file.close();
// }

// void ServerConf::validateConfigFile(const ConfigBlock &block) {
// 	if (!block.hasDirective("server_name")) {
// 		throw std::runtime_error("Error: Missing 'server_name' directive.");
// 	}

// 	if (!block.hasDirective("listen")) {
// 		throw std::runtime_error("Error: Missing 'listen' directive.");
// 	}

// 	//!\ Ajouter d'autres validations spécifiques ici...
// }

// const std::vector<ConfigBlock> &ServerConf::getConfigBlocks() const {
// 	return _configBlocks;
// }

// std::string ServerConf::getValue(const std::string &key) const {
// 	for (std::vector<ConfigBlock>::const_iterator it = _configBlocks.begin(); it
// != _configBlocks.end(); ++it)
// 	{
// 		const ConfigBlock &block = *it;
// 		// Vérification dans les directives du bloc courant
// 		if (block.hasDirective(key)) return block.getDirective(key);

// 		// Si la directive n'est pas trouvée dans ce bloc, on cherche dans ses
// 		// sous-blocs (enfants)
// 		for (std::vector<ConfigBlock>::const_iterator childIt =
// block.getChildren().begin(); childIt != block.getChildren().end(); ++childIt)
// 		{
//         	const ConfigBlock &child = *childIt;
//        		if (child.hasDirective(key)) return child.getDirective(key);
// 		}
//     }
// 	throw std::runtime_error("Directive " + key +
// 							 " not found in configuration");
// }
