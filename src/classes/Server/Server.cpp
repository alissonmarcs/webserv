#include "Server.hpp"

Server::Server () : host (""), port (0), server_name (""), client_max_body_size (0), routes (vector<Route>())
{
  error_pages = map<int, string>();
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
Server::validServerDirective (const string &directive)
{
  if (directive == "host" || directive == "listen" || directive == "server_name" || directive == "error_page" || 
  		directive == "client_max_body_size" || directive == "location" || directive == "return" || directive == "server" ||
		directive == "}" || directive == "{")
	return ;
  std::cerr << "Invalid directive: " << directive << std::endl;
  throw std::invalid_argument("Invalid directive: " + directive);
}

void
Server::checkServerValues(Server &server)
{
	if (server.getHost().empty())
		throw ConfigParserException("Error: missing host directive");
	if (server.getPort() == 0)
		throw ConfigParserException("Error: missing listen directive");
	if (server.getServerName().empty())
		throw ConfigParserException("Error: missing server_name directive");
	if (server.routes.empty())
		throw ConfigParserException("Error: missing location directive");
	if (server.getClientMaxBodySize() == 0)
		throw ConfigParserException("Error: missing client_max_body_size directive");
}

void
Server::validatePort (int directiveValue)
{
  if (directiveValue < 0 || directiveValue > 65535)
		throw ConfigParserException("Error: invalid port number");
}

void
Server::parseServerConfig (const string &line, Server &server)
{
  istringstream serverStream (line);
  string directiveName, directiveValue;
  serverStream >> directiveName;

  validServerDirective(directiveName);
  lineTreatment(directiveValue);
  if (directiveName == "host")
    {
      serverStream >> directiveValue;
      server.setHost (directiveValue);
    }
  else if (directiveName == "listen")
    {
      int directiveValue;

	  serverStream >> directiveValue;

	  validatePort (directiveValue);
      server.setPort (static_cast<uint16_t>(directiveValue));
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

void
Server::addRoute (Route route)
{
	for (vector<Route>::iterator it = routes.begin(); it != routes.end(); it++)
		if (it->getPath() == route.getPath())
			throw ConfigParserException("Error: duplicate location directive");
  routes.push_back (route);
}
