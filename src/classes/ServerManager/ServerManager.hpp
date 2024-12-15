#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include "Client.hpp"
#include "Server.hpp"

class ServerManager
{
public:
  ServerManager ();
  ~ServerManager ();
  ServerManager (const ServerManager &src);

  void mainLoop ();
  void initServers ();
  Server *isServer (int fd);
  void acceptClient (Server *owner);

  int epoll_fd;
  vector<Server> servers;
  map<int, Client> clients;
};

#endif