#ifndef MSG_HPP
# define MSG_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <vector>
#include <deque>
#include <map>
#include <algorithm>

const std::string red("\033[0;31m");
const std::string green("\033[1;32m");
const std::string yellow("\033[1;33m");
const std::string cyan("\033[0;36m");
const std::string magenta("\033[0;35m");
const std::string reset("\033[0m");

typedef	struct	incomingMessage
{
	int					accepted_socket; //pour identifier d'ou cela vient du cote du serveur
	std::string			userID;
	char				buffer_in[512];
	size_t				recv_size; //la qut de caracteres recus
//	long int			date; // a voir si utile date de reception + format
	std::deque<char>	buffer;
} incomingMessage;

typedef	struct	outgoingMessage
{
	int					accepted_socket; //pour identifier d'ou cela vient du cote du serveur
	std::string			userID;
	std::string			message;
	char				buffer_in[512];
	size_t				message_size; //la qut de caracteres recus
	//long int			date; // a voir si utile date de reception + format
} outgoingMessage;

class Msg
{
	public:
		Msg();
		~Msg();
		bool	is_complete(char * buff, size_t size);//
		size_t	size_buffer(char * buff, size_t size);//
		int		load_buffer(char * buff, size_t size);
		void	trim_buffer();
		void	test();
		void	view();//
		void	split(/*incomingMessage aMess,*/ std::string sep);

		int		initialize(int acc_socket, std::string user, char * buff, int recv_size);

	private:
		char									buffer_in[512];
		char									buffer_out[512];
		std::deque<char>						buffer;
		incomingMessage							aMessage;
		outgoingMessage							uniqueMessage;
		std::deque<incomingMessage>				received_message;
		std::deque<outgoingMessage>				message_list;
};

#endif
