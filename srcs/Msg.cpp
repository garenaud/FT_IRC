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
{
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
	}
	this->received_message.push_back(this->aMessage);
	return (0);
}
void	Msg::test(void)
{
	strcpy(this->buffer_in,"coucou c'est moi \r\n   ");
	std::cout << std::boolalpha << this->buffer_in << "  "<< is_complete((this->buffer_in), 512)<< std::noboolalpha << std::endl;
	strcpy(this->buffer_out,"coucou c'est moi \n");
	std::cout << std::boolalpha << this->buffer_out << "  "<< is_complete((this->buffer_out), 512)<< std::noboolalpha << std::endl;
}
