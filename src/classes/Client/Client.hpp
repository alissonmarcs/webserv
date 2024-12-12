#ifndef CLIENT_H
#define CLIENT_H

#include "Utils.hpp"
#include "Server.hpp"

class Client
{
public:
    Client ();
    Client (int fd, struct sockaddr_in addr, Server * owner);
    ~Client ();
    Client (const Client & src);

    int client_fd;
    struct sockaddr_in adress;
    Server * server_owner;

};

#endif