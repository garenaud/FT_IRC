#ifndef USER_HPP
# define USER_HPP
#include <iostream>
#include "Server.hpp"
//#include "Msg.hpp"
#include "Channel.hpp"

class Channel;

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
		void		addJoinedChannel(Channel *channel);
		void		removeJoinedChannel(Channel *channel);
		void 		sendAllJoinedChannels(std::string msg);
		std::vector<Channel *>	getJoinedChannels();
=======
		void		addJoinedChannel(std::string channelName);
		void		removeJoinedChannel(std::string channelName);
		void 		sendAllJoinedChannels(std::string msg) const;
		std::vector<std::string>	getJoinedChannels() const;
>>>>>>> 7cd873ba13ac3faa27fcbee5e6310d6ffd593394
		void		addOperatorChannel(std::string channelName);
		void		rmOperatorChannel(std::string channelName);
		bool		isOperator(std::string channelName) const;
		bool 		getIsAlive() const;

	private:
		const int	fd;
		std::string	nick;
		std::string	user;
		std::string	passwd;
		std::string	realname;
		std::string	hostname;
		std::string mode;
		int	isRegistered;
		bool isAlive;

		std::vector<std::string>	invitedChannels;
<<<<<<< HEAD
		std::vector<Channel *>		joinedChannels;
=======
		std::vector<std::string>	joinedChannels;
>>>>>>> 7cd873ba13ac3faa27fcbee5e6310d6ffd593394
		std::vector<std::string>	operatorChannels;
};

std::ostream& operator<<(std::ostream& o, User const &src);

#endif