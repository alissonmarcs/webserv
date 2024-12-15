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

  /* Getter and Setters */
  int getServerFd ();
  string getIp();
  void setIp(string ip);
  uint16_t getPort();
  void setPort(uint16_t port);


private:
  int server_fd;
  string ip;
  uint16_t port;
  struct sockaddr_in adress;
};

#endif