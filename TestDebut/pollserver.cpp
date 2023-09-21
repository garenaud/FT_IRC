#include "pollserver.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

const char* PORT = "6667"; // Port we're listening on

void *PollServer::get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) 
    {
        return &(reinterpret_cast<struct sockaddr_in*>(sa)->sin_addr);
    }
    return &(reinterpret_cast<struct sockaddr_in6*>(sa)->sin6_addr);
}

int PollServer::get_listener_socket()
{
    int yes = 1;
    int rv;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) 
    {
        std::cerr << "selectserver: " << gai_strerror(rv) << std::endl;
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) 
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) 
        { 
            continue;
        }

        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) 
        {
            close(listener);
            continue;
        }

        break;
    }
    if (p == NULL) 
    {
        return -1;
    }
    freeaddrinfo(ai);
    if (listen(listener, 10) == -1) 
    {
        return -1;
    }

    return listener;
}

void PollServer::add_to_pfds(int newfd)
{
    pollfd newPfd;
    newPfd.fd = newfd;
    newPfd.events = POLLIN;
    pfds.push_back(newPfd);
}

void PollServer::del_from_pfds(int index)
{
    pfds.erase(pfds.begin() + index);
}

PollServer::PollServer()
{
    listener = get_listener_socket();
    if (listener == -1) {
        std::cerr << "Error getting listener socket" << std::endl;
        exit(1);
    }

    pollfd listenerPfd;
    listenerPfd.fd = listener;
    listenerPfd.events = POLLIN;
    pfds.push_back(listenerPfd);
}

void PollServer::run()
{
    char buf[256];
    char remoteIP[INET6_ADDRSTRLEN];
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;

    for(;;) 
    {
        int poll_count = poll(&pfds[0], pfds.size(), -1);

        if (poll_count == -1) 
        {
            perror("poll");
            exit(1);
        }

        for(size_t i = 0; i < pfds.size(); ++i)
        {
            if (pfds[i].revents & POLLIN) 
            {
                if (pfds[i].fd == listener) 
                {
                    addrlen = sizeof remoteaddr;
                    int newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        add_to_pfds(newfd);

                        std::cout << "pollserver: new connection from "
                            << inet_ntop(remoteaddr.ss_family,
                                get_in_addr((struct sockaddr*)&remoteaddr),
                                remoteIP, INET6_ADDRSTRLEN)
                            << " on socket " << newfd << std::endl;
                    }
                } else {
                    int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);
                    int sender_fd = pfds[i].fd;

                    if (nbytes <= 0) {
                        if (nbytes == 0) {
                            std::cout << "pollserver: socket " << sender_fd << " hung up" << std::endl;
                        } else {
                            perror("recv");
                        }
                        close(pfds[i].fd);
                        del_from_pfds(i);
                    } else {
                        for(size_t j = 0; j < pfds.size(); ++j) {
                            int dest_fd = pfds[j].fd;
                            if (dest_fd != listener && dest_fd != sender_fd) {
                                if (send(dest_fd, buf, nbytes, 0) == -1) {
                                    perror("send");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
