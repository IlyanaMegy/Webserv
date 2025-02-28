#include "../../inc/Network/Server.hpp"

Server::Server(uint16_t port) : _port(port), _socket(port)
{
	if (listen(_socket.getFd(), MAXCONNECT) < 0)
		throw std::runtime_error("Failed to listen on socket");
}

Server::~Server(void)
{
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
		if (it->second)
			delete it->second;
}

void Server::printinfos(void)
{
	std::vector<ServerConf*> server;
	for (std::map<std::string, ServerConf*>::iterator it = _confs.begin(); it != _confs.end(); ++it)
        server.push_back(it->second);

	std::cout << BOLD << "\n------------- Config -------------" << RESET << std::endl;
	for (size_t i = 0; i < server.size(); i++)
	{
		int r = rand() % 6;
		std::cout << rainbow(r) << "----------------------------------" << std::endl;
		std::cout << "Server #" << i + 1 << std::endl;
		std::cout << "Server name: " << server[i]->getServerName() << std::endl;
		std::cout << "Host: " << server[i]->getHost() << std::endl;
		std::cout << "Root: " << server[i]->getRoot() << std::endl;
		std::cout << "Index: " << server[i]->getIndex() << std::endl;
		std::cout << "Port: " << server[i]->getPort() << std::endl;
		std::cout << "Max BSize: " << server[i]->getMaxBodySize() << std::endl;
		// std::cout << "Error pages: " << server[i]->getErrorPages().size() << std::endl;
		// std::map<short, std::string>::const_iterator it = server[i]->getErrorPages().begin();
		// while (it != server[i]->getErrorPages().end())
		// {
		// 	std::cout << (*it).first << " - " << it->second << std::endl;
		// 	++it;
		// }
		std::cout << "\nLocations: " << server[i]->getLocations().size() << std::endl;
		std::vector<Location> locations = server[i]->getLocations(); // Stocker le résultat de getLocations() dans une variable
        std::vector<Location>::iterator it_location = locations.begin();
        while (it_location != locations.end())
        {
			std::cout << "Name Location: " << it_location->getPath() << std::endl;
			std::vector<Method> methods = it_location->getMethods();
			std::cout << "Available methods: ";
			for (std::vector<Method>::const_iterator method_it = methods.begin(); method_it != methods.end(); ++method_it)
				std::cout << *method_it << " ";
			std::cout << std::endl;
			std::cout << "index: " << it_location->getIndexLocation() << std::endl;
			if (it_location->getCgiPath().empty())
			{
				std::cout << "root: " << it_location->getRootLocation() << std::endl;
				if (!it_location->getReturn().empty())
					std::cout << "return: " << it_location->getReturn() << std::endl;
				if (!it_location->getAlias().empty())
					std::cout << "alias: " << it_location->getAlias() << std::endl;
			}
			else
			{
				std::cout << "cgi root: " << it_location->getRootLocation() << std::endl;
				std::cout << "sgi_path: " << it_location->getCgiPath().size() << std::endl;
				std::cout << "sgi_ext: " << it_location->getCgiExtension().size() << std::endl;
			}
			std::cout << std::endl;
			++it_location;
		}
		it_location = server[i]->getLocations().begin();
		std::cout << "----------------------------------\n" << std::endl;
	}
	std::cout << "" << RESET <<  std::endl;
}

// void	Server::closeConnection(int clientFd)
// {
// 	delete _clients[clientFd];
// 	_clients.erase(clientFd);
// }

// Socket	&Server::getSocket(void)
// {
// 	return _socket;
// }

// Client	*Server::getClient(int clientFd)
// {
// 	return _clients[clientFd];
// }

// void	Server::acceptClient(Epoll &epoll)
// {
// 	int		clientSocket;
// 	Client	*client = new Client(_socket.getFd(), "");

// 	if (!client)
// 		throw std::exception();
// 	clientSocket = client->getSocket().getFd();
// 	_clients[clientSocket] = client;
// 	std::cout << "New client accepted!" << std::endl;
// 	epoll.addFd(clientSocket, EPOLLIN | EPOLLOUT);
// }

// void	Server::readFrom(int clientFd)
// {
// 	char	buffer[BUFFER_SIZE] = "";
// 	ssize_t	res;

// 	res = recv(_clients[clientFd]->getSocket().getFd(), buffer, BUFFER_SIZE - 1, 0);
// 	if (res == 0) {
// 		_clients[clientFd]->setShouldClose(true);
// 		return ;
// 	}
// 	if (!_clients[clientFd]->getRequest())
// 		_clients[clientFd]->createNewRequest(_clients[clientFd]->leftoverMessage);
// 	_clients[clientFd]->getRequest()->parse(std::string(buffer));
// 	std::cout << "End of reading!" << std::endl;
// }

// void	Server::sendTo(int clientFd)
// {
// 	Response	response;
// 	std::string	responseMessage;

// 	if (!_clients[clientFd]->getRequest() || !_clients[clientFd]->getRequest()->getResponse().getIsComplete())
// 		return ;
// 	response = _clients[clientFd]->getRequest()->getResponse();
// 	response.createMessage();
// 	if (response.getShouldClose())
// 		_clients[clientFd]->setShouldClose(true);
// 	else
// 		_clients[clientFd]->leftoverMessage = _clients[clientFd]->getRequest()->getUntreatedMessage();
// 	responseMessage = response.getMessage();
// 	if (send(clientFd, responseMessage.c_str(), responseMessage.length(), 0) == -1)
// 		throw std::exception();
// 	_clients[clientFd]->deleteRequest();
// 	std::cout << "Sent to client " << CYAN << clientFd << RESET << std::endl;
// }

uint16_t Server::getPort() const
{
	return _port;
}

bool Server::isConfigKnown(std::string serverName)
{
	return _confs.find(serverName) != _confs.end();
}

void Server::addConfig(ServerConf *conf)
{
	_confs[conf->getServerName()] = conf;
}

// const char *Server::SocketCreationErrException::what() const throw() {
// 	std::cerr << "Erreur lors de la création du socket" << std::endl;
// }
