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
 * @param  param: 
 * @retval None
 */
void ServerConf::setHost(std::string param)
{
	checkToken(param);
	if (param == "localhost")
		param = "127.0.0.1";
	struct sockaddr_in hostBinary;
	if (inet_pton(AF_INET, param.c_str(), &(hostBinary.sin_addr)))
		throw std::runtime_error("Error: Invalid IP address format for host");
	_host = hostBinary.sin_addr.s_addr;
	std::cout << "host = " << _host << std::endl;
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
	char dir[1024];
	getcwd(dir, 1024);
	std::string complete_root = dir + root;
	std::cout << "-> full root = " << complete_root << std::endl;
	if (getTypePath(complete_root) != 2)
		throw std::runtime_error("Error: Wrong syntax: root");
	_root = complete_root;
}

void ServerConf::setPort(std::string param)
{
	unsigned int port;
	
	port = 0;
	checkToken(param);
	for (size_t i = 0; i < param.length(); i++)
		if (!std::isdigit(param[i]))
			throw std::runtime_error("Wrong syntax: port");
	port = ft_stoi((param));
	if (port < 1 || port > 65636)
		throw std::runtime_error("Wrong syntax: port");
	_port = (uint16_t) port;
}

/**
 * @brief  set max size of clients' body request
 * @param  `param`
 * @retval None
 */
void ServerConf::setClientMaxBodySize(std::string param)
{
	checkToken(param);
	for (size_t i = 0; i < param.length(); i++)
		if (param[i] < '0' || param[i] > '9')
			throw std::runtime_error("Wrong syntax: client_max_body_size");

	if (!ft_stoi(param))
		throw std::runtime_error("Wrong syntax: client_max_body_size");
	_client_max_body_size = ft_stoi(param);
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
