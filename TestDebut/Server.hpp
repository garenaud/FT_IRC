#ifndef SERVER_HPP
# define SERVER_HPP
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
		void	del_from_pdfs(); // a voir
		void	setHint(int family, int type, int flag);

	private:
		int port;
		std::string passwd;
		bool stop;

		// variable main...
		int				listener_socket;//listener
		int				accepted_socket;//newfd
		std::string		buffer[512];//char buf[256]
		std::string		remoteIP[INET6_ADDRSTRLEN];
		socklen_t		addrlen;

		struct		timeval tv;
		struct	addrinfo	hints;
		std::vector<pollfd>	pdfs;
/*        struct sockaddr_in server_address;
		struct sockaddr_in client_address;
		struct sockaddr casted;
*/
};

#endif
