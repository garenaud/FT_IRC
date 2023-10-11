#ifndef ICOMMAND_HPP
# define ICOMMAND_HPP
#include <iostream>
#include <vector>
#include "User.hpp"
#include "Server.hpp"

class User;
class Server;

class Command
{
	public:
		Command(Server &server, std::string prefix, std::string command, std::vector<std::string> params);
		Command(Server &server);
		~Command();
		void		setPrefix(std::string prefix);
		std::string	getPrefix();
		void		setCommand(std::string command);
		std::string	getCommand();
		void		setParams(std::vector<std::string> params);
		std::string	getParams();
		void		execute(User user);
		void		pass(User user, std::string prefix, std::vector<std::string> params);
		void		nick(User user, std::string prefix, std::vector<std::string> params);
		void		ping(User user, std::string prefix, std::vector<std::string> params);
		void		user(User user, std::string prefix, std::vector<std::string> params);
		void 		cap(User user, std::string prefix, std::vector<std::string> params);
		void		join(User user, std::string prefix, std::vector<std::string> params);
		//void		registration(User user);
		void		handleData(User user, const std::string& data);
		void		parseLine(User user, std::string line);

		typedef void (Command::*CmdFunc)(User, std::string, std::vector<std::string>);
		static const CmdFunc cmdArr[];
		std::string		buffer;
	private:
		Server						&server;
		std::string					prefix;
		std::string					command;
		std::vector<std::string>	params;
};

#endif