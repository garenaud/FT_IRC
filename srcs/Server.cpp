#include "Server.hpp"
#include "User.hpp"
#include "Msg.hpp"
#include "Command.hpp"

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
    std::cout << "add_to_pfds: new connection from " << newfd << std::endl;
}

void    Server::del_from_pfds(int index)
{// a voir, manque test erreur
    if (index < (int)this->pfds.size())
    {
        removeUser(this->pfds[index].fd);
        this->pfds.erase(this->pfds.begin() + index);
    }
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
        addUser(this->accepted_socket);
        //std::cout << newUser << std::endl;
        //std::cout << users[0];
        const char* c =this->remoteIP.c_str();
        connection = inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr),(char *)c, INET6_ADDRSTRLEN);

        std::cout << "pollserver: new connection from "<< connection << " on socket " << accepted_socket << std::endl;
    }
}

void Server::handleClient(Msg &aMess, int index)
{
    // ajout de memset
    memset(this->buffer, 0, sizeof(this->buffer)); //051023
    int nbytes = recv(pfds[index].fd, this->buffer, sizeof(this->buffer), 0);
    int sender_fd = pfds[index].fd;
    // partie modifiee
 //   Msg     aMess;
   // Msg *aMess = new Msg;
    aMess.initialize(sender_fd, "user", this->buffer, nbytes);//initialize(this->accepted_socket, "user", this->buffer, nbytes);
    //aMess.view();
    //aMess.split("\r\n");
    aMess.split2("\r\n");
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
       // std::cout << green;
        std::string received_data(this->buffer, nbytes);
        std::cout << redbg << "Received data: " << received_data << reset << std::endl;
        Command cmd(*this);
        //cmd.handleData(users[getUserIndex(sender_fd)], received_data);
       while(aMess.getMessageSize() > 0)
        {
            cmd.parseLine(users[getUserIndex(sender_fd)], aMess.getMessage());
            std::cout << red << aMess.getMessageSize() << reset << std::endl;
        }
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

void    Server::run()
{
    setListeningSocket();
      Msg     aMess;///
      Command cmd(*this);
     //  Msg *aMess = NULL;
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
                      handleClient(aMess,i);///
                  }
              }
         }
    } 
}

void Server::addUser(int fd)
{
    User newUser(fd);
    users.push_back(newUser);
    for (std::size_t i = 0; i < users.size(); ++i)
    {
        std::cout << yellow << "FD = " << users[i].getFd() << " NICKNAME = " << users[i].getNick() << " USERNAME = " << users[i].getUser() << reset << std::endl;
    }
}

void Server::removeUser(int fd)
{
    int index = getUserIndex(fd);
    if (index != -1)
    {
        users.erase(users.begin() + index);
    }
}

void Server::sendPing(int client_fd)
{
    std::string ping_msg = "PING :localhost 6667\r\n";
    send(client_fd, ping_msg.c_str(), ping_msg.length(), 0);
    std::cout << "Ping envoye \n";
}

bool    Server::isNickAvailable(const std::string& nick)
{
    for (std::size_t i = 0; i < users.size(); ++i)
    {
        if (users[i].getNick() == nick)
        {
            return false;
        }
    }
    return true;
}

int     Server::getUserIndex(int fd)
{
    for (std::size_t i = 0; i < users.size(); ++i)
    {
        if (users[i].getFd() == fd)
        {
            return i;
        }
    }
    return -1;
}

int     Server::getPfdsIndex(int fd)
{
    for (std::size_t i = 0; i < pfds.size(); ++i)
    {
        if (pfds[i].fd == fd)
        {
            return i;
        }
    }
    return -1;
}

std::string Server::getPasswd()
{
    return this->passwd;
}

void    Server::displayUsers()
{
    for (std::size_t i = 0; i < users.size(); ++i)
    {
        std::cout << yellowbg << " FD = " << users[i].getFd() << " NICKNAME = " << users[i].getNick() << " USERNAME = " << users[i].getUser() << " REALNAME = " << users[i].getRealname() << reset << std::endl;
    }
}