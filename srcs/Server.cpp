#include "Server.hpp"
#include "User.hpp"
#include "Msg.hpp"

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
    std::memset(&this->hints, 0, sizeof(this->hints));
    this->hints.ai_family = family;
    this->hints.ai_socktype = type;
    this->hints.ai_flags = flag;
}

int     Server::get_listener_socket(void)
{
    int listener;     // Listening socket descriptor
    int yes=1;        // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo *ai, *p;
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
{//a voir
    struct pollfd   data;

    data.fd = newfd;
    data.events = POLLIN;

    this->pfds.push_back(data);
}

void    Server::del_from_pfds(int index)
{// a voir, manque test erreur
if (index < (int)this->pfds.size())
    this->pfds.erase(this->pfds.begin() + index);
}

void    Server::setListeningSocket()
{// a voir..
    this->listener_socket = get_listener_socket();
    if (this->listener_socket == -1)
    {
        std::cerr << "error getting listening socket\n";
        exit (1);
    }
    add_to_pfds(this->listener_socket);
}

void    Server::handleNewConnection()
{
    std::string connection;

    this->addrlen = sizeof(this->remoteaddr);
    this->accepted_socket = accept(this->listener_socket,reinterpret_cast<struct sockaddr *>(&this->remoteaddr), &(this->addrlen));
    if (this->accepted_socket == -1)
    {
        perror("accept");
    }
    else
    {
        add_to_pfds(this->accepted_socket);
/*         User newUser(accepted_socket, "defaultNick", "defaultUser");
        users.push_back(newUser); */
        //std::cout << newUser << std::endl;
        //std::cout << users[0];
        const char* c =this->remoteIP.c_str();
        connection = inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr),(char *)c, INET6_ADDRSTRLEN);

        std::cout << "pollserver: new connection from "<< connection << " on socket " << accepted_socket << std::endl;
    }
}

void Server::handleClient(int index)
{
    int nbytes = recv(pfds[index].fd, this->buffer, sizeof(this->buffer), 0);
    int sender_fd = pfds[index].fd;
    // partie modifiee
    Msg     aMess;
    aMess.initialize(this->accepted_socket, "user", this->buffer, nbytes);
    // fin partie modifiee
    if (nbytes <= 0)
    {
        // Got error or connection closed by client
        if (nbytes == 0)
        {
            // Connection closed
            std::cerr << "pollserver: socket " << sender_fd << " hung up\n";
        }
        else
        {
            perror("recv");
        }
        close(pfds[index].fd); // Bye!
        del_from_pfds(index);
    }
    else
    {
        // Convertit le buffer en une chaîne C++ pour faciliter la manipulation
        std::string received_data(this->buffer, nbytes);

        // Vérifie si CAP LS a été envoyé par le client
        if (received_data.find("CAP LS") != std::string::npos)
        {
            std::string cap_end = ":localhost CAP * LS :multi-prefix sasl\r\n";
            send(sender_fd, cap_end.c_str(), cap_end.length(), 0);
        }
        if (received_data.find("CAP REQ :multi-prefix") != std::string::npos)
        {
            std::string cap_end = ":localhost CAP * ACK :multi-prefix\r\n";
            send(sender_fd, cap_end.c_str(), cap_end.length(), 0);
        }
        if (received_data.find("CAP END") != std::string::npos)
        {
            std::string cap_end = ":localhost CAP * ACK :none\r\n";
            send(sender_fd, cap_end.c_str(), cap_end.length(), 0);
            //std::cout << ":localhost CAP * ACK :none" << std::endl;
        }
        if (received_data.find("JOIN") != std::string::npos)
        {
            std::cout << "join..." << std::endl;
        }
    if (received_data.find("NICK") != std::string::npos && received_data.find("USER") != std::string::npos)
    {
        // Extrait les informations NICK et USER
        std::string nick = received_data;
        std::string user = received_data;

        // Ajoute l'utilisateur à la liste
        addUser(pfds[index].fd, nick, user);
    }
        else
        {
            // We got some good data from a client
            for(int j = 0; j < (int)this->pfds.size(); j++)
            {
                // Send to everyone!
                int dest_fd = pfds[j].fd;
                // Except the listener and ourselves
                if (dest_fd != this->listener_socket && dest_fd != sender_fd)
                {
                    if (send(dest_fd, this->buffer, nbytes, 0) == -1)
                    {
                        perror("send");
                    }
                }
            }
        }
    }
}

void    Server::run()
{
    setListeningSocket();
    for (;;)
    {
        int poll_count = poll(&pfds[0], this->pfds.size(), -1);
        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }
         for(int i = 0; i < (int)this->pfds.size(); i++)
         {
              if (this->pfds[i].revents & POLLIN)
              {
                  if (this->pfds[i].fd == this->listener_socket)
                  {
                      handleNewConnection();
                  }
                  else
                  {
                      handleClient(i);
                  }
              }
         }
    }
}

void Server::addUser(int fd, const std::string& nick, const std::string& user)
{
    User newUser(fd, nick, user);
    users.push_back(newUser);
    for (std::size_t i = 0; i < users.size(); ++i)
    {
        std::cout << users[i].getFd() << " NICKNAME = " << users[i].getNick() << " USERNAME = " << users[i].getUser() << std::endl;
    }
}
