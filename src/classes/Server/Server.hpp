#ifndef SERVER_H
#define SERVER_H

#include "Route.hpp"
#include "Utils.hpp"

class Route;

class Server
{
public:
  Server ();
  ~Server ();
  Server (const Server &other);
  Server &
  operator= (const Server &other)
  {
    if (this != &other)
      {
        host = other.host;
        port = other.port;
        server_name = other.server_name;
        error_pages = other.error_pages;
        client_max_body_size = other.client_max_body_size;
		routes = other.routes;
      }
    return (*this);
  }

  void init ();

  /* Getter and Setters */
  int getServerFd ();
  uint16_t getPort ();
  void setPort (uint16_t port);
  string getHost ();
  void setHost (string host);
  void setServerName (string server_name);
  string getServerName ();
  void setClientMaxBodySize (size_t client_max_body_size);
  size_t getClientMaxBodySize ();
  void addRoute (Route route);
  vector<Route> getRoutes ();

  void parseServerConfig (const string &line, Server &server);
  void validServerDirective (const string &directive);

  map<int, string> error_pages;

private:
  string host;
  uint16_t port;
  string server_name;
  size_t client_max_body_size;
  vector<Route> routes;
  int server_fd;
  struct sockaddr_in adress;
};

#endif