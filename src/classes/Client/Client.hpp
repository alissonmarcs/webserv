#ifndef CLIENT_H
#define CLIENT_H

#include "Server.hpp"
#include "Utils.hpp"

class Server;

class Client
{
public:
  Client ();
  Client (int fd, struct sockaddr_in addr, Server *owner);
  ~Client ();
  Client (const Client &src);

private:
  /* Connection */
  int client_fd;
  struct sockaddr_in adress;
  Server *server_owner;

  /* Request */
  map<string, string> request_headers;
};

#endif