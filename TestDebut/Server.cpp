#include "Server.hpp"

Server::Server() {}
Server::~Server() {}


void    Server::setPort(int port)
{
    this->port = port;
}
void    Server::setStop(bool status)
{
    this->stop = status;
}

void    Server::setPasswd(std::string passwd)
{
    this->passwd = passwd;
}

void    Server::setTV(int sec, int musec)
{
    this->tv.tv_sec = sec;
    this->tv.tv_usec = musec;
}

int     Server::getPort()
{
    return this->port;
}

void    *Server::get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(reinterpret_cast<struct sockaddr_in*>(sa)->sin_addr);
    }
    return &(reinterpret_cast<struct sockaddr_in6*>(sa)->sin6_addr);
}

void    Server::setHint(int family, int type, int flag)
{
    this->hints.ai_family = family;
    this->hints.ai_socktype = type;
    this->hints.ai_flags = flag;
}

int     Server::get_listener_socket(void)
{
    int listener;     // Listening socket descriptor
    int yes=1;        // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo *ai, *p;// hints etant prive
    setHint(AF_UNSPEC, SOCK_STREAM, AI_PASSIVE);

    std::string s;
    std::stringstream out;
    out << getPort();
    s = out.str();
    const char * port = s.c_str();
    if ((rv = getaddrinfo(NULL, port, &this->hints, &ai)) != 0) {
        std::cerr << "selectserver: " << gai_strerror(rv) << std::endl;
        exit(1);
    }
    for(p = ai; p != NULL; p = p->ai_next)
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { continue; }
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }
    if (p == NULL) { return -1;}
    freeaddrinfo(ai); // All done with this

    // Listen
    if (listen(listener, 10) == -1) {return -1;}
    return listener;
}

void    Server::add_to_pfds(int newfd)
{
    struct pollfd   data;

    data.fd = newfd;
    data.events = POLLIN;

    this->pdfs.push_back(data);


}
