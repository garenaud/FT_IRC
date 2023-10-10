#ifndef USER_HPP
# define USER_HPP
#include <iostream>

class User
{
	public:
		User(int fd, std::string nick, std::string user);
		~User();
		void		setFd(int fd);
		int			getFd();
		void		setNick(std::string nick);
		std::string	getNick();
		void		setUser(std::string user);
		std::string	getUser();
		void		setPasswd(std::string passwd);
		std::string	getPasswd();
/* 		void		setIsRegistered();
		bool		getIsRegistered(); */

	private:
		User();
		int	fd;
		std::string	nick;
		std::string	user;
		std::string	passwd;
		bool	isRegistered;
};

std::ostream& operator<<(std::ostream& o, User const &src);

#endif