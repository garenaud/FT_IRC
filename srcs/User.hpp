#ifndef USER_HPP
# define USER_HPP
#include <iostream>
#include "Server.hpp"
//#include "Msg.hpp"

class User
{
	public:
		User(int fd, std::string nick, std::string user);
		User(int fd);
		~User();
		void		operator=(User const &src);
		bool		operator==(const User& other) const;
		void		setFd(int fd);
		int			getFd() const;
		void		setNick(std::string nick);
		std::string	getNick();
		void		setUser(std::string user);
		std::string	getUser();
		void		setPasswd(std::string passwd);
		std::string	getPasswd();
		void		setRealname(std::string realname);
		std::string	getRealname();
		void		setHostname(std::string hostname);
		std::string	getHostname();
		void		setMode(std::string mode);
		std::string	getMode();
 		void		setIsRegistered(int isRegistered);
		int			getIsRegistered();
		void 		checkRegistration();

		void		addInvitedChannel(std::string channelName);
		void		rmInvitedChannel(std::string channelName);
		bool		isInvited(std::string channelName) const;
<<<<<<< HEAD
		void		addJoinedChannel(std::string channelName);
		void		removeJoinedChannel(std::string channelName);
		void 		sendAllJoinedChannels(std::string msg) const;
		std::vector<std::string>	getJoinedChannels() const;
=======
		void		addOperatorChannel(std::string channelName);
		void		rmOperatorChannel(std::string channelName);
		bool		isOperator(std::string channelName) const;
>>>>>>> ce8a8cb2dc02fd177fda193097c364b8ced124cc

	private:
		const int	fd;
		std::string	nick;
		std::string	user;
		std::string	passwd;
		std::string	realname;
		std::string	hostname;
		std::string mode;
		int	isRegistered;

		std::vector<std::string>	invitedChannels;
<<<<<<< HEAD
		std::vector<std::string>	joinedChannels;
=======
		std::vector<std::string>	operatorChannels;
>>>>>>> ce8a8cb2dc02fd177fda193097c364b8ced124cc
};

std::ostream& operator<<(std::ostream& o, User const &src);

#endif