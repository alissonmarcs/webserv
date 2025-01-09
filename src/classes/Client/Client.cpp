#include "Client.hpp"

Client::Client ()
{
  memset (&adress, 0, sizeof (adress));
  client_fd = 0;
}

Client::Client (int fd, struct sockaddr_in addr, Server *owner)
    : last_read(0), client_fd (fd), adress (addr), server_owner (owner), error_code (0), is_request_parsing_done (false), is_chunked (false)
{
}

Client::Client (const Client &src) { (void)src; }

Client::~Client () {}