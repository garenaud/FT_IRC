#include "User.hpp"

User::User(int fd, std::string nick, std::string user)
{
	this->fd = fd;
	this->nick = nick;
	this->user = user;
	this->isRegistered = 0;
}

User::User(int fd)
{
	this->fd = fd;
	this->isRegistered = 0;
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
	std::cout << greenbg << "new nickname = " << this->nick << "fd = " << this->fd << reset << std::endl;
}

std::string	User::getNick()
{
	return this->nick;
}

void	User::setUser(std::string user)
{
	this->user = user;
	checkRegistration();
	std::cout << greenbg << "new username = " << this->user << " fd = " << this->fd << reset << std::endl;
}

std::string	User::getUser()
{
	return this->user;
}

void User::setPasswd(std::string passwd)
{
	this->passwd = passwd;
	checkRegistration();
}

std::string	User::getPasswd()
{
	return this->passwd;
}

void User::setRealname(std::string realname)
{
	this->realname = realname;
}

std::string	User::getRealname()
{
	return this->realname;
}

void User::setHostname(std::string hostname)
{
	this->hostname = hostname;
}

std::string	User::getHostname()
{
	return this->hostname;
}

void User::setMode(std::string mode)
{
	this->mode = mode;
}

std::string	User::getMode()
{
	return this->mode;
}

void User::checkRegistration() 
{
    if (!user.empty() && !nick.empty() && !passwd.empty()) {
        this->isRegistered = 1;
		std::cout << greenbg << "isRegistered = " << isRegistered << reset << std::endl;
    }
	else
	{
		this->isRegistered = 0;
	}
}

void User::setIsRegistered(int isRegistered)
{
	this->isRegistered = isRegistered;
}

int User::getIsRegistered()
{
	return this->isRegistered;
}

std::ostream& operator<<(std::ostream& o, User &src) 
{
    o << "User = " << src.getUser() << ", Nick = " << src.getNick() << ", FD = " << src.getFd();
    return o;
}