#include "Client.hpp"

Client::Client ()
{
  memset (&adress, 0, sizeof (adress));
  client_fd = 0;
}

Client::Client (int fd, struct sockaddr_in addr, Server *owner)
    : client_fd (fd), adress (addr), server_owner (owner)
{
}

Client::Client (const Client &src) { (void)src; }

Client::~Client () {}