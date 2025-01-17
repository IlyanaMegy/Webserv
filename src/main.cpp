#include "../inc/Epoll.hpp"
#include "../inc/Server.hpp"

// #include <vector>

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
			// epoll_ctl
			std::cout << e.what() << std::endl;
		}

		// Parcourir les événements
		for (int i = 0; i < epoll.getReadyFd(); i++) {
			// CASE SERVER
			if (epoll.getFd(i) == server.getSocket().getFd()) {
				try {
					server.acceptClient();
				} catch (std::exception &e) {
					// epoll_ctl
					std::cout << e.what() << std::endl;
				}
			}
			else {
				server.readFrom(i);
				// Événement sur un socket client
				// char buffer[BUFFER_SIZE];
				// ssize_t bytes_read = recv(events[i].data.fd, buffer,
				// BUFFER_SIZE, 0); if (bytes_read <= 0) {
				//   // Connexion fermée ou erreur
				//   if (bytes_read == 0) {
				//     std::cout << "Client disconnected." << std::endl;
				//   } else {
				//     perror("recv");
				//   }
				// }
				continue;

				// else {

				// ---------------------------------------------------------------------------------------------
				// ---------------------------------------------------------------------------------------------
				// // HTTP Request saved in buffer var
				// // Traiter la requête et envoyer une réponse
				// buffer[bytes_read] = '\0';
				// std::cout << "Received: " << buffer << std::endl;

				// HTTP Response
				// send(events[i].data.fd, response.c_str(), response.size(),
				// 0); verifs
				server.sendTo(i);
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

