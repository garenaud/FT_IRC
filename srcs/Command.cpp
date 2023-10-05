#include "Command.hpp"
#include "User.hpp"
#include "Server.hpp"

Command::Command(Server &server, std::string prefix, std::string command, std::vector<std::string> params)
: server(server), prefix(prefix), command(command), params(params)
{
}

const Command::CmdFunc Command::cmdArr[] = {&Command::ping, &Command::nick, &Command::user};

Command::~Command() {}

void		Command::setPrefix(std::string prefix)
{
	this->prefix = prefix;
}

std::string	Command::getPrefix()
{
	return this->prefix;
}

void	Command::setCommand(std::string command)
{
	this->command = command;
}

void	Command::setParams(std::vector<std::string> params)
{
	this->params = params;
}

std::string	Command::getParams()
{
	std::string params;

	for (std::vector<std::string>::iterator it = this->params.begin(); it != this->params.end(); ++it)
	{
		params += *it;
		params += " ";
	}
	return params;
}

void 	Command::execute(User user)
{
    static const std::string arr[] = {"PING", "NICK", "USER"};

    for (int i = 0; i < 3; i++)
    {
        if (this->command == arr[i])
        {
            (this->*cmdArr[i])(user, this->prefix, this->params);
            return;
        }
    }
}

void	Command::ping(User user, std::vector<std::string> params)
{
	std::string pong = "PONG :localhost 6667\r\n";
	send(user.getFd(), pong.c_str(), pong.length(), 0);
	std::cout << "Pong envoye \n";
}

void	Command::nick(User user, std::vector<std::string> params)
{
	if (params.size() != 1)
	{
		std::string err = ":server 461 " + user.getNick() + " NICK :Not enough parameters\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	}
	std::string nick = params[0];
	if (nick.length() > 9)
	{
		std::string err = ":server 432 " + user.getNick() + " NICK :Erroneous nickname\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	}
	if (server.isNickAvailable(nick))
	{
		std::string err = ":server 433 " + user.getNick() + " NICK :Nickname is already in use\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	}
	user.setNick(nick);
	std::cout << "NICKNAME = " << user.getNick() << std::endl;
}

void	Command::user(User user, std::vector<std::string> params)
{
	if (params.size() != 4)
	{
		std::string err = ":server 461 " + user.getNick() + " USER :Not enough parameters\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	}
	std::string username = params[0];
	std::string mode = params[1];
	std::string unused = params[2];
	std::string realname = params[3];
	user.setUser(username);
	std::cout << "USERNAME = " << user.getUser() << std::endl;
}
