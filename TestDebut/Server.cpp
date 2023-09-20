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
