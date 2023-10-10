#include "Command.hpp"
#include "User.hpp"
#include "Server.hpp"

Command::Command(Server &server, std::string prefix, std::string command, std::vector<std::string> params)
: server(server), prefix(prefix), command(command), params(params)
{
}

Command::Command(Server &server)
: server(server)
{
}

const Command::CmdFunc Command::cmdArr[] = {&Command::pass, &Command::ping, &Command::nick, &Command::user};
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
std::string	Command::getCommand()
{
	return this->command;
}

std::string	Command::getParams()
{
	std::string params;

	for (std::vector<std::string>::iterator it = this->params.begin(); it != this->params.end(); ++it)
	{
		params += *it;
		params += " + ";
	}
	return params;
}

void 	Command::execute(User user)
{
    static const std::string arr[] = {"PASS", "PING", "NICK", "USER"};

    for (int i = 0; i < 4; i++)
    {
        if (this->command == arr[i])
        {
            (this->*cmdArr[i])(user, this->prefix, this->params);
            return;
        }
    }
	std::cout << this->command << " = Commande inconnue" << std::endl;
}

void	Command::ping(User user, std::string prefix, std::vector<std::string> params)
{
	if (params.size() >= 0)
	{
		std::cout << "PING = " << params[0] << std::endl;
	}
	if (prefix.length() > 0)
	{
		std::cout << "PREFIX = " << prefix << std::endl;
	}
	std::string pong = "PONG :localhost 6667\r\n";
	send(user.getFd(), pong.c_str(), pong.length(), 0);
	std::cout << "Pong envoye \n";
}

void	Command::nick(User user, std::string prefix, std::vector<std::string> params)
{
	if (prefix.length() > 0)
	{
		std::cout << "PREFIX = " << prefix << std::endl;
	}
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

void	Command::user(User user, std::string prefix, std::vector<std::string> params)
{
	if (prefix.length() > 0)
	{
		std::cout << "PREFIX = " << prefix << std::endl;
	}
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
	std::string welcome_msg = ":localhost 001 " + user.getNick() + " :Welcome to the Internet Relay Network " + user.getNick() + "\r\n";
    send(user.getFd(), welcome_msg.c_str(), welcome_msg.length(), 0);
}

void	Command::pass(User user, std::string prefix, std::vector<std::string> params)
{
	if (prefix.length() > 0)
	{
		std::cout << "PREFIX = " << prefix << std::endl;
	}
	if (params.size() != 1)
	{
		std::string err = ":server 461 " + user.getNick() + " PASS :Not enough parameters\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	}
/* 	if (user.getIsRegistered() == true)
	{
		std::string err = ":server 462 " + user.getNick() + " :You may not reregister\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	} */
	if (params[0] != user.getPasswd())
	{
		std::string err = ":server 464 " + user.getNick() + " :Password incorrect\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	}
	user.setPasswd(params[0]);
}

void Command::handleData(User user, const std::string& data) 
{
    buffer += data;

    // Trouver toutes les commandes complètes
    std::size_t pos;
    while ((pos = buffer.find("\r\n")) != std::string::npos) {
        std::string line = buffer.substr(0, pos);
        buffer = buffer.substr(pos + 2);

        // Traitement de la ligne (i.e., une commande complète)
        // Remplacer "user" par l'instance de l'utilisateur actuel
        parseLine(user, line);
    }
}

void	Command::parseLine(User user, std::string line)
{
	std::istringstream iss(line);
    std::string prefix, command;
    std::vector<std::string> params;
	user.getFd();

    // Supposons que notre ligne commence par un préfixe optionnel
    if (line[0] == ':') {
        iss >> prefix;
    }
    iss >> command;
    std::string param;
    while (iss >> param) {
        params.push_back(param);
    }
	Command cmd(server, prefix, command, params);
	std::cout << cyan << "command = " << cmd.getCommand() << " params = " << cmd.getParams() << reset << std::endl; 
	cmd.execute(user);
}