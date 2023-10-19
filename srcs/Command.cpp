#include "Command.hpp"
/* #include "User.hpp"
#include "Server.hpp"
#include "Msg.hpp" */

Command::Command(Server &server, std::string prefix, std::string command, std::vector<std::string> params)
: server(server), prefix(prefix), command(command), params(params)
{
}

Command::Command(Server &server)
: server(server)
{
}
//
const Command::CmdFunc Command::cmdArr[] = {&Command::join, &Command::cap, &Command::pass, &Command::ping, &Command::pong, &Command::nick, &Command::user, &Command::who, &Command::privmsg};
Command::~Command() {}

void	Command::sendChannelUsers(std::vector<User> channelUsers, std::string msg) const
{// a modifier
	for (int i = 0; i < static_cast<int>(channelUsers.size()); i++)
		send(channelUsers[i].getFd(), msg.c_str(), msg.length(), 0);
}

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

void 	Command::execute(User &user)
{
	static const std::string arr[] = {"JOIN", "CAP", "PASS", "PING", "PONG", "NICK", "USER", "WHO", "PRIVMSG"};//
	for (size_t i = 0; i < 9; i++)//
	{
		if (this->command == arr[i])
		{
			(this->*cmdArr[i])(user, this->prefix, this->params);
			return;
		}
	}
	std::cout << this->command << " = Commande inconnue" << std::endl;
}
// ICI
void	Command::privmsg(User &user, std::string prefix, std::vector<std::string> params)
{
	std::cout << prefix << std::endl;
	Channel *chan = server.getChannel(params[0]);
	std::string	source = user.getNick();
	std::string	message;
	if (params.size() >= 2)
	{
		message = params[params.size() - 1];
/*		if (message.length() == 0)
		{// a verifier
		std::string err = ":server 401 "  + user.getNick() +": No text to send\r\n";//
		send(user.getFd(), err.c_str(), err.length(), 0);
		}*/
		std::cout << cyan << "\t\t" <<message <<reset << std::endl;
	}
	else {};
	if (!server.isNickAvailable(params[0]))
	{

		User	*usr= server.getUserByNick(params[0]);

		if (usr)
		{
			std::cout << "PRIVMSG user case\n";
			message = ":" + user.getNick() +" PRIVMSG " + usr->getNick() + " :" + message +"\r\n";
			send(usr->getFd(), message.c_str(), message.length(), 0);
			//std::cout << "message user \t" + message + " usr = " << usr->getNick() << std::endl;
		}
	}
	else if (chan)
	{
		std::cout << cyan << "\t\t" << chan->getName() <<reset <<std::endl;
		std::vector<User> users = chan->getUsers();
		for (size_t i = 0; i < users.size(); i++)
		{
			std::string	msg = ":" + user.getNick() +" PRIVMSG " + chan->getName() + " :" + message +"\r\n";
			if (i==0)
			{
				std::cout << "PRIVMSG channel case\n";
				std::cout << "users.size() = "<< users.size() <<" \n";
			}
			std::cout << red << "\t\t\t" << msg << reset<< std::endl;
			send(users[i].getFd(), msg.c_str(), msg.length(), 0);
		}
	}
	else
	{
		std::string err = ":server 401 " + user.getNick() + " " + params[0] + " " + ": No such nick/channel\r\n";// :
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	}
}

void	Command::ping(User &user, std::string prefix, std::vector<std::string> params)
{
	(void)prefix;
 	if (params.size() >= 0)
	{
		//std::cout << "PING = " << params[0] << std::endl;
	}
	std::string pong = "PONG :localhost 6667\r\n";
	send(user.getFd(), pong.c_str(), pong.length(), 0);
	//std::cout << "Pong envoye \n";
}

void	Command::pong(User &user, std::string prefix, std::vector<std::string> params)
{
	(void)prefix;
 	if (params.size() > 0)
	{
		std::cout << "PONG " << params[0] << std::endl;
		std::string pong = "PONG " + params[0] + "\r\n";
		send(user.getFd(), pong.c_str(), pong.length(), 0);
	}
	std::string pong = "PONG :localhost 6667\r\n";
	send(user.getFd(), pong.c_str(), pong.length(), 0);
	//std::cout << "Pong envoye \n";
}

void	Command::nick(User &user, std::string prefix, std::vector<std::string> params)
{
	(void)prefix;
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
		int counter = 1;
	std::string originalNick = nick;

	while (!server.isNickAvailable(nick))
	{
		nick = originalNick + std::to_string(counter);
		counter++;
	}
	user.setNick(nick);
		if (user.getIsRegistered() == 1)
	{
		std::string err = ":server 001 " + user.getNick() + " :Welcome to the Internet Relay Network " + user.getNick() + "\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		user.setIsRegistered(2);
	}
	//std::cout << user.getFd() << " NICKNAME = " << user.getNick() << std::endl;
}

void	Command::user(User &user, std::string prefix, std::vector<std::string> params)
{
	(void)prefix;
	if (params.size() != 4)
	{
		std::string err = ":server 461 " + user.getNick() + " USER :Not enough parameters\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	}
	user.setUser(params[0]);
	user.setMode(params[1]);
	user.setHostname(params[2]);
	user.setRealname(params[3]);
	//std::cout << magentabg << user.getFd() << " " << "nickName = " << user.getNick() << " username = " << user.getUser() << " realname = " << user.getRealname() << " hostname = " << user.getHostname() << " hostname = " << user.getHostname() << reset << std::endl;
	if (user.getIsRegistered() == 1)
	{
		std::string err = ":server 001 " + user.getNick() + " :Welcome to the Internet Relay Network " + user.getNick() + "\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		user.setIsRegistered(2);
	}
	server.displayUsers();
}

void	Command::pass(User &user, std::string prefix, std::vector<std::string> params)
{
	(void)prefix;
	//std::cout << magenta << "isRegistered = " << user.getIsRegistered() << reset << std::endl;
	if (params.size() != 1)
	{
		std::string err = ":server 461 " + user.getNick() + " PASS :Not enough parameters\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	}
	//std::cout << "serveur passwd = " << server.getPasswd() << " passwd = " << params[0] << std::endl;
 	if (user.getIsRegistered() >= 1)
	{
		std::string err = ":server 462 " + user.getNick() + " :You may not reregister\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	}
 	if (params[0] != server.getPasswd())
	{
		std::string err = ":server 464 " + user.getNick() + " :Password incorrect\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		server.del_from_pfds(server.getPfdsIndex(user.getFd()));
		return ;
	}
	user.setPasswd(params[0]);
	if (user.getIsRegistered() == 1)
	{
		std::string err = ":server 001 " + user.getNick() + " :Welcome to the Internet Relay Network " + user.getNick() + "\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		user.setIsRegistered(2);
	}
}

void	Command::cap(User &user, std::string prefix, std::vector<std::string> params)
{
	if (prefix.length() > 0)
	{
		std::cout << "PREFIX = " << prefix << std::endl;
	}
	if (params.size() >= 0)
	{
		std::string err = ":server 461 " + user.getNick() + " CAP :Not enough parameters\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	}
	std::string subcommand = params[0];
	std::string cap = params[1];
	if (subcommand == "LS")
	{
		//std::string cap_msg = ":localhost CAP " + user.getNick() + " LS :cap-notify\r\n";
		std::cout << red << "CAP = " << cap << std::endl;
		std::string cap_end = ":localhost CAP * LS :multi-prefix sasl\r\n";
		send(user.getFd(), cap_end.c_str(), cap_end.length(), 0);
	}
	else if (subcommand == "REQ")
	{
		//std::string cap_msg = ":localhost CAP " + user.getNick() + " ACK :cap-notify\r\n";
		std::string cap_end = ":localhost CAP * ACK :multi-prefix\r\n";
		send(user.getFd(), cap_end.c_str(), cap_end.length(), 0);
	}
	else if (subcommand == "END")
	{
		//std::string cap_msg = ":localhost CAP " + user.getNick() + " END\r\n";
		std::string cap_end = ":localhost CAP * ACK :none\r\n";
		send(user.getFd(), cap_end.c_str(), cap_end.length(), 0);
	}
	else
	{
		std::string err = ":server 410 " + user.getNick() + " :Invalid CAP subcommand\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	}
}


void	Command::join(User &user, std::string prefix, std::vector<std::string> params)
{
	(void) prefix;
	if (params.size() < 1)
	{
		std::string err = ":server 461 " + user.getNick() + " JOIN :Not enough parameters\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	}
	std::string channel = params[0];
	if (channel[0] != '#')
	{
		std::string err = ":server 403 " + user.getNick() + " :No such channel\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	}
	if (server.getChannel(channel) == nullptr)
	{
		server.createChannel(channel, user);
		this->_channel = server.getChannel(channel);
	}
	else
	{
		this->_channel = server.getChannel(channel);
		if (!this->_channel->getModeI() || (this->_channel->getModeI() && user.isInvited(this->_channel->getName())))
		{
			if (this->_channel->getModeI() && !user.isInvited(this->_channel->getName()))
			{
				std::string ERR_INVITEONLYCHAN = "server 473 " + user.getNick() + " " + this->_channel->getName() + " :Cannot join channel (+i)\r\n";
				send(user.getFd(), ERR_INVITEONLYCHAN.c_str(), ERR_INVITEONLYCHAN.length(), 0);
				return;
			}
			if (!this->_channel->getModeK() || (this->_channel->getModeK() && (params[1] == this->_channel->getPassword())))
			{
				if (this->_channel->getModeK() && (params[1] != this->_channel->getPassword()))
				{
					std::string ERR_BADCHANNELKEY  = "server 475 " + user.getNick() + " " + this->_channel->getName()  + " :Cannot join channel (+k)\r\n";
					send(user.getFd(), ERR_BADCHANNELKEY.c_str(), ERR_BADCHANNELKEY.length(), 0);
					return;
				}

				if (this->_channel->getSize() == this->_channel->getMax())
				{
					std::string ERR_CHANNELISFULL = "server 471 " + user.getNick() + " " + this->_channel->getName()  + " :Cannot join channel (+l)\r\n";
					send(user.getFd(), ERR_CHANNELISFULL.c_str(), ERR_CHANNELISFULL.length(), 0);
					return;
				}
				if (this->_channel->isKicked(user))
				{
					std::string ERR_BANNEDFROMCHAN = "server 474 " + user.getNick() + " " + this->_channel->getName() + " :Cannot join channel after being kicked\r\n";
					send(user.getFd(), ERR_BANNEDFROMCHAN.c_str(), ERR_BANNEDFROMCHAN.length(), 0);
					return;
				}
				this->_channel->addUser(user);
			}
		}
	}
	std::string createMessage = ":" + user.getNick() + " JOIN " + this->_channel->getName() + "\r\n";
	std::string topicMessage = ":" + user.getNick() + " " + this->_channel->getName() + " : " + this->_channel->getTopic() + "\r\n";
	std::string listUsersMessage = ":" + user.getNick() + " " + this->_channel->getName() + " : " + this->_channel->getList() + "\r\n";
	send(user.getFd(), createMessage.c_str(), createMessage.length(), 0);
	send(user.getFd(), topicMessage.c_str(), topicMessage.length(), 0);
	send(user.getFd(), listUsersMessage.c_str(), listUsersMessage.length(), 0);

	std::cout << "JOIN = " << channel << std::endl;
}

void Command::handleData(User &user, const std::string& data)
{
	buffer += data;
	std::size_t pos;
	while ((pos = buffer.find("\r\n")) != std::string::npos)
	{
		std::string line = buffer.substr(0, pos);
		buffer = buffer.substr(pos + 2);
		parseLine(user, line);
	}
}

void Command::parseLine(User &user, std::string line)
{
	std::istringstream iss(line);
	std::string prefix, command;
	std::vector<std::string> params;
	user.getFd();
	if (line[0] == ':')
	{
		iss >> prefix;
	}
	iss >> command;
	std::string param;
	std::string lastWord;
	while (iss >> param)
	{
		if (param[0] == ':')
		{
			lastWord = param.substr(1);
			std::string remainder;
			getline(iss, remainder);
			lastWord += remainder;
			params.push_back(lastWord);
			break;
		}
		else
		{
			params.push_back(param);
		}
	}
	Command cmd(server, prefix, command, params);
	//std::cout << cyan << "command = " << cmd.getCommand() << " params = " << cmd.getParams() << reset << std::endl;
	cmd.execute(user);
}

void	Command::who(User &user, std::string prefix, std::vector<std::string> params)
{
	(void)prefix;
	(void)params;
	(void)user;
	server.displayUsers();
}
