#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include "Utils.hpp"
#include "Client.hpp"
#include "Server.hpp"

class Client;
class Server;

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
  Server *getServer (int index);
  Server *getLastServer (void);
  int getServersSize ();
  void addEmptyServer (void);
  vector<Server> &getServersRef ();

  void readFromClient (Client & client);

  void checkIOEvents (int ready_fds, struct epoll_event *events);
  void checkTimeout ();

private:
  int epoll_fd;
  vector<Server> servers;
  map<int, Client> clients;
};

#endif