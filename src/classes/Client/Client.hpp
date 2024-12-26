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

  struct sockaddr_in * getAdress () { return (&adress); }
  short getErrorCode () { return (error_code); }
  
  void readRequest ();
  bool parseRequestLine(string & request);

private:
  /* Connection */
  int client_fd;
  struct sockaddr_in adress;
  Server *server_owner;
  short error_code;

  /* Request */
  string method, target_resource, version;
  map<string, string> request_headers;

};

#endif