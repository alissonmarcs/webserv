#include "Client.hpp"

Client::Client ()
{
  memset (&adress, 0, sizeof (adress));
  client_fd = 0;
}

Client::Client (int fd, struct sockaddr_in addr, Server *owner)
    : is_request_parsing_done (false), response_is_done (false), last_read (time (NULL)), 
    client_fd (fd), adress (addr), server_owner (owner), status_code (0),
    is_chunked (false), is_sized (false), route (NULL)
{
  memset (&file_info, 0, sizeof (file_info));
  memset (&env_vars, 0, sizeof (env_vars));
}

Client::Client (const Client &src) { (void)src; }

Client::~Client () {}
