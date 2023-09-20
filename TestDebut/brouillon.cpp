#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>

bool stop = false;

void handler(int)
{
	stop = true;
}

int main(int argc, char **argv)
{
	if (argc != 2)
		return 0;
	signal(SIGINT, handler);
	fd_set read_fds;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
	{
		perror("Erreur lors de la creation du serveur");
		return 1;
	}
	std::cout << "La socket " << serverSocket << " est ouverte en TCP/IP" << std::endl;
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(atoi(argv[1]));
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
	{
		perror("Erreur lors de la liaison du socket");
		return 1;
	}
	if (listen(serverSocket, 5) == -1)
	{
		perror("Erreur lors du listen");
		return 1;
	}
	std::cout << "Serveur IRC basique en ecoute sur le port " << argv[1] << std::endl;
	while (!stop)
	{
		FD_ZERO(&read_fds);
		FD_SET(serverSocket, &read_fds);
		int selectRes = select(serverSocket + 1, &read_fds, NULL, NULL, &tv);
		if (selectRes == -1)
		{
			perror("Erreur du select");
			break;
		}
		else if (selectRes > 0)
		{
			std::cout << "Dans la boucle while (!stop)..." << std::endl;
			struct sockaddr_in clientAdress;
			socklen_t clientAdressLength = sizeof(clientAdress);
			int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAdress, &clientAdressLength);
			if (clientSocket == -1)
			{
				perror("Erreur lors de l'acceptation de la connexion client");
				continue;
			}
			std::cout << "Nouvelle connexion cliente acceptee." << std::endl;

			std::string welcomeMessage = "Welcome to the first server of grenaud-";
			send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size(), 0);
			close(clientSocket);
		}
	}
	if (stop == true)
		std::cout << "utilisation ctrl + c" << std::endl;
	close(serverSocket);
	return 0;
}
