#include "Server.hpp"

Server::Server ()
{
  host = "";
  server_name = "";
  client_max_body_size = 0;
  memset (&adress, 0, sizeof (adress));
}

Server::Server (const Server &src) 
	:	host (src.host), 
		port (src.port), 
		server_name (src.server_name),
    	client_max_body_size (src.client_max_body_size) {
			  error_pages = src.error_pages;
			  routes = src.routes;
		}

Server::~Server () {}

void
Server::init ()
{
  int directiveValue = 1;

  server_fd = socket (AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
    FATAL_ERROR ("Error creating server's socket");
  setsockopt (server_fd, SOL_SOCKET, SO_REUSEADDR, &directiveValue, sizeof (int));
  memset (&adress, 0, sizeof (adress));
  adress.sin_family = AF_INET;
  adress.sin_addr.s_addr = inet_addr (host.c_str ());
  if (adress.sin_addr.s_addr == (in_addr_t)-1)
    FATAL_ERROR ("Error converting ip address");
  adress.sin_port = htons (port);
  if (bind (server_fd, (struct sockaddr *)&adress, sizeof (adress)) < 0)
    FATAL_ERROR ("Error binding server's socket");
  if (listen (server_fd, QUEUED_CONNECTIONS) < 0)
    FATAL_ERROR ("Error listening on server's socket");
}

void
Server::parseServerConfig (const string &line, Server &server)
{
  istringstream serverStream (line);
  string directiveName, directiveValue;
  serverStream >> directiveName;

  lineTreatment(directiveValue);
  if (directiveName == "host")
    {
      serverStream >> directiveValue;
      server.setHost (directiveValue);
    }
  else if (directiveName == "listen")
    {
      u_int16_t directiveValue;

      serverStream >> directiveValue;
      server.setPort (directiveValue);
    }
  else if (directiveName == "server_name")
    {
      serverStream >> directiveValue;
      server.setServerName (directiveValue);
    }
  else if (directiveName == "error_page")
    {
      int code;
      string path;

      serverStream >> code >> path;
	  lineTreatment(path);
      server.error_pages[code] = path;
    }
  else if (directiveName == "client_max_body_size")
    {
      size_t directiveValue;

      serverStream >> directiveValue;
      server.setClientMaxBodySize (directiveValue);
    }
}

int
Server::getServerFd ()
{
  return (server_fd);
}

uint16_t
Server::getPort ()
{
  return (port);
}

void
Server::setPort (uint16_t port)
{
  this->port = port;
}

string
Server::getHost ()
{
  return (host);
}

void
Server::setHost (string host)
{
  this->host = host;
}

void
Server::setServerName (string server_name)
{
  this->server_name = server_name;
}

string
Server::getServerName ()
{
  return (server_name);
}

void
Server::setClientMaxBodySize (size_t client_max_body_size)
{
  this->client_max_body_size = client_max_body_size;
}

size_t
Server::getClientMaxBodySize ()
{
  return (client_max_body_size);
}

void
Server::addRoute (Route route)
{
  routes.push_back (route);
}

vector<Route>
Server::getRoutes ()
{
  return (routes);
}