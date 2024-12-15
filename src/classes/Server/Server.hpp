#ifndef SERVER_H
#define SERVER_H

#include "Utils.hpp"

class Server
{
public:
  Server ();
  ~Server ();
  Server (const Server &src);
  void init ();

  int server_fd;
  string ip;
  uint16_t port;
  struct sockaddr_in adress;
};

#endif