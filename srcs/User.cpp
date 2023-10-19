#include "User.hpp"
#include "Server.hpp"

User::User(int fd, std::string nick, std::string user) : fd(fd), nick(nick), user(user), isRegistered(0)
{
}

User::User(int fd) : fd(fd), isRegistered(0)
{
}

User::~User() {}

void	User::operator=(User const &src)
{
	//this->fd = src.fd;
	this->nick = src.nick;
	this->user = src.user;
	this->passwd = src.passwd;
	this->realname = src.realname;
	this->hostname = src.hostname;
	this->mode = src.mode;
	this->isRegistered = src.isRegistered;
}

bool	User::operator==(const User& other) const
{
	return (this->fd == other.fd &&
			this->nick == other.nick &&
			this->user == other.user &&
			this->passwd == other.passwd &&
			this->realname == other.realname &&
			this->hostname == other.hostname &&
			this->mode == other.mode &&
			this->isRegistered == other.isRegistered);
}

/* void	User::setFd(int fd)
{
	this->fd = fd;
} */

int		User::getFd() const
{
	return this->fd;
}

void	User::setNick(std::string nick)
{
	this->nick = nick;
	//std::cout << greenbg << "new nickname = " << this->nick << "fd = " << this->fd << reset << std::endl;
}

std::string	User::getNick()
{
	return this->nick;
}

void	User::setUser(std::string user)
{
	this->user = user;
	checkRegistration();
	//std::cout << greenbg << "new username = " << this->user << " fd = " << this->fd << reset << std::endl;
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
    if (!user.empty() && !nick.empty() && !passwd.empty()) 
	{
        this->isRegistered = 1;
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

void	User::addInvitedChannel(std::string channelName)
{
	this->invitedChannels.push_back(channelName);
}

void	User::rmInvitedChannel(std::string channelName)
{
	for (std::vector<std::string>::iterator it = invitedChannels.begin(); it != invitedChannels.end(); ++it)
	{
		if (*it == channelName)
			it = invitedChannels.erase(it);
	}
}

bool	User::isInvited(std::string channelName) const
{
	std::vector<std::string>::const_iterator it;
	for (it = this->invitedChannels.begin(); it != this->invitedChannels.end(); ++it)
	{
		if (*it == channelName)
			return true;
	}
	return false;
}

void	User::addOperatorChannel(std::string channelName)
{
	this->operatorChannels.push_back(channelName);
}

void	User::rmOperatorChannel(std::string channelName)
{
	for (std::vector<std::string>::iterator it = operatorChannels.begin(); it != operatorChannels.end(); ++it)
	{
		if (*it == channelName)
			it = operatorChannels.erase(it);
	}
}

bool	User::isOperator(std::string channelName) const
{
	std::vector<std::string>::const_iterator it;
	for (it = this->operatorChannels.begin(); it != this->operatorChannels.end(); ++it)
	{
		if (*it == channelName)
			return true;
	}
	return false;
}