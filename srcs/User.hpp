#ifndef USER_HPP
# define USER_HPP
#include <iostream>

//#include "Msg.hpp"

class User
{
	public:
		User(int fd, std::string nick, std::string user);
		User(int fd);
		~User();
		void		operator=(User const &src);
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

	private:
		const int	fd;
		std::string	nick;
		std::string	user;
		std::string	passwd;
		std::string	realname;
		std::string	hostname;
		std::string mode;
		int	isRegistered;
};

std::ostream& operator<<(std::ostream& o, User const &src);

#endif