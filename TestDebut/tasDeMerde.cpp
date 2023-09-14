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
/* std::stringstream ss(buffer);
ss >> cmd >> arg; */
void	handle_client(int clientSocket)
{
	char buffer[4096];
	int bytesRead;

	while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0)
	{
		std::string receivedData(buffer, bytesRead);
		if (receivedData.find("CAP LS") != std::string::npos)
		{
			std::cout << "commande CAP LS" << std::endl;
			std::string capabilities = ":localhost CAP * LS :multi-prefix sasl\r\n";
			send(clientSocket, capabilities.c_str(), capabilities.size(), 0);
		}
		if (receivedData.find("CAP REQ :multi-prefix") != std::string::npos)
		{
			std::cout << "commande CAP REQ" << std::endl;
			std::string response = ":localhost CAP * ACK :multi-prefix\r\n";
			send(clientSocket, response.c_str(), response.size(), 0);
		}
		if (receivedData.find("CAP END") != std::string::npos)
		{
			std::cout << "commande CAP END" << std::endl;
			send(clientSocket, receivedData.c_str(), receivedData.size(), 0);
			continue;
		}
		if (receivedData.find("NICK") != std::string::npos)
		{
			std::cout << "commande NICK" << std::endl;
/* 			std::string response = ":localhost 001 grenaud- :Welcome to the Internet Relay Chat Network, grenaud-";
			send(clientSocket, response.c_str(), response.size(), 0); */
			continue;
		}
		if (receivedData.find("USER") != std::string::npos)
		{
			std::cout << "commande USER" << std::endl;
 			std::string response = ":localhost 001 grenaud- :Welcome to the Internet Relay Chat Network, grenaud-";
			send(clientSocket, response.c_str(), response.size(), 0);
			continue;
		}
/* 		if (receivedData.find("JOIN") != std::string::npos)
		{
			std::cout << "commande JOIN" << std::endl;
			std::string response = ":localhost 461 grenaud- JOIN :Not enough parameters";
			send(clientSocket, response.c_str(), response.size(), 0);
			continue;
		} */
	}
	std::cout << "fonction handle_client finie" << std::endl;
}

int main(int argc, char **argv)
{
	//signal pour gerer le ctrl + c
	signal(SIGINT, handler);

	fd_set read_fds;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	if (argc != 2)
		return 0;
	//creation de la socket pour le serveur
	//AF_INET = protocole IPV4
	//SOCK_STREAM = type de socket oriente connexion (associes a des protocoles de transport tels que TCP)
	//0 = le protocole, dans ce cas la on laisse le systeme choisir en fonction des autres parametres (on pourrait mettre IPROTO_TCP a la place pour indiquer explicitement que l'on souhaite utiliser le protocole TCP)
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	//si la socket renvoie -1 c'est qu'il y a une erreur lors de la creation du serveur
	if (serverSocket == -1)
	{
		perror("Erreur lors de la creation du serveur");
		return 1;
	}
	std::cout << "La socket " << serverSocket << " est ouverte en TCP/IP" << std::endl;
	//la structure sockaddr_in est utilise pour stocker les infos sur l'adresse et le port du serveur
	struct sockaddr_in serverAddress;
	//ici on definit le domaine d'adresse(address family) de la structure, AF_INET indique qu'on utilise le protocole IPV4
	serverAddress.sin_family = AF_INET;
	//ici on definit le port, la fonction htons (Host to Network Short). Cette fonction assure que l'ordre des octets est correct pour le reseau
	serverAddress.sin_port = htons(atoi(argv[1]));
	//ici on configure l'adress IP du serveur. INADDR_ANY signifie que le serveur sera en ecoute sur toutes les interfaces reseau disponible de la machine.
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	//ici on creee une socket pour le user qui se connecterait grace a bind
	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
	{
		perror("Erreur lors de la liaison du socket");
		return 1;
	}

	//on ecoute sur la socket du serveur (serverSocket), le 2e parametre est le nombre max de connexion qui peuvent etre mises en file d'attente
	if (listen(serverSocket, 5) == -1)
	{
		perror("Erreur lors du listen");
		return 1;
	}

	std::cout << "Serveur IRC basique en ecoute sur le port " << argv[1] << std::endl;

	while (!stop)
	{
		//la partie select est la pour verifier si une nouvelle connexion est prete avant d'appeler accept et donc eviter le cote bloquant de la fonction accept
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
			handle_client(clientSocket);
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