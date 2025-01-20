#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <stdexcept>
# include <cstring>

# include "Socket.hpp"

# define BUFFER_SIZE 42

class Client {

private:
	char buffer[BUFFER_SIZE];
	// HTTPReq request;
	// HTTPRep reponse;
	Socket _socket;

public:
	Client(int serverFd);
	Client(Client const &ref);
	~Client();

	Client	&operator=(Client const &ref);

	Socket &getSocket();
	void read();

};

#endif