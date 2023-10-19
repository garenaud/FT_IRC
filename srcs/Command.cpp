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

const Command::CmdFunc Command::cmdArr[] = {&Command::cap, 
											&Command::join, 
											&Command::pass, 
											&Command::ping, 
											&Command::pong, 
											&Command::nick, 
											&Command::user, 
											&Command::who, 
											&Command::mode};
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

void 	Command::execute(User &user)
{
	std::cout << this->command << std::endl;
    static const std::string arr[] = {"CAP", "JOIN", "PASS", "PING", "PONG", "NICK", "USER", "WHO", "MODE"};
    for (size_t i = 0; i < 9; i++)
    {
        if (this->command == arr[i])
        {
            (this->*cmdArr[i])(user, this->prefix, this->params);
            return;
        }
    }
	std::cout << this->command << " = Commande inconnue" << std::endl;
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
		//std::cout << "PONG " << params[0] << std::endl;
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
	// checks
	(void) prefix;
	if (params.size() < 1)
	{
		std::string ERR_NEEDMOREPARAMS = ":localhost 461 " + user.getNick() + " JOIN :Not enough parameters.\r\n";
		send(user.getFd(), ERR_NEEDMOREPARAMS.c_str(), ERR_NEEDMOREPARAMS.length(), 0);
		return ;
	}
	std::string channel = params[0];
	if (channel[0] != '#')
	{
		std::string ERR_NOSUCHCHANNEL = ":localhost 403 " + user.getNick() + " " + channel + " :No such channel\r\n";
		send(user.getFd(), ERR_NOSUCHCHANNEL.c_str(), ERR_NOSUCHCHANNEL.length(), 0);
		return ;
	}
	// No channel
	if (server.getChannel(channel) == nullptr)
	{
		// Create
		server.createChannel(channel, user);
		this->_channel = server.getChannel(channel);

		// RPL
		std::vector<User> channelUsers = this->_channel->getUsers();
		std::string createMessage = ":" + user.getNick() + "!" + user.getUser() + "@localhost JOIN " + this->_channel->getName() + "\r\n";
		sendChannelUsers(channelUsers, createMessage, user);
		if (this->_channel->getTopic() != "")
		{
			std::string topicMessage = ":localhost 332 " + user.getNick() + " " + this->_channel->getName() + " " + this->_channel->getTopic() + "\r\n";
			send(user.getFd(), topicMessage.c_str(), topicMessage.length(), 0);
		}
		std::string listUsersMessage = ":localhost 353 " + user.getNick() + " " + this->_channel->getName() + " :" + this->_channel->getList() + "\r\n";
		send(user.getFd(), listUsersMessage.c_str(), listUsersMessage.length(), 0);
		std::string endOfNames = ":localhost 366 " + user.getNick() + " " + this->_channel->getName() + " End of /NAMES list.\r\n";
		send(user.getFd(), endOfNames.c_str(), endOfNames.length(), 0);
		return;
	}

	// join active channel
	if (server.getChannel(channel) != nullptr)
	{
		this->_channel = server.getChannel(channel);
		if (this->_channel->getModeI() && !user.isInvited(this->_channel->getName()))
		{
			std::string ERR_INVITEONLYCHAN = "474 " + user.getNick() + " " + channel + " :Cannot join channel (+i)\r\n";
			send(user.getFd(), ERR_INVITEONLYCHAN.c_str(), ERR_INVITEONLYCHAN.length(), 0);
			return;
		}
		else
		{
			if (this->_channel->getModeK() && (params.size() < 2 || params[1] != this->_channel->getPassword())) 
			{
				std::string ERR_BADCHANNELKEY  = "475 " + user.getNick() + " " + this->_channel->getName()  + " :Cannot join channel (+k)\r\n";
				send(user.getFd(), ERR_BADCHANNELKEY.c_str(), ERR_BADCHANNELKEY.length(), 0);
				return;
			}
			else
			{
				if (this->_channel->getSize() >= this->_channel->getMax())
				{
					std::string ERR_CHANNELISFULL = "471 " + user.getNick() + " " + this->_channel->getName()  + " :Cannot join channel (+l)\r\n";
					send(user.getFd(), ERR_CHANNELISFULL.c_str(), ERR_CHANNELISFULL.length(), 0);
					return;
				}
				else
				{
					if (this->_channel->isKicked(user))
					{
						std::string ERR_BANNEDFROMCHAN = "474 " + user.getNick() + " " + this->_channel->getName() + " :Cannot join channel after being kicked\r\n";
						send(user.getFd(), ERR_BANNEDFROMCHAN.c_str(), ERR_BANNEDFROMCHAN.length(), 0);
						return;
					}
					else
					{
						// adduser
						this->_channel->addUser(user);
						// RPL
						std::vector<User> channelUsers = this->_channel->getUsers();
						std::string createMessage = ":" + user.getNick() + "!" + user.getUser() + "@localhost JOIN " + this->_channel->getName() + "\r\n";
						sendChannelUsers(channelUsers, createMessage, user);
						if (this->_channel->getTopic() != "")
						{
							std::string topicMessage = ":localhost 332 " + user.getNick() + " " + this->_channel->getName() + " " + this->_channel->getTopic() + "\r\n";
							send(user.getFd(), topicMessage.c_str(), topicMessage.length(), 0);
						}
						std::string listUsersMessage = ":localhost 353 " + user.getNick() + " " + this->_channel->getName() + " :" + this->_channel->getList() + "\r\n";
						send(user.getFd(), listUsersMessage.c_str(), listUsersMessage.length(), 0);
						std::string endOfNames = ":localhost 366 " + user.getNick() + " " + this->_channel->getName() + " End of /NAMES list.\r\n";
						send(user.getFd(), endOfNames.c_str(), endOfNames.length(), 0);
					}
				}
			}
		}
	}
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
    cmd.execute(user);
}

void	Command::who(User &user, std::string prefix, std::vector<std::string> params)
{
	(void)prefix;
	(void)params;
	(void)user;
	server.displayUsers();
}

void	Command::mode(User &user, std::string prefix, std::vector<std::string> params)
{
	////// GENERAL CHECKS ////////////////////////////////////////////////////////////////////////////////////
	(void)prefix;
	if (params.size() < 2)
	{
		std::string err = ":server 461 " + user.getNick() + " MODE :Not enough parameters\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	}
	if (params[0] == user.getNick())
		return;
	if (params[1] == "b")
		return;
	std::string channel = params[0];
	if (server.getChannel(channel) == nullptr)
	{
		std::string ERR_NOSUCHCHANNEL = ":server 403 " + user.getNick() + " :No such channel\r\n";
		send(user.getFd(), ERR_NOSUCHCHANNEL.c_str(), ERR_NOSUCHCHANNEL.length(), 0);
		return ;
	}
	this->_channel = server.getChannel(channel);
	if (!server.getChannel(channel)->isChanops(user))
	{
		std::string ERR_CHANOPRIVSNEEDED = ":server 482 " + user.getNick() + " " + this->_channel->getName() + " :You're not channel operator\r\n";
		send(user.getFd(), ERR_CHANOPRIVSNEEDED.c_str(), ERR_CHANOPRIVSNEEDED.length(), 0);
		return ;
	}
	if ((params[1][0] != '+' && params[1][0] != '-')
			|| (params[1][1] != 'i' && params[1][1] != 't' && params[1][1] != 'k'
			&& params[1][1] != 'o' && params[1][1] != 'l'))
	{
		std::string ERR_INVALIDMODEPARAM = ":" + user.getNick() + " " + this->_channel->getName() + " MODE " + params[1] + " :Invalid parameter.\r\n";
		send(user.getFd(), ERR_INVALIDMODEPARAM.c_str(), ERR_INVALIDMODEPARAM.length(), 0);
		return;
	}
	if ((params[1][1] == 'o' || (params[1][0] == '+' && (params[1][1] == 'k' || params[1][1] == 'l'))) && params.size() < 3)
	{
		std::string err = ":server 461 " + user.getNick() + " JOIN :Not enough parameters\r\n";
		send(user.getFd(), err.c_str(), err.length(), 0);
		return ;
	}
	std::vector<User> channelUsers = this->_channel->getUsers();

	///// OPTION +o ///////////////////////////////////////////////////////////////////////////////////////////
	if (params[1][1] == 'o')
	{
		User *paramUser = server.getUserByNick(params[2]);
		if (!this->_channel->isUser(*paramUser))
		{
			std::string ERR_USERNOTINCHANNEL = ":server 441 " + user.getNick() + " " + paramUser->getNick() + " " + this->_channel->getName() + " :They aren't on that channel\r\n";
			send(user.getFd(), ERR_USERNOTINCHANNEL.c_str(), ERR_USERNOTINCHANNEL.length(), 0);
			return;
		}
		else
		{
			if (params[1][0] == '+')
			{
				this->_channel->addChanops(*paramUser, user);
				paramUser->addOperatorChannel(this->_channel->getName());
				std::string msg = params[2] + " is now operator in " + params[0] + "\r\n";
				sendChannelUsers(channelUsers, msg, user);
				return;
			}
			if (params[1][0] == '-')
			{
				this->_channel->rmChanops(*paramUser);
				paramUser->rmOperatorChannel(params[0]);
				std::string msg = params[2] + " is not operator anymore in " + params[0] + "\r\n";
				sendChannelUsers(channelUsers, msg, user);
				return;
			}

		}
	}

	//////// OPTION +k / +l /////////////////////////////////////////////////////////////////////////////
	if (params[1][0] == '+' && (params[1][1] == 'k' || params[1][1] == 'l'))
	{
		if (params[1][1] == 'k')
		{
			if (!checkPassword(params[2]))
			{
				std::string ERR_INVALIDMODEPARAM = ":server 696 " + user.getNick() + " " + this->_channel->getName() + params[1] + params[2] + " :Invalid key\r\n";
				send(user.getFd(), ERR_INVALIDMODEPARAM.c_str(), ERR_INVALIDMODEPARAM.length(), 0);
				return ;
			}
			this->_channel->setRmMode("+k");
			this->_channel->setPassword(params[2]);
			std::string msg = ":localhost 324 " + user.getNick() + " " + this->_channel->getName() + " +k " + params[2] + "\r\n";
			sendChannelUsers(channelUsers, msg, user);
			return;
		}
		if (params[1][1] == 'l')
		{
			if (params[1][0] == '+')
			{
				int num = std::atoi(params[2].c_str());
				if (num < static_cast<int>(this->_channel->getUsers().size()) || num > INT_MAX || num == 0)
				{
					std::string ERR_INVALIDMODEPARAM = ":server 696 " + user.getNick() + " " + this->_channel->getName() + params[1] + params[2] + " :Invalid user limit\r\n";
					send(user.getFd(), ERR_INVALIDMODEPARAM.c_str(), ERR_INVALIDMODEPARAM.length(), 0);
					return ;
				}
				this->_channel->setRmMode("+l");
				this->_channel->setMax(num);
				std::string msg = user.getNick() + " sets a new limit of users on " + this->_channel->getName() + " : " + params[2] + "\r\n";
				sendChannelUsers(channelUsers, msg, user);
				return;
			}
		}
	}
	if (params[1][0] == '-' && (params[1][1] == 'k' || params[1][1] == 'l'))
	{
		if (params[1][1] == 'k')
		{
			this->_channel->setPassword("");
			this->_channel->setRmMode("-k");
			std::string msg = user.getNick() + " removes key of " + this->_channel->getName() + "\r\n";
			sendChannelUsers(channelUsers, msg, user);
			return;
		}
		if (params[1][1] == 'l')
		{
			this->_channel->setMax(INT_MAX);
			this->_channel->setRmMode("-l");
			std::string msg = user.getNick() + " removes limit of user on " + this->_channel->getName() + "\r\n";
			sendChannelUsers(channelUsers, msg, user);
			return;
		}
	}

	///////////// OPTION +i ////////////////////////////////////////////////////////////////////////////////
	if (params[1][1] == 'i')
	{
		if (params[1][0] == '-')
		{
			this->_channel->setRmMode("-i");
			for (std::vector<User>::iterator it = channelUsers.begin(); it != channelUsers.end(); ++it)
			{
				it->rmInvitedChannel(this->_channel->getName());
			}
			std::string msg = user.getNick() + " removes the invited-only mode on " + this->_channel->getName() + "\r\n";
			sendChannelUsers(channelUsers, msg, user);
			return;
		}
		if (params[1][0] == '+')
		{
			this->_channel->setRmMode("+i");
			for (std::vector<User>::iterator it = channelUsers.begin(); it != channelUsers.end(); ++it)
			{
				it->addInvitedChannel(this->_channel->getName());
			}
			std::string msg = user.getNick() + " sets the invited-only mode on " + this->_channel->getName() + "\r\n";
			sendChannelUsers(channelUsers, msg, user);
			return;
		}
	}

	////////// OPTION +t //////////////////////////////////////////////////////////////////////////////////
	if (params[1][1] == 't')
	{
		if (params[1][0] == '-')
		{
			this->_channel->setRmMode("-t");
			std::string msg = user.getNick() + " removes the protected topic mode on " + this->_channel->getName() + "\r\n";
			sendChannelUsers(channelUsers, msg, user);
			return;
		}
		if (params[1][0] == '+')
		{
			this->_channel->setRmMode("+t");
			std::string msg = user.getNick() + " sets the protected topic mode on " + this->_channel->getName() + "\r\n";
			sendChannelUsers(channelUsers, msg, user);
			return;
		}
	}
	else
		return;
}

//  /MODE <channel> <+/- mode> [parametres]
// command   p[0]      p[1]      p[2, ...]

void	Command::sendChannelUsers(std::vector<User> channelUsers, std::string msg, User user) const
{
	for (int i = 0; i < static_cast<int>(channelUsers.size()); i++)
	{
		if (channelUsers[i].getNick() != user.getNick())
			send(channelUsers[i].getFd(), msg.c_str(), msg.length(), 0);
	}
}

bool	checkPassword(std::string passWord)
{
	if (passWord.length() > 20)
		return false;
	for (size_t i = 0; i < passWord.length(); ++i)
	{
		char c = passWord[i];
		if (!isalnum(c) && !ispunct(c)) 
			return false;
		if (isspace(c))
			return false;
	}
	return true;
}