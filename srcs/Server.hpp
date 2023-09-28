#ifndef SERVER_HPP
# define SERVER_HPP
#include "User.hpp"
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <vector>

class User;

class Server
{

	public:
		Server();
		~Server();
		void	setPort(int port);
		int		getPort();
		void	setStop(bool status);
		void	setPasswd(std::string passwd);
		void	setTV(int sec, int musec);

		void	*get_in_addr(struct sockaddr *sa);
		int		get_listener_socket(void);
		void	add_to_pfds(int newfd);// a voir
		void	del_from_pfds(int index); // a voir
		void	setHint(int family, int type, int flag);

		void	setListeningSocket();
		void	handleNewConnection();
		void	handleClient(int index);
		void	run();
		void	addUser(int fd, const std::string& nick, const std::string& user);

	private:
		int port;
		std::string passwd;
		bool stop;

		// variable main...
		int				listener_socket;//listener
		int				accepted_socket;//newfd
		char		buffer[512];//char buf[256]
		std::string		remoteIP;// [INET6_ADDRSTRLEN];
		socklen_t		addrlen;

		struct		timeval tv;
		struct	addrinfo	hints;
		std::vector<pollfd>	pfds;
		std::vector<User> users;
		struct sockaddr_storage remoteaddr;
/*        struct sockaddr_in server_address;
		struct sockaddr_in client_address;
		struct sockaddr casted;
*/
};

#endif