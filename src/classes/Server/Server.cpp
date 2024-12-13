#include "Server.hpp"

Server::Server ()
{
    bzero (&adress, sizeof(adress));
    server_fd = 0;
    port = 0;
}

Server::Server (const Server & src)
{
    (void) src;
}

Server::~Server ()
{

}

void Server::init ()
{
    int value = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
        FATAL_ERROR("Error creating server's socket");
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int));
    bzero(&adress, sizeof(adress));
    adress.sin_family = AF_INET;
    adress.sin_addr.s_addr = inet_addr(ip.c_str());
    if (adress.sin_addr.s_addr == (in_addr_t) -1)
        FATAL_ERROR("Error converting ip address");
    adress.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *)&adress, sizeof(adress)) < 0)
        FATAL_ERROR("Error binding server's socket");
    if (listen(server_fd, QUEUED_CONNECTIONS) < 0)
        FATAL_ERROR("Error listening on server's socket");
}
