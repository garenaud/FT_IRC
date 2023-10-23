#include "srcs/Server.hpp"
#include "srcs/Msg.hpp"
#include "srcs/Command.hpp"

int main(int argc, char *argv[])
{
	if (argc == 3 && std::string(argv[1]).std::string::find_first_not_of("0123456789",0) == std::string::npos)
	{
		//Msg message;// ?
		Server aServer;
		aServer.setPasswd(argv[2]);
		aServer.setPort(atoi(argv[1]));
		aServer.run();
	}
	else
	{
		std::cout << red << "Usage: ./Servers port password\n" << reset;
		return (-1);
	}
}
