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



class Server
{

    public:
        Server();
        ~Server();
        void    setPort(int port);
        int     getPort();
        void    setStop(bool status);
        void    setPasswd(std::string passwd);
        void    setTV(int sec, int musec);
    private:
        int port;
        std::string passwd;

        bool stop;
        struct timeval tv;
/*        struct sockaddr_in server_address;
        struct sockaddr_in client_address;
        struct sockaddr casted;
*/
};

#endif
