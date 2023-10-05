#ifndef ICOMMAND_HPP
# define ICOMMAND_HPP
#include <iostream>
#include <vector>
#include "User.hpp"
#include "Server.hpp"

class Command
{
	public:
		Command(Server &server, std::string prefix, std::string command, std::vector<std::string> params);
		~Command();
		void		setPrefix(std::string prefix);
		std::string	getPrefix();
		void		setCommand(std::string command);
		std::string	getCommand();
		void		setParams(std::vector<std::string> params);
		std::string	getParams();
		void		execute(User user);
		void		nick(User user, std::vector<std::string> params);
		void		ping(User user, std::vector<std::string> params);
		void		user(User user, std::vector<std::string> params);

		typedef void (Command::*CmdFunc)(User, std::string, std::vector<std::string>);
		static const CmdFunc cmdArr[];
	private:
		Command();
		Server&						server;
		std::string					prefix;
		std::string					command;
		std::vector<std::string>	params;
};

#endif