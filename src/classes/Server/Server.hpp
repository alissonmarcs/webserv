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
		server_fd = other.server_fd;
		adress = other.adress;
		isPortSet = other.isPortSet;
      }
    return (*this);
  }

  void init ();

  /* Setters */
  void setPort (uint16_t port) { this->port = port; }
  void setHost (string host) { isValidIp(host) ? this->host = host : throw ConfigParserException("Error: invalid host"); }
  void setServerName (string server_name) { this->server_name = server_name; }
  void setClientMaxBodySize (size_t client_max_body_size) { this->client_max_body_size = client_max_body_size; }
  void setIsPortSet (bool isPortSet) { this->isPortSet = isPortSet; }

  /* Getters */
  uint16_t getPort () const { return port; }
  string getHost () const { return host; }
  string getServerName () const { return server_name; }
  size_t getClientMaxBodySize () const { return client_max_body_size; }
  vector<Route> getRoutes () const { return routes; }
  int getServerFd () const { return server_fd; }
  struct sockaddr_in getAdress () const { return adress; }
  bool getIsPortSet () const { return isPortSet; }

  void parseServerConfig (const string &line, Server &server);
  void validServerDirective (const string &directive);
  void checkServerValues(Server &server);
  void validatePort (int directiveValue);
  void addRoute (Route route);

  map<int, string> error_pages;

  //checking for duplicate directives
  map<string, bool> directiveStatus;

  //checking for duplicated error pages
  map<int , string> errorPagesMap;

private:
  string host;
  uint16_t port;
  string server_name;
  size_t client_max_body_size;
  vector<Route> routes;
  int server_fd;
  struct sockaddr_in adress;
  bool isPortSet;
};

#endif