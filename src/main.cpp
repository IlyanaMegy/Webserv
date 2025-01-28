#include "Epoll.hpp"
#include "Server.hpp"

#define PORT 8080

#define FILE "configs/default.conf"

void	runServer(void)
{
	Server	server(PORT);
	Epoll	epoll(server.getSocket().getFd());

	while (true) {
		try {
			epoll.wait();
		} catch (std::exception &e) {
			std::cout << e.what() << std::endl;
		}

		for (int i = 0; i < epoll.getReadyFdsNb(); i++) {
			if (epoll.getFd(i) == server.getSocket().getFd()) {
				try {
					server.acceptClient(epoll);
				} catch (std::exception &e) {
					std::cout << e.what() << std::endl;
				}
			}

			else {
				if (epoll.getEvent(i) == (EPOLLIN | EPOLLOUT)
				|| epoll.getEvent(i) == EPOLLIN)
					server.readFrom(epoll.getFd(i));
				if (epoll.getEvent(i) == (EPOLLIN | EPOLLOUT)
				|| epoll.getEvent(i) == EPOLLOUT)
					server.sendTo(epoll.getFd(i));

				if (server.getClient(epoll.getFd(i))->getShouldClose()) {
					epoll.deleteFd(epoll.getFd(i));
					server.closeConnection(epoll.getFd(i));
				}
			}
		}
	}
}

int	main(void)
{
	try {
		runServer();
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		return 1;
	}
	return 0;
}

