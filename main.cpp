#include "srcs/Server.hpp"
#include "srcs/Msg.hpp"
#include "srcs/Command.hpp"

int main(int argc, char *argv[])
{
    if (argc == 3)
    {
        Msg message;
        message.test();
        Server aServer;
        aServer.setPasswd(argv[2]);
        aServer.setPort(atoi(argv[1]));
        aServer.run();
    }
    else
    return (-1);
}