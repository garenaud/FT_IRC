#include "Msg.hpp"

Msg::Msg() {};
Msg::~Msg() {};

bool	Msg::is_complete(char * buff, size_t size)
{
	bool	result = false;

	for (size_t i=0; i < size - 1; i++)
	{
		if (buff[i] == '\r')
		{
			if (buff[i+1] == '\n')
			{
				result = true;
				break ;
			}
		}
	}
	return (result);
}

size_t	Msg::size_buffer(char * buff, size_t size)
{// a verifier
	size_t	result = size;

	for (size_t i=0; i < size - 1; i++)
	{
		if (buff[i] == '\r')
		{
			if (buff[i+1] == '\n')
			{
				result = i + 1 ;
				break ;
			}
		}
	}
	return (result);
}
/*
int		Msg::load_buffer(char * buff, size_t size)
{
	//??
	//char	tmp[512];
	size_t	size_buff = size_buffer(buff, size);
	if (size_buff < size)
	{
		for (size_t i=0; i< size_buff; i++)
		{
			this->buffer_out[i] = buff[i];
		}
		if (size_buffer(this->buffer_out, size) == size_buff)
		{
			// ok ?
			size_t	j = 0;
			for (size_t i = size_buff +1; i < size; i++)
			{
				this->buffer_in[j] = buff[i];
			}
			for (size_t i = j + 1; i < size; i++)
			{
				this->buffer_in[i] = '\0';
			}
			return (size_buff);// la taille jusqu'a \r\n
		}
		else {return (0);} //erreur
	}
	else
	{}
}
*/
void	Msg::trim_buffer(void)
{}
int		Msg::initialize(int acc_socket, std::string user, char * buff, int recv_size)
{
	this->aMessage.accepted_socket = acc_socket;
	this->aMessage.userID = user;
	this->aMessage.recv_size = recv_size;
	for (size_t i=0; i < 512; i++)
	{
		this->aMessage.buffer_in[i] = buff[i];
		//std::cout << yellow << this->aMessage.buffer_in[i] << reset;
	}
	//std::cout <<std::endl;
	this->received_message.push_back(this->aMessage);
	return (0);
}

void	Msg::view(void)
{
	std::cout << "Impression des infos du message:\n";
	std::cout << "reception sur le soccket : " << this->aMessage.accepted_socket << std::endl;
	std::cout << "userID : " << this->aMessage.userID << std::endl;
	std::cout << "taille du message (recv) = "<< this->aMessage.recv_size << std::endl;
	for (size_t i=0; i < this->aMessage.recv_size; i++)
	{
		if (this->aMessage.buffer_in[i] !=  '\r' && this->aMessage.buffer_in[i] !=  '\n')
		{
			std::cout << green << this->aMessage.buffer_in[i] << reset;
		}
		else if (this->aMessage.buffer_in[i] ==  '\r' ) {std::cout<< red<<'r' << reset;}
		else if (this->aMessage.buffer_in[i] ==  '\n' ) {std::cout<< red<<'n' << reset;}
	}

}

void	Msg::split(/*incomingMessage aMess,*/ std::string sep)
{
	// fais gaffe, length != recv_size !!
	std::string tmp(received_message[0].buffer_in);
	//std::string	tmp1(aMessage.buffer_in);
	size_t	occurence = 0;
	size_t	pos = 0;
	std::vector<int>	block;
	block.push_back(0);
	while ((pos = tmp.find(sep, pos )) != std::string::npos)
	{
		++ occurence;
		if (pos < tmp.length())
		{
			block.push_back(pos);
		}
		//pos += target.length();
		++ pos;
	}
	//block.push_back(aMessage.recv_size);
	block.push_back(received_message[0].recv_size);
	//block.push_back(tmp.length());
	for (std::vector<int>::const_iterator i = block.begin(); i != block.end(); ++i)
	std::cout << cyan << *i << ' ';
	std::cout << yellow << occurence << reset << std::endl;
	block.clear();


}

void	Msg::test(void)
{
	strcpy(this->buffer_in,"coucou c'est moi \r\n   ");
	std::cout << std::boolalpha << this->buffer_in << "  "<< is_complete((this->buffer_in), 512)<< std::noboolalpha << std::endl;
	strcpy(this->buffer_out,"coucou c'est moi \n");
	std::cout << std::boolalpha << this->buffer_out << "  "<< is_complete((this->buffer_out), 512)<< std::noboolalpha << std::endl;
}
