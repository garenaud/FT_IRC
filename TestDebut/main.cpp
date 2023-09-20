#include "Server.hpp"

int main(int argc, char *argv[])
{
    if (argc == 3)
    {
        Server aServer;
        aServer.setPasswd(argv[2]);
        aServer.setPort(atoi(argv[1]));
    }
    else
    return (-1);
}
