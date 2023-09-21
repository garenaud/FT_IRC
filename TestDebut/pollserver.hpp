#ifndef POLLSERVER_HPP
#define POLLSERVER_HPP

#include <vector>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

class PollServer {
private:
    int listener;
    std::vector<pollfd> pfds;
    
    void *get_in_addr(struct sockaddr *sa);
    int get_listener_socket();
    void add_to_pfds(int newfd);
    void del_from_pfds(int index);
public:
    PollServer();
    void run();
};

#endif // POLLSERVER_HPP
