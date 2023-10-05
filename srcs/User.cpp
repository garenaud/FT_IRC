#include "User.hpp"

User::User(int fd, std::string nick, std::string user)
{
	this->fd = fd;
	this->nick = nick;
	this->user = user;
}

User::~User() {}

void	User::setFd(int fd)
{
	this->fd = fd;
}

int		User::getFd()
{
	return this->fd;
}

void	User::setNick(std::string nick)
{
	this->nick = nick;
}

std::string	User::getNick()
{
	return this->nick;
}

void	User::setUser(std::string user)
{
	this->user = user;
}

std::string	User::getUser()
{
	return this->user;
}

std::ostream& operator<<(std::ostream& o, User &src) 
{
    o << "User = " << src.getUser() << ", Nick = " << src.getNick() << ", FD = " << src.getFd();
    return o;
}
