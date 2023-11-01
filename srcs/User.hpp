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
		int			getFd();
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
		time_t		getLastPing();
		void		setLastPing(time_t lastPing);
		void 		checkRegistration();

		void		addInvitedChannel(std::string channelName);
		void		rmInvitedChannel(std::string channelName);
		bool		isInvited(std::string channelName) const;
		void		addJoinedChannel(Channel *channel);
		void		removeJoinedChannel(Channel *channel);
		void 		sendAllJoinedChannels(std::string msg);
		std::vector<Channel *>	getJoinedChannels();
		void		addOperatorChannel(std::string channelName);
		void		rmOperatorChannel(std::string channelName);
		bool		isOperator(std::string channelName) const;
		bool 		getIsAlive() const;
		std::vector<Channel *>	getJoinedChannels() const;

	private:
		time_t		lastPing;
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
		std::vector<Channel *>		joinedChannels;
		std::vector<std::string>	operatorChannels;
};

std::ostream& operator<<(std::ostream& o, User const &src);

#endif