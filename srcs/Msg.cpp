#include "Msg.hpp"

size_t	Msg::incomingCount = 0;
size_t	Msg::outgoingCount = 0;

Msg::Msg() {};
Msg::~Msg() {};

std::string		Msg::getMessage()
{
	//return (this->uniqueMessage.message);
	std::string	tmp = this->message_list[0].message;
	//std::cout << red << tmp << reset << std::endl;
	this->message_list.pop_front();
	return (tmp);
}

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
int		Msg::initialize(int acc_socket, User &user, char * buff, int recv_size)
{
	this->aMessage.accepted_socket = acc_socket;
	this->aMessage.user = &user;
	//std::cout << green << "user = " << this->aMessage.user->getUser() << reset << std::endl;
	this->aMessage.recv_size = recv_size;
//	this->aMessage.buffer.clear(); // ajout 0910
	for (size_t i=0; i < 512; i++)
	{
		this->aMessage.buffer_in[i] = buff[i];
	}
	this->received_message.push_back(this->aMessage);
	incomingCount++;//
	return (0);
}

void	Msg::view(void)
{
	std::cout << "Impression des infos du message:\n";
	std::cout << "reception sur le socket : " << this->aMessage.accepted_socket << std::endl;
	std::cout << "userID : " << this->aMessage.user << std::endl;
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
void	Msg::split2(std::string sep)
{
		std::string tmp(received_message[0].buffer_in);
		tmp = tmp.substr(0, this->aMessage.recv_size);
		//std::cout << red << "\nmessage recu \t" <<tmp<< reset << std::endl;
		//std::string	tmp1 = ""; // ajout 0910
		std::string tmp1(this->aMessage.buffer.begin(), this->aMessage.buffer.end());
		size_t	found;
		while ((found = tmp.find(sep)) != std::string::npos)
		{
			tmp1.append(tmp.substr(0, found));
			this->aMessage.buffer.clear();
			//tmp1 = tmp.substr(0, found);//
			//std::cout << green << "\t\t\t" << tmp1 << std::endl;
			tmp.replace(0, found+2, "");// pb ici
			// transfer ok (lldb)
			this->uniqueMessage.accepted_socket = this->aMessage.accepted_socket;
			this->uniqueMessage.user = this->aMessage.user;
			this->uniqueMessage.message = tmp1;
			this->uniqueMessage.message_size = tmp1.length();
			this->message_list.push_back(this->uniqueMessage);
			tmp1 = "";
		}
		for (size_t i = 0; i < tmp.length(); ++i)
		{
			this->aMessage.buffer.push_back(tmp[i]);
		}
		for (size_t i = 0; i < this->aMessage.buffer.size(); ++i)
		{
			std::cout << yellow << this->aMessage.buffer[i] << std::endl;
		}
		//std::cout << red << tmp << std::endl;

		this->received_message.erase(received_message.begin());

}


void	Msg::split(std::string sep)
{// deprecated

	std::string tmp(received_message[0].buffer_in);
	tmp = tmp.substr(0, this->aMessage.recv_size); // **

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
		++ pos;
	}

	block.push_back(received_message[0].recv_size);
	// partie split
	for (size_t i = 0; i < block.size() - 1; ++i)
	{
		size_t	start;
		size_t	end = block[i+1];
		if (i == 0)
			start = 0;
		else {start = block[i] + 2;};

		std::string tmp1 = tmp.substr(start, end - start);
		if (end - start != 0)
		{
			// transfert vers outgoing message.
			this->uniqueMessage.accepted_socket = this->aMessage.accepted_socket;
			this->uniqueMessage.user = this->aMessage.user;
			this->uniqueMessage.message = tmp1;
			this->uniqueMessage.message_size = tmp1.length();
			this->message_list.push_back(this->uniqueMessage);
		//	occurence--;
		// test effacement queue incoming segfault
		//	std::cout << "\n Trf  avant " << red << this->received_message.size() << std::endl;
		//	if (occurence == 0)
		//		this->received_message.pop_front();// foire
		//	std::cout << "\n Trf apres " << red << this->received_message.size() << std::endl;
		//	incomingCount--;// calcul faux
		//
		}
		if ((i == block.size() - 1) && end < tmp.length() -2)
		{
			std::cout << red << "message incomplet\n" << reset << std::endl;
			for (size_t j = start; j < tmp.length(); ++j)
			{
				this->aMessage.buffer.push_back(tmp1[j]);
			}
			std::cout << red << this->aMessage.buffer.size() << reset << std::endl;
		}
		std::cout << std::endl << magenta << tmp1 << std::endl;//**
	}

	for (size_t i = 0; i < this->message_list.size(); ++i)
	{
		std::cout << i << "   "<<red << this->message_list[i].message << reset << std::endl;
	}
	for (std::vector<int>::const_iterator i = block.begin(); i != block.end(); ++i)
	{
		std::cout << cyan << *i << ' ';
	}

	//std::cout << yellow << occurence << reset << std::endl;
	block.clear();
	this->received_message.erase(received_message.begin());//

	/*for (size_t i = 0; i < 512; ++i)
	{
		received_message[0].buffer_in[i] = '\0';
	}*/


}

void	Msg::test(void)
{
	strcpy(this->buffer_in,"coucou c'est moi \r\n   ");
	std::cout << std::boolalpha << this->buffer_in << "  "<< is_complete((this->buffer_in), 512)<< std::noboolalpha << std::endl;
	strcpy(this->buffer_out,"coucou c'est moi \n");
	std::cout << std::boolalpha << this->buffer_out << "  "<< is_complete((this->buffer_out), 512)<< std::noboolalpha << std::endl;
}

int		Msg::getMessageSize()
{
	return (this->message_list.size());
}
