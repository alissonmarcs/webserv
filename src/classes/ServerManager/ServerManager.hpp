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
  void acceptClient (Server *owner);
  Server *isServer (int fd);
  Server * getServer (int index);
  Server * getLastServer (void);
  int getServersSize ();
  void addEmptyServer (void);

private:
  int epoll_fd;
  vector<Server> servers;
  map<int, Client> clients;
};

#endif